# USB-IO

A USB-connected GPIO board running an experimental Lua framework for IoT.

Built on top of FreeRTOS and Arduino, this project implements a cooperative multithreading scheduler based on Lua coroutines. This allows for a concurrent, threaded programming style while maintaining compatibility with non-reentrant Arduino libraries.

## Project Structure
```
 .
 ├── include
 ├── lib
 │   ├── lua        # Lua language
 │   └── luatt      # Arduino/Lua glue
 ├── lua
 │   ├── lib        # Lua external dependencies
 │   ├── src        # Lua source code
 │   └── luatt.py   # Linux host-side driver program
 ├── src
 ├── README.md      # this file
 ├── BUILDING.md    # build instructions
 └── platformio.ini # project settings
```

## System Architecture

The microcontroller is permanently attached to a Linux host via USB. This design is intended for building automation, security, and other fixed infrastructure, not mobile battery-powered devices.

The system has two parts: a Linux host and a microcontroller device. Repurposed wifi routers running OpenWrt make nice hosts for permanent installs, but Mac and Linux computers are convenient for development. The host runs [luatt.py](https://github.com/ryanrsrs/usb-io/blob/main/lua/luatt.py) which handles all communication with the microcontroller (via USB virtual serial port).

## Why Lua?

* **Cooperative multithreading with Lua coroutines**  
Under the hood, Lua uses setjmp/longjmp to transfer control between threads. This allows an easy, threaded programming style while safely reusing Arduino libraries. Because program control flow switches only at well-definied points, there is no need for synchronization in most threads.

* **Lua REPL**  
From the host, you can run a Lua REPL on the microcontroller. This is a little Lua shell running on the live system. The shell has a command history and line editing, and your commands are executed concurrently with whatever other Lua code is already running. Since it's cooperatively scheduled, you can call functions that interact with hardware, internal APIs, pull stats from the memory manager, etc. It's not a brittle debug view where you can look but not touch.

* **Fast updates**  
Updating the Lua code is extremely fast. You can wipe the Lua state, reset peripherals back to inital values, and load a new Lua application in ~100 ms. It feels instant. You do not need to reflash the microcontroller, or even reset it, when doing development in Lua. 

* **Erlang-style live code updates**  
It's possible to have two versions of a module running at the same time, including controlled handoff of state.  
Future.

* **Hardware-in-the-loop testing**  
Future.


## Luatt.py
Runs on the host and talks to the microcontroller. All communication with the micro flows through this script.

* **Loading Lua code**  
The microcontroller starts with no Lua code loaded. All Lua code must be loaded in over USB.    
Luatt.py can load single .lua files as well as zip files containing several Lua sources and a loading script to specify load order. See [luatt.py source](https://github.com/ryanrsrs/usb-io/blob/main/lua/luatt.py) for details.

* **MQTT Proxy**  
Luatt.py can pass messages between the microcontroller and an MQTT Broker on the network. This allows Lua code to publish messages and subscribe to topics.

* **REPL**  
Luatt.py provides a Lua shell with line editing and command history, running on the microcontroller.

* **Luatt.py secondary processes**  
Luatt.py processes can connect to each other using a unix socket. This lets you connect multiple Lua shells to a single microcontroller. They all run cooperatively in the same Lua context.

* **Logging**  
Future.

See [luatt.py source](https://github.com/ryanrsrs/usb-io/blob/main/lua/luatt.py) for command line options and more details.


## Hardware
* [Nordic nRF52840](https://www.nordicsemi.com/Products/nRF52840) on an [Adafruit ItsyBitsy dev board](https://www.adafruit.com/product/4481)
* Linx KH3 keyfob receiver [datasheet.pdf](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FRXM-418-KH3%7FC%7Fpdf%7FEnglish%7FENG_DS_RXM-418-KH3_C.pdf%7FRXM-418-KH3)
* 6 [NMOS outputs](https://www.digikey.com/en/products/detail/alpha-omega-semiconductor-inc/AO3422/1855787) for driving small loads or relay coils.
* [PCT2075 temperature sensor](https://www.nxp.com/part/PCT2075GV)
* [Magnetic buzzer](https://www.digikey.com/en/products/detail/pui-audio-inc/SMT-0931-S-R/5011411)


## Schematic
[<img src="https://github.com/ryanrsrs/usb-io/blob/main/board-schematic.png?raw=true" width="50%" height="50%">](https://github.com/ryanrsrs/usb-io/blob/main/board-schematic.png)


## License

MIT License. See file LICENSE in this directory.


## Author

Ryan Salsbury  
ryanrs@gmail.com
