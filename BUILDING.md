# Building

## Dependencies

All external dependencies are handled by git or platformio.

### Packages from the [PlatformIO Registry](https://registry.platformio.org)  
from [platformio.ini](https://github.com/ryanrsrs/usb-io/blob/main/platformio.ini)

* [platformio/nordicnrf52](https://registry.platformio.org/platforms/platformio/nordicnrf52)  
Platform support for Nordic nRF52840.

* [Adafruit nRF52840 ItsyBitsy](https://docs.platformio.org/en/stable/boards/nordicnrf52/adafruit_itsybitsy_nrf52840.html)  
Board support for [Adafruit ItsyBitsy dev board](https://www.adafruit.com/product/4481).

* [Adafruit DotStar](https://registry.platformio.org/libraries/adafruit/Adafruit%20DotStar)  
RGB LED Driver.

* [Adafruit BusIO](https://registry.platformio.org/libraries/adafruit/Adafruit%20BusIO)  
I2C & SPI abstraction.

* [Adafruit PCT2075](https://registry.platformio.org/libraries/adafruit/Adafruit%20PCT2075)  
PCT2075 temperature sensor.

### Other Dependencies  
from [.gitmodules](https://github.com/ryanrsrs/usb-io/blob/main/.gitmodules)

* Lua Language.  
https://github.com/ryanrsrs/lua  
(forked from [lua/lua v5.4.7](https://github.com/lua/lua/tree/v5.4.7))

* Lua Priority Queue.  
https://github.com/ryanrsrs/lpriorityqueue  
(forked from [iskolbin/lpriorityqueue](https://github.com/iskolbin/lpriorityqueue))

* Lua JSON library.  
https://github.com/ryanrsrs/json.lua  
(forked from [rxi/json.lua](https://github.com/rxi/json.lua))

### Future Lua

I made [some changes](https://github.com/ryanrsrs/lua/commit/151a163e48ce0a5b652f85ccd0e5a121de3d863e) to the Lua sources to get it working on Arduino. These changes are tracked on the [luatt branch of ryanrsrs/lua](https://github.com/ryanrsrs/lua/tree/luatt).

When Lua 5.4.8 is released, I'll rebase my changes on the new version.


## Install [PlatformIO](https://platformio.org)
You can use either the [VS Code plugin](https://platformio.org/install/ide?install=vscode) or the pure python [PlatformIO Core CLI](https://platformio.org/install/cli).

```
# Installing PlatformIO Core CLI on Linux

curl -fsSL -o get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py

python3 get-platformio.py

Installer version: 1.2.2
Platform: Linux-6.1.0-20-amd64-x86_64-with-glibc2.36
Python version: 3.11.2 (main, Sep 14 2024, 03:00:30) [GCC 12.2.0]
Python path: /usr/bin/python3
Creating a virtual environment at /home/ryan/.platformio/penv
Updating Python package manager (PIP) in the virtual environment
Requirement already satisfied: pip in ./.platformio/penv/lib/python3.11/site-packages (23.0.1)
Collecting pip
  Downloading pip-24.3.1-py3-none-any.whl (1.8 MB)
     ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 1.8/1.8 MB 13.7 MB/s eta 0:00:00
Installing collected packages: pip
  Attempting uninstall: pip
    Found existing installation: pip 23.0.1
    Uninstalling pip-23.0.1:
      Successfully uninstalled pip-23.0.1
Successfully installed pip-24.3.1
PIP has been successfully updated!
Virtual environment has been successfully created!
Installing PlatformIO Core
Collecting platformio
  Downloading platformio-6.1.16.tar.gz (239 kB)
  Installing build dependencies ... done
  Getting requirements to build wheel ... done
  Preparing metadata (pyproject.toml) ... done
Collecting click<9,>=8.0.4 (from platformio)
  Downloading click-8.1.7-py3-none-any.whl.metadata (3.0 kB)
Collecting colorama (from platformio)
  Downloading colorama-0.4.6-py2.py3-none-any.whl.metadata (17 kB)
Collecting marshmallow==3.* (from platformio)
  Downloading marshmallow-3.23.1-py3-none-any.whl.metadata (7.5 kB)
Collecting pyelftools<1,>=0.27 (from platformio)
  Downloading pyelftools-0.31-py3-none-any.whl.metadata (381 bytes)
Collecting pyserial==3.5.* (from platformio)
  Downloading pyserial-3.5-py2.py3-none-any.whl.metadata (1.6 kB)
Collecting requests==2.* (from platformio)
  Downloading requests-2.32.3-py3-none-any.whl.metadata (4.6 kB)
Collecting semantic-version==2.10.* (from platformio)
  Downloading semantic_version-2.10.0-py2.py3-none-any.whl.metadata (9.7 kB)
Collecting tabulate==0.* (from platformio)
  Downloading tabulate-0.9.0-py3-none-any.whl.metadata (34 kB)
Collecting ajsonrpc==1.2.* (from platformio)
  Downloading ajsonrpc-1.2.0-py3-none-any.whl.metadata (6.9 kB)
Collecting starlette<0.40,>=0.19 (from platformio)
  Downloading starlette-0.39.2-py3-none-any.whl.metadata (6.0 kB)
Collecting wsproto==1.* (from platformio)
  Downloading wsproto-1.2.0-py3-none-any.whl.metadata (5.6 kB)
Collecting bottle==0.13.* (from platformio)
  Downloading bottle-0.13.2-py2.py3-none-any.whl.metadata (1.8 kB)
Collecting uvicorn<0.31,>=0.16 (from platformio)
  Downloading uvicorn-0.30.6-py3-none-any.whl.metadata (6.6 kB)
Collecting packaging>=17.0 (from marshmallow==3.*->platformio)
  Downloading packaging-24.2-py3-none-any.whl.metadata (3.2 kB)
Collecting charset-normalizer<4,>=2 (from requests==2.*->platformio)
  Downloading charset_normalizer-3.4.0-cp311-cp311-manylinux_2_17_x86_64.manylinux2014_x86_64.whl.metadata (34 kB)
Collecting idna<4,>=2.5 (from requests==2.*->platformio)
  Downloading idna-3.10-py3-none-any.whl.metadata (10 kB)
Collecting urllib3<3,>=1.21.1 (from requests==2.*->platformio)
  Downloading urllib3-2.2.3-py3-none-any.whl.metadata (6.5 kB)
Collecting certifi>=2017.4.17 (from requests==2.*->platformio)
  Downloading certifi-2024.8.30-py3-none-any.whl.metadata (2.2 kB)
Collecting h11<1,>=0.9.0 (from wsproto==1.*->platformio)
  Downloading h11-0.14.0-py3-none-any.whl.metadata (8.2 kB)
Collecting anyio<5,>=3.4.0 (from starlette<0.40,>=0.19->platformio)
  Downloading anyio-4.6.2.post1-py3-none-any.whl.metadata (4.7 kB)
Collecting sniffio>=1.1 (from anyio<5,>=3.4.0->starlette<0.40,>=0.19->platformio)
  Downloading sniffio-1.3.1-py3-none-any.whl.metadata (3.9 kB)
Downloading ajsonrpc-1.2.0-py3-none-any.whl (22 kB)
Downloading bottle-0.13.2-py2.py3-none-any.whl (104 kB)
Downloading marshmallow-3.23.1-py3-none-any.whl (49 kB)
Downloading pyserial-3.5-py2.py3-none-any.whl (90 kB)
Downloading requests-2.32.3-py3-none-any.whl (64 kB)
Downloading semantic_version-2.10.0-py2.py3-none-any.whl (15 kB)
Downloading tabulate-0.9.0-py3-none-any.whl (35 kB)
Downloading wsproto-1.2.0-py3-none-any.whl (24 kB)
Downloading click-8.1.7-py3-none-any.whl (97 kB)
Downloading pyelftools-0.31-py3-none-any.whl (180 kB)
Downloading starlette-0.39.2-py3-none-any.whl (73 kB)
Downloading uvicorn-0.30.6-py3-none-any.whl (62 kB)
Downloading colorama-0.4.6-py2.py3-none-any.whl (25 kB)
Downloading anyio-4.6.2.post1-py3-none-any.whl (90 kB)
Downloading certifi-2024.8.30-py3-none-any.whl (167 kB)
Downloading charset_normalizer-3.4.0-cp311-cp311-manylinux_2_17_x86_64.manylinux2014_x86_64.whl (142 kB)
Downloading h11-0.14.0-py3-none-any.whl (58 kB)
Downloading idna-3.10-py3-none-any.whl (70 kB)
Downloading packaging-24.2-py3-none-any.whl (65 kB)
Downloading urllib3-2.2.3-py3-none-any.whl (126 kB)
Downloading sniffio-1.3.1-py3-none-any.whl (10 kB)
Building wheels for collected packages: platformio
  Building wheel for platformio (pyproject.toml) ... done
  Created wheel for platformio: filename=platformio-6.1.16-py3-none-any.whl size=419943 sha256=82350de46b07eb8c80971a74e1ba20e9f386363953e4ec5558b43917592c3181
  Stored in directory: /home/ryan/.cache/pip/wheels/5c/07/02/8eaf6e48aa4805731974ea928cf8c87a1f8b08c0678d1d223a
Successfully built platformio
Installing collected packages: pyserial, pyelftools, bottle, urllib3, tabulate, sniffio, semantic-version, packaging, idna, h11, colorama, click, charset-normalizer, certifi, ajsonrpc, wsproto, uvicorn, requests, marshmallow, anyio, starlette, platformio
Successfully installed ajsonrpc-1.2.0 anyio-4.6.2.post1 bottle-0.13.2 certifi-2024.8.30 charset-normalizer-3.4.0 click-8.1.7 colorama-0.4.6 h11-0.14.0 idna-3.10 marshmallow-3.23.1 packaging-24.2 platformio-6.1.16 pyelftools-0.31 pyserial-3.5 requests-2.32.3 semantic-version-2.10.0 sniffio-1.3.1 starlette-0.39.2 tabulate-0.9.0 urllib3-2.2.3 uvicorn-0.30.6 wsproto-1.2.0

PlatformIO Core has been successfully installed into an isolated environment `/home/ryan/.platformio/penv`!

The full path to `platformio.exe` is `/home/ryan/.platformio/penv/bin/platformio`

If you need an access to `platformio.exe` from other applications, please install Shell Commands
(add PlatformIO Core binary directory `/home/ryan/.platformio/penv/bin` to the system environment PATH variable):

See https://docs.platformio.org/page/installation.html#install-shell-commands
```



## Build the firmware

```
# Activate PlatformIO penv
source ~/.platformio/penv/bin/activate

# Clone repo
git clone --recurse-submodules https://github.com/ryanrsrs/usb-io.git

cd usb-io

# Build everything
pio run

Processing adafruit_itsybitsy_nrf52840 (platform: nordicnrf52; board: adafruit_itsybitsy_nrf52840; framework: arduino)
--------------------------------------------------------------------------
Platform Manager: Installing nordicnrf52
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Platform Manager: nordicnrf52@10.6.0 has been installed!
Tool Manager: Installing platformio/toolchain-gccarmnoneeabi @ >=1.60301.0,<1.80000.0
Downloading  [####################################]  100%          
Unpacking  [####################################]  100%          
Tool Manager: toolchain-gccarmnoneeabi@1.70201.0 has been installed!
Tool Manager: Installing platformio/framework-arduinoadafruitnrf52 @ ~1.10601.0
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Tool Manager: framework-arduinoadafruitnrf52@1.10601.0 has been installed!
Tool Manager: Installing platformio/framework-cmsis @ ~2.50700.0
Downloading  [####################################]  100%          
Unpacking  [####################################]  100%
Tool Manager: framework-cmsis@2.50700.210515 has been installed!
Tool Manager: Installing platformio/tool-adafruit-nrfutil @ ~1.503.0
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Tool Manager: tool-adafruit-nrfutil@1.503.0 has been installed!
Tool Manager: Installing platformio/tool-sreccat @ ~1.164.0
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Tool Manager: tool-sreccat@1.164.0 has been installed!
Tool Manager: Installing platformio/tool-scons @ ~4.40801.0
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Tool Manager: tool-scons@4.40801.0 has been installed!
Library Manager: Installing adafruit/Adafruit DotStar @ ^1.2.5
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Library Manager: Adafruit DotStar@1.2.5 has been installed!
Library Manager: Resolving dependencies...
Library Manager: Installing Adafruit BusIO
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Library Manager: Adafruit BusIO@1.16.2 has been installed!
Library Manager: Installing adafruit/Adafruit PCT2075 @ ^1.0.5
Downloading  [####################################]  100%
Unpacking  [####################################]  100%
Library Manager: Adafruit PCT2075@1.0.5 has been installed!
Library Manager: Resolving dependencies...
Verbose mode can be enabled via `-v, --verbose` option
CONFIGURATION: https://docs.platformio.org/page/boards/nordicnrf52/adafruit_itsybitsy_nrf52840.html
PLATFORM: Nordic nRF52 (10.6.0) > ItsyBitsy nRF52840 Express
HARDWARE: NRF52840 64MHz, 243KB RAM, 796KB Flash
DEBUG: Current (blackmagic) External (blackmagic, cmsis-dap, jlink, stlink)
PACKAGES: 
 - framework-arduinoadafruitnrf52 @ 1.10601.0 (1.6.1) 
 - framework-cmsis @ 2.50700.210515 (5.7.0) 
 - tool-adafruit-nrfutil @ 1.503.0 (5.3) 
 - tool-sreccat @ 1.164.0 (1.64) 
 - toolchain-gccarmnoneeabi @ 1.70201.0 (7.2.1)
LDF: Library Dependency Finder -> https://bit.ly/configure-pio-ldf
LDF Modes: Finder ~ chain, Compatibility ~ soft
Found 18 compatible libraries
Scanning dependencies...
Dependency Graph
|-- Wire @ 1.0
|-- Adafruit DotStar @ 1.2.5
|-- Adafruit BusIO @ 1.16.2
|-- Adafruit PCT2075 @ 1.0.5
|-- luatt
|-- Adafruit TinyUSB Library
Building in release mode
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/Adafruit_TinyUSB_API.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/Adafruit_USBD_CDC.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/Adafruit_USBD_Device.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/Adafruit_USBD_Interface.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/Adafruit_USBH_Host.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/cdc/Adafruit_USBH_CDC.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/hid/Adafruit_USBD_HID.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/midi/Adafruit_USBD_MIDI.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/msc/Adafruit_USBD_MSC.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/msc/Adafruit_USBH_MSC.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/ports/esp32/Adafruit_TinyUSB_esp32.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/ports/nrf/Adafruit_TinyUSB_nrf.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/ports/rp2040/Adafruit_TinyUSB_rp2040.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/ports/samd/Adafruit_TinyUSB_samd.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/video/Adafruit_USBD_Video.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/arduino/webusb/Adafruit_USBD_WebUSB.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/audio/audio_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/bth/bth_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/cdc/cdc_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/cdc/cdc_host.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/dfu/dfu_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/dfu/dfu_rt_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/hid/hid_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/hid/hid_host.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/midi/midi_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/msc/msc_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/msc/msc_host.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/net/ecm_rndis_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/net/ncm_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/usbtmc/usbtmc_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/vendor/vendor_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/class/video/video_device.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/common/tusb_fifo.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/device/usbd.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/device/usbd_control.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/host/hub.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/host/usbh.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/analog/max3421/hcd_max3421.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/microchip/samd/dcd_samd.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/nordic/nrf5x/dcd_nrf5x.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/raspberrypi/pio_usb/dcd_pio_usb.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/raspberrypi/pio_usb/hcd_pio_usb.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/raspberrypi/rp2040/dcd_rp2040.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/raspberrypi/rp2040/hcd_rp2040.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/raspberrypi/rp2040/rp2040_usb.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/portable/synopsys/dwc2/dcd_dwc2.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib68d/Adafruit_TinyUSB_Arduino/tusb.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/src/lua_funcs.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/src/main.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/src/periphs.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/src/util.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib20a/SPI/SPI.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib20a/SPI/SPI_nrf52832.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib2bf/Wire/Wire_nRF52.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib09d/Adafruit BusIO/Adafruit_BusIO_Register.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib09d/Adafruit BusIO/Adafruit_I2CDevice.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib09d/Adafruit BusIO/Adafruit_SPIDevice.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/libbd2/Adafruit DotStar/Adafruit_DotStar.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/liba33/Adafruit PCT2075/Adafruit_PCT2075.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lapi.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lauxlib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lbaselib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lcode.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lcorolib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lctype.c.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/lib20a/libSPI.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ldblib.c.o
Indexing .pio/build/adafruit_itsybitsy_nrf52840/lib20a/libSPI.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ldebug.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ldo.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ldump.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lfunc.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lgc.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/linit.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/liolib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/llex.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lmathlib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lmem.c.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/lib09d/libAdafruit BusIO.a
Indexing .pio/build/adafruit_itsybitsy_nrf52840/lib09d/libAdafruit BusIO.a
Archiving .pio/build/adafruit_itsybitsy_nrf52840/lib2bf/libWire.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/loadlib.c.o
Indexing .pio/build/adafruit_itsybitsy_nrf52840/lib2bf/libWire.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lobject.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lopcodes.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/loslib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lparser.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lstate.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lstring.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lstrlib.c.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/liba33/libAdafruit PCT2075.a
Indexing .pio/build/adafruit_itsybitsy_nrf52840/liba33/libAdafruit PCT2075.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ltable.c.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/libbd2/libAdafruit DotStar.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ltablib.c.o
Indexing .pio/build/adafruit_itsybitsy_nrf52840/libbd2/libAdafruit DotStar.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ltests.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/ltm.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lundump.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lutf8lib.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lvm.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/lua/lzio.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/libc72/luatt/luatt_context.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/libc72/luatt/luatt_io.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/libc72/luatt/luatt_loader.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduinoVariant/variant.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/HardwarePWM.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/IPAddress.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/Print.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/RingBuffer.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/Stream.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/Tone.cpp.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/libFrameworkArduinoVariant.a
Indexing .pio/build/adafruit_itsybitsy_nrf52840/libFrameworkArduinoVariant.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/Uart.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/WInterrupts.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/WMath.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/WString.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/avr/dtostrf.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/abi.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/delay.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/croutine.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/event_groups.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/list.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/portable/MemMang/heap_3.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/queue.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/stream_buffer.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/tasks.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/Source/timers.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/portable/CMSIS/nrf52/port_cmsis.c.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/libc72/libluatt.a
Indexing .pio/build/adafruit_itsybitsy_nrf52840/libc72/libluatt.a
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/portable/CMSIS/nrf52/port_cmsis_systick.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/freertos/portable/GCC/nrf52/port.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/hooks.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/itoa.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/linker/gcc_startup_nrf52.S.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/linker/gcc_startup_nrf52840.S.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/main.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/new.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_gpiote.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_power.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_pwm.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_qspi.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_spim.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_spis.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_temp.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/drivers/src/nrfx_timer.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/mdk/system_nrf52.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/nordic/nrfx/mdk/system_nrf52840.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/pulse.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/pulse_asm.S.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/rtos.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/sysview/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/sysview/SEGGER/SEGGER_RTT.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/sysview/SEGGER/SEGGER_RTT_ASM_ARMv7M.S.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/sysview/SEGGER/SEGGER_RTT_printf.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/sysview/SEGGER/SEGGER_SYSVIEW.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/sysview/SEGGER_SYSVIEW_FreeRTOS.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/utility/AdaCallback.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/utility/SoftwareTimer.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/utility/adafruit_fifo.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/utility/debug.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/utility/utilities.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/wiring.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/wiring_analog.cpp.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/wiring_analog_nRF52.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/wiring_digital.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/wiring_private.c.o
Compiling .pio/build/adafruit_itsybitsy_nrf52840/FrameworkArduino/wiring_shift.c.o
Archiving .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/liblua.a
Indexing .pio/build/adafruit_itsybitsy_nrf52840/lib8a2/liblua.a
Archiving .pio/build/adafruit_itsybitsy_nrf52840/libFrameworkArduino.a
Indexing .pio/build/adafruit_itsybitsy_nrf52840/libFrameworkArduino.a
Linking .pio/build/adafruit_itsybitsy_nrf52840/firmware.elf
Building .pio/build/adafruit_itsybitsy_nrf52840/firmware.hex
Checking size .pio/build/adafruit_itsybitsy_nrf52840/firmware.elf
Advanced Memory Usage is available via "PlatformIO Home > Project Inspect"
RAM:   [          ]   3.6% (used 8924 bytes from 248832 bytes)
Flash: [==        ]  23.3% (used 190120 bytes from 815104 bytes)
Building .pio/build/adafruit_itsybitsy_nrf52840/firmware.zip
Zip created at .pio/build/adafruit_itsybitsy_nrf52840/firmware.zip
====================== [SUCCESS] Took 44.33 seconds ======================
```

## Other build targets

```
pio run --verbose

pio run --target clean

pio run --target upload
```
