#!/usr/bin/env python3

# Luatt CLI
#
# Loads Lua files onto the microcontroller over USB.
# Provides a Lua REPL running on the microcontroller.
#
# Usage:
#   Connect to the microcontroller via USB. Listen on a unix socket for
#   downstream luatt.py processes.
#
#   $ python3 luatt.py /dev/cu.usbserial --mqtt=192.168.1.1:1883
#
#
#   Connect to another luatt.py process using a unix domain socket. The
#   upstream process will forward our commands to the microcontroller.
#
#   $ python3 luatt.py /tmp/luatt.cu.usbserial
#
#   The idea between these two modes is that usually luatt.py will load Lua
#   files onto the micro, connect the MQTT broker, and do some logging, but
#   generally not show a REPL.
#
#   Later if you decide you do need to poke around with the REPL, you can
#   start a downstream luatt.py process to do it. That way you don't have
#   to kill and restart the main luatt.py (e.g. to enable the REPL).
#
#
#
# Options:
#   --mqtt=ipaddr:port
#       Connect to MQTT broker and act as a proxy so the microcontroller
#       can publish and subscribe. Only valid if connected directly to the
#          microcontroller (i.e. not from a downstream luatt.py process).
#
#   -r  Reset microcontroller Lua state. Does not do a hardware reset, but
#       does set hardware peripherals to their initial state. Clears all
#       Lua variables, objects, frees memory, etc.
#
#   filename.lua    Load Lua file onto microcontroller and run it.
#
#   Loader.cmd      Text file with a list of .lua files to load. Loads
#                   and runs the lua files in the order given.
#
#   archive.luaz    Zip file with a Loader.cmd and some .lua files inside.
#
#
# The REPL:
#   lua> print(2+3*4)
#   14
#   lua> a = 55
#   lua> print(a*10)
#   550
#
# REPL meta commands, they work like the command line options.
#   !reset
#   !load file.lua


import ctypes
import fcntl
import json
import os
import queue
import re
import readline
import secrets
import select
import shlex
import socket
import socketserver
import stat
import sys
import termios
import threading
import time
import zipfile

try:
    import paho.mqtt.client as paho_client
except ImportError:
    # paho mqtt not installed
    paho_client = None

Subscriptions = set()

QS = {}
ReplQ = queue.Queue(20)

Downstreams = {}

LogFile = None
LogColor = False

Quit = False
Force_Update = False

# Add API readline.forced_update()
#
# We need this to scroll the output while the user is typing at the prompt.
# New output is inserted above the line you're editing, keeping it from
# getting mixed up.
version = readline._READLINE_LIBRARY_VERSION
if 'EditLine' in version:
    # libedit on MacOS
    lib = ctypes.cdll.LoadLibrary('libedit.3.dylib')
else:
    lib = ctypes.cdll.LoadLibrary('libreadline.so.' + version)
readline.forced_update = lib.rl_forced_update_display

# Use multi-colored output for the debugging output.
# TODO: add options to control debug output and logging
def print_line(msg, label='', color=''):
    start = ''
    end = ''
    if LogColor and color:
        start = f'\033[{color}m'
        end = f'\033[0m'
    if Enable_REPL and LogFile == sys.stdout:
        sys.stdout.write(f"\033[2K\r{start}{label}{msg}{end}\n")
        if Force_Update: readline.forced_update()
    elif LogFile:
        #LogFile.write(f"{label}{msg}\n")
        LogFile.write(f"{start}{label}{msg}{end}\n")

def printer_serial(msg, label='', color=''):
    if not LogFile: return
    if not label: label = "ser>  "
    if not color: color = 32
    print_line(msg, label, color)

def printer_sock(msg, label='', color=''):
    if not LogFile: return
    if not label: label = "sock> "
    if not color: color = 33
    print_line(msg, label, color)


# Open USB serial connection to microcontroller.
# Use canonical mode to read line-at-a-time from the micro,
# not raw mode.
#
# TODO: test if you can publish emojis to the mqtt broker.
def open_serial(path):
    fd = os.open(path, os.O_RDWR | os.O_NOCTTY)

    # [iflag, oflag, cflag, lflag, ispeed, ospeed, cc]
    tt = termios.tcgetattr(fd)

    tt[0] = termios.IGNBRK | termios.IGNPAR
    tt[1] = 0
    tt[2] = termios.CS8 | termios.CREAD | termios.CLOCAL | termios.HUPCL
    tt[3] = termios.ICANON
    tt[4] = termios.B9600
    tt[5] = termios.B9600
    for i in range(termios.VEOL2+1):
        tt[6][i] = b'\0'

    termios.tcflush(fd, termios.TCIFLUSH)
    termios.tcsetattr(fd, termios.TCSANOW, tt)
    return fd

# Open socket connection to upstream luatt.py process.
def open_socket(path):
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.connect(path)
    return sock


# Open device and determine if we're talking to a USB serial device or
# a unix socket.
Conn = { 'path': sys.argv[1] }
mode = os.stat(Conn['path']).st_mode
if stat.S_ISCHR(mode):
    Conn['fd'] = open_serial(Conn['path'])
    Conn['is_socket'] = False
elif stat.S_ISSOCK(mode):
    Conn['sock'] = open_socket(Conn['path'])
    Conn['fd'] = Conn['sock'].fileno()
    Conn['is_socket'] = True

if Conn['is_socket']:
    # Downstream luatt.py processes don't proxy mqtt messages.
    paho_client = None
elif paho_client is not None:
    # Only create when talking to serial device.
    paho_client = paho_client.Client()

# We use a mostly-text protocol to talk to the microcontroller.
#
# The message format is a variable number of fields separated by '|'
# and terminated with a newline.
#   "field1|field2|field3\n"
#
# If a particular field contains control characters, newlines, '|',
# or starts with '&', then it cannot be included directly. Instead,
# those fields are encoded as "&NNN" where NNN is the size in bytes
# of the raw field. Each raw field is then be appended to the command,
# each followed by a newline.
#
# Example, encode the following 4 fields:
#   "Hello World!"
#   "123"
#   "bad\tchar"
#   "embedded\n123"
#
# Result:
#   "Hello World!|123|&8|&12\nbad\tchar\nembedded\n123\n"
#
# Encoding rationale:
# - Most short bits of text and json won't need to be escaped.
# - Operate the USB serial port in line mode.
# - Simple to handle both on the microcontroller and in python.

# Check if a field needs to be escaped.
def is_clean(b):
    if not b: return True
    if b[0] == b'&'[0]: return False
    for ch in b:
        if ch < 32 or ch > 126 or ch == b'|'[0]:
            return False
    return True

# Escape field only if needed.
# Returns (field, trailer)
#   "123"  => ("123", None)
#   "1\t3" => ("&3", "1\t3")
def escape_arg(s):
    bstr = s.encode('utf-8')
    if is_clean(bstr):
        return (bstr, None)
    else:
        return (f'&{len(bstr)}'.encode('utf-8'), bstr)

# Each command sent to the microcontroller has a random token.
# When the microcontroller sends a reply, it will include the token.
# This tells luatt.py how to route the reply, for example, some
# replies will need to be forwarded to a downstream luatt.py over
# the unix sock, if that's where the request originated.
#
# Asynchronous output from background Lua threads is sent with the
# token "sched".
def new_token():
    return f"{os.getppid()}/{os.getpid()}/{secrets.token_hex(12)}"

# Encodes a command and sends it upstream, either to the microcontroller,
# of an upstream luatt.py process.
def write_command(fd, token, *args):
    out = [[token.encode('utf-8')]]
    for arg, raw in map(escape_arg, args):
        out[0].append(arg)
        if raw is not None: out.append(raw)
    line = b'|'.join(out[0])
    out[0] = line
    out.append(b'')
    out = b'\n'.join(out)

    # TODO: add real logging here
    line = line.decode('utf-8')
    if LogFile:
        if Conn['fd'] == fd:
            if Conn['is_socket']:
                label, color = "sock< ", 36
            else:
                label, color = "ser<  ", 35
        else:
            label, color = ">sock ", 94
        print_line(line, label, color)

    # atomic write
    os.write(fd, out)
    return token

# Read input from the microcontroller or socket, and
# splits it into lines.
#
# fd        File descriptor to read.
# partial   Buffered input that has already been read.
#
# Returns (one_full_line, next_partial)
# Pass next_partial on the next call to read_packet_line or
# read_packet_bytes.
def read_packet_line(fd, partial):
    if '\n' in partial:
        # we can satisfy request entirely from buffered input
        return partial.split('\n', 1)
    line = [partial]
    while not Quit:
        rd, wr, ex = select.select([fd], [], [fd])
        if fd not in rd: continue
        buf = os.read(fd, 4096)
        bd = buf
        try:
            buf = bd.decode('utf-8')
        except UnicodeDecodeError:
            print('\r\n\n----------\r\n')
            print('Error: utf-8 decode failed.')
            print(len(bd))
            print(repr(bd))
            print('----------\r\n\r\n')
            raise
        if not buf: continue
        parts = buf.split('\n', 1)
        line.append(parts[0])
        if len(parts) == 1: continue
        return (''.join(line), parts[1])
    # quitting...
    return None

# Read a fixed number of bytes from a file descriptor, followed by newline.
def read_packet_bytes(fd, n, partial):
    if len(partial) >= n + 1:
        # skip newline
        return (partial[:n], partial[n+1:])
    line = [partial]
    n -= len(partial)
    while not Quit and n >= 0:
        rd, wr, ex = select.select([fd], [], [fd])
        if fd not in rd: continue
        buf = os.read(fd, 4096).decode('utf-8')
        #print_line(f"read2({repr(buf)})")
        if not buf: continue
        #SerialLog.write(buf)
        #SerialLog.flush()
        line.append(buf)
        n -= len(buf)
    if Quit:
        return None
    line = ''.join(line)
    return (line[:n], line[n:][1:]) # skip newline

# Microcontroller publishes MQTT message.
def dev_cmd_pub(cmd):
    if len(cmd) != 4:
        print("Error: pub needs 4 args.")
        return
    topic = cmd[2]
    payload = cmd[3]
    paho_client.publish(topic, payload)

# Microcontroller subscribes to MQTT topic.
def dev_cmd_sub(cmd):
    if len(cmd) != 3:
        print("Error: sub needs 3 args.")
        return
    topic = cmd[2]
    Subscriptions.add(topic)
    paho_client.subscribe(topic)

# Microcontroller unsubscribes.
def dev_cmd_unsub(cmd):
    if len(cmd) != 3:
        print("Error: unsub needs 3 args.")
        return
    topic = cmd[2]
    if topic == '*':
        for topic in Subscriptions:
            paho_client.unsubscribe(topic)
        Subscriptions.clear()
    else:
        paho_client.unsubscribe(topic)
        Subscriptions.discard(topic)

# Parse a command from the microcontroller.
def process_serial_packet(packet):
    packet = tuple(packet)
    if len(packet) < 2:
        # mostly log text output
        print_line(repl(packet), "pckt> ", 103)
        return
    token = packet[0]
    cmd = packet[1]

    # MQTT commands
    if paho_client and cmd == 'pub':
        dev_cmd_pub(packet)
        return
    elif paho_client and cmd == 'sub':
        dev_cmd_sub(packet)
        return
    elif paho_client and cmd == 'unsub':
        dev_cmd_unsub(packet)
        return
    else:
        # check if token is ours
        q = QS.get(token)
        if q is not None:
            q.put(packet)
        elif token.split('/')[0] == str(os.getppid()):
            body = '|'.join(packet[1:])
            if Enable_REPL:
                sys.stdout.write(f"\033[2K\r{body}\n")
                if Force_Update: readline.forced_update()
            else:
                sys.stdout.write(body + "\n")

    # check if token came from a downstream luatt.py
    ds = Downstreams.get(token)
    if ds:
        write_command(ds, *packet)

    #sys.stdout.write(f"Error: unrecognized dev cmd {repr(cmd)}\n")
    return

# Read one packet from file descriptor.
def read_packet(fd, partial, printer):
    try:
        v = read_packet_line(fd, partial)
        if v is None: return
    except OSError:
        return

    line, partial = v

    if 'error' in line.lower():
        printer(line, color=91)
    else:
        printer(line)

    fields = []
    for f in line.split('|'):
        if f[:1] == '&':
            n = int(f[1:])
            v = read_packet_bytes(fd, n, partial)
            if v is None: return
            fields.append(v[0])
            partial = v[1]
        else:
            fields.append(f)
    return (fields, partial)

# Thread to read packets from the serial port or unix socket.
def read_serial():
    partial = ''
    while not Quit:
        v = read_packet(Conn['fd'], partial, printer_serial)
        if not v:
            partial = ''
            continue

        packet, partial = v
        process_serial_packet(packet)

# Main thread calls this to wait for a reply after sending a command.
# Blocks until the read_serial() thread routes the reply to out queue.
def wait_for_ret(q, token):
    while not Quit:
        v = q.get()
        body = '|'.join(v[1:])
        if v[0] == token:
            sys.stdout.write(body + "\n")
            if v[1] == 'ret':
                return v
    return None

# Main thread calls this after first connecting. We expect the
# microcontroller to send a version line automatically.
def wait_for_version(q):
    while not Quit:
        v = q.get()
        if v[0] == 'sched' and v[1] == 'version':
            sys.stdout.write('|'.join(v) + "\n")
            return v
    return None

if not Conn['is_socket']:
    if 0:
        # only log the primary luatt.py
        log_name = f"/tmp/luatt.{os.getpid()}.log"
        # use line buffering
        LogFile = open(log_name, 'w', buffering=1)

if LogFile:
    LogColor = LogFile.isatty()
else:
    LogColor = sys.stdout.isatty()
Enable_REPL = sys.stdout.isatty()

th_serial = threading.Thread(target=read_serial, daemon=True)

if not Conn['is_socket']:
    QS['sched'] = ReplQ
    th_serial.start()
    wait_for_version(ReplQ)
    del QS['sched']
else:
    th_serial.start()
    write_command(Conn['fd'], "noret", "reconnect", str(os.getppid()))


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code):
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    print_line(f"Connected with result code {reason_code}", 'mqtt  ', 106)
    for topic in Subscriptions:
        client.subscribe(topic)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print_line(msg.topic+" "+str(msg.payload), 'mqtt  ', 106)
    write_command(Conn['fd'], "noret", "msg", msg.topic, msg.payload.decode('utf-8'))

# Socket server to handle unix domain connections from downstream luatt.py processes.
# Each downstream connected luatt.py gets a new thread to babysit it.
class SocketHandler(socketserver.BaseRequestHandler):
    def handle(self):
        last_token = None
        partial = ''
        while not Quit:
            v = read_packet(self.request.fileno(), partial, printer_sock)
            if not v: break
            packet, partial = v

            if last_token:
                del Downstreams[last_token]
                last_token = None
            token = packet[0]
            if token != '' and token != 'noret':
                Downstreams[token] = self.request.fileno()
                last_token = token
            write_command(Conn['fd'], *packet)
        if last_token:
            del Downstreams[last_token]

if not Conn['is_socket']:
    # Create unix socket for downstream luatt.py processes to connect to.
    # Only do this if we're talking directly to the USB serial device.
    #
    # Unix socket paths
    #
    # 508 ~/usb-io/lua$ ls -l /dev/cu.usbmodemFD114301 /tmp/luatt.*
    # crw-rw-rw-  1 root  wheel  0x16000005 Oct 15 12:12 /dev/cu.usbmodemFD114301
    # srwxr-xr-x  1 ryan  wheel           0 Oct 15 09:49 /tmp/luatt.97372
    # lrwxr-xr-x  1 ryan  wheel          11 Oct 15 09:49 /tmp/luatt.cu.usbmodemFD114301 -> luatt.97372
    sock_name = f"luatt.{os.getpid()}"
    link_name = f"luatt.{os.path.basename(Conn['path'])}"
    tmp_dir = '/tmp'
    sock_path = os.path.join(tmp_dir, sock_name)
    link_path = os.path.join(tmp_dir, link_name)

    Server = socketserver.ThreadingUnixStreamServer(sock_path, SocketHandler)

    if os.path.islink(link_path):
        # remove existing symlink
        os.unlink(link_path)
    os.symlink(sock_name, link_path)

    Server_thread = threading.Thread(target=Server.serve_forever, daemon=True)
    Server_thread.start()
else:
    Server = None
    Server_thread = None


re_name_eq = re.compile('^([_a-zA-Z][_a-zA-Z0-9]*)=')

def find_loader_cmd(z):
    subdir_loader = None
    for info in z.infolist():
        if info.filename == "Loader.cmd":
            return info
        path, base = os.path.split(info.filename)
        if base != "Loader.cmd": continue
        if '/' in path: continue # only look one dir level
        if subdir_loader:
            print("Error: luaz archive has multiple Loader.cmd files.")
            return None
        subdir_loader = info
    return subdir_loader

def cmd_reset():
    token = new_token()
    QS[token] = ReplQ
    write_command(Conn['fd'], token, "reset")
    wait_for_ret(ReplQ, token)
    del QS[token]

def split_lua_name(s):
    eq = s.split('=', 1)
    if len(eq) == 2 and '/' not in eq[0]:
        name = eq[0]
        file = eq[1]
    else:
        name = os.path.splitext(os.path.basename(s))[0]
        file = s
    return (name, file)

def load_data(name, data):
    token = new_token()
    QS[token] = ReplQ
    write_command(Conn['fd'], token, "load", name, data)
    wait_for_ret(ReplQ, token)
    del QS[token]

def load_luaz(path):
    with zipfile.ZipFile(path, metadata_encoding='utf-8') as z:
        loader = find_loader_cmd(z)
        if not loader:
            print("Error: Loader.cmd not found.")
            return None
        loader_dir = os.path.split(loader.filename)[0]
        for line in z.read(loader).splitlines():
            line = line.decode('utf-8')
            if not line.strip(): continue
            name, src = split_lua_name(line)
            src_path = os.path.join(loader_dir, src)
            data = z.read(src_path).decode('utf-8')
            load_data(name, data)

def load_loader_cmd(path):
    loader_dir = os.path.split(path)[0]
    for line in open(path).readlines():
        line = line.strip()
        if not line: continue
        name, src = split_lua_name(line)
        src_path = os.path.join(loader_dir, src)
        data = open(src_path).read()
        load_data(name, data)

def cmd_load(cmd):
    if len(cmd) < 2:
        print("Error: !load no arguments given.")
        return
    for arg in cmd[1:]:
        ext = os.path.splitext(arg)[1]
        if ext == '.zip' or ext == '.luaz':
            load_luaz(arg)
            continue
        elif ext == '.cmd':
            load_loader_cmd(arg)
            continue

        name, path = split_lua_name(arg)
        try:
            f = open(path, 'r', encoding='utf-8')
            data = f.read(100 * 1024)
        except OSError as err:
            print(f"Error: !load {err.strerror}")
            return
        load_data(name, data)

def cmd_eval(line):
    token = new_token()
    QS[token] = ReplQ
    write_command(Conn['fd'], token, "eval", line)
    wait_for_ret(ReplQ, token)
    del QS[token]

def parse_line(line):
    if line[:1] != '!':
        cmd_eval(line)
        return True

    args = shlex.split(line)
    if args[0] == '!reset':
        cmd_reset()
        return True
    elif args[0] == '!exit' or args[0] == '!quit':
        return False
    elif args[0] == '!load':
        cmd_load(args)
        return True
    elif args[0] == '!reload':
        pass
    else:
        print('Error: Bad command')
    return True

for arg in sys.argv[2:]:
    if arg[:7] == '--mqtt=':
        arg = arg.split("=", 1)[1]
        if Conn['is_socket']:
            print("Error: cannot proxy MQTT when connecting another luatt.py process.")
            sys.exit(2)

        if paho_client is None:
            print("Error: module paho-mqtt not available.")
            sys.exit(2)

        # Connect to MQTT Broker
        paho_client.on_connect = on_connect
        paho_client.on_message = on_message
        if ":" in arg:
            ip, port = arg.split(":", 1)
            port = int(port)
        else:
            ip, port = arg, 1883
        paho_client.connect(ip, port, 60)
        paho_client.loop_start()
        continue

    if arg == '-r':
        cmd_reset()
        continue

    if arg[:5] == 'eval:':
        cmd_eval(arg[5:])
        continue

    ext = os.path.splitext(arg)[1]
    if ext in ('.lua', '.luaz', '.zip', '.cmd'):
        cmd_load(['load', arg])
        continue

    else:
        print(f"Error: bad command line arg {repr(arg)}")

try:
    if Enable_REPL:
        while True:
            Force_Update = True
            s = input("lua> ")
            Force_Update = False
            if s and not parse_line(s):
                break
    else:
        while True:
            time.sleep(1)
except (EOFError, KeyboardInterrupt):
    print()
    pass
finally:
    Quit = True
    if Server:
        Server.shutdown()
        Server_thread.join()
        os.unlink(link_path)
        os.unlink(sock_path)
    if Conn['is_socket']:
        Conn['sock'].close()
    else:
        os.close(Conn['fd'])
    th_serial.join()
    if LogFile:
        LogFile.close()
