# OPEN-SYNSCAN

## Intro

Open-Synscan runs on a ESP8266 to control 2 step motors and work with Skywatcher SynScan protocol using WIFI connection.

Tested with EQMOD and Android Synscan PRO.

EQMOD only works with serial comm port, so SerialCom2Wifi app must be used (https://github.com/vsirvent/SynscanCom2Wifi).

## Getting the code

Open-Synscan runs with Sming Framework (https://github.com/SmingHub/Sming/), so first step is to clone and setup sming enviroment:

```
git clone https://github.com/SmingHub/Sming.git
```

Then clone Open-Synscan:

```
git clone https://github.com/vsirvent/Open-Synscan.git
```

## Compilation

Now we need to edit file "Makefile-user.mk" located at "Synscan" folder and set SMING_HOME folder. 
Edit line:

```
SMING_HOME=#Write Sming folder here. Ex: /home/vicen/src/Sming/Sming/
```

```
make
```

## Flash to ESP8266

Make sure ESP8266 boots in flash mode (plug FLASH jumper in control PCB and reboot).

Then type:

```
make flash
```

## Configure motor values

Each mount can have a different number of gear teeth on DEC and RA axis. You can check your mount values at <http://eq-mod.sourceforge.net/prerequisites.html>. You must set the correct values of the gear teeth number and the GT2 wheel teeth numbers at Motor.h constant members:

```
  Motor.h
  ...
	static const u32 MOUNT_RATIO = 144; //The mount gear teeth number
	static const u32 GEAR_RATIO_NUM = 60; //Big GT2 wheel teeth number
	static const u32 GEAR_RATIO_DEN = 16; //Small GT2 wheel teeth number
  ...
```

## Debug

You can debug using serial port at 115200bps 8n1 or by ethernet network receiving UDP packets with netcat. To get traces with netcat type the following commands:

```
nc -u [open-synscan-ip] 6667
log on #To start receiving traces
log off #To stop receiving traces
```
## Usage

After boot, open-synscan will create a WIFI network "Synscan" with password "12345678". You can then connect PC computer/tablet or phone, start the control application (EQMOD, SynScan PRO), connect the application to the mount and control it using Synscan protocol.

## Making the ESP8266 control board and setting up step motors

The PCB control board schematics based on ESP8266 chip is available here:

https://circuitmaker.com/Projects/Details/Vicente-Sirvent/SynscanMotor

![screeshot](https://raw.githubusercontent.com/vsirvent/Open-Synscan/master/Hardware/open-synscan-3d.png)

Motors are mounted in the telescope mount with 3d print models available in "3dFiles" folder. Step motors use gt2 6mm with wheels (60 and 16 teeth) + 158mm gt2 6mm closed belts to move the gears of the mount.

## Demo

This demo video shows Open-Synscan working with Cartes du Ciel + EQMOD + SynscanCom2Wifi (https://github.com/vsirvent/SynscanCom2Wifi) to control a Meade LXD75 mount using WIFI connection:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/-moP3M088pw/0.jpg)](https://www.youtube.com/watch?v=-moP3M088pw)
