Open-Synscan runs on a ESP8266 to control 2 step motors and work with Skywatcher SynScan protocol using WIFI connection.

Tested with EQMOD and Android Synscan PRO.

EQMOD only works with serial comm port, so SerialCom2Wifi app must be used (check the project on github).

* Getting the code and compiling

Open-Synscan runs with Sming Framework (https://github.com/SmingHub/Sming/), so first step is to clone and setup sming enviroment:

```
git clone https://github.com/SmingHub/Sming.git
```

Then clone Open-Synscan:

```
git clone https://github.com/vsirvent/Open-Synscan.git
```

Now we need to edit file "Makefile-user.mk" located at "Synscan" folder and set SMING_HOME folder. 
Edit line:

```
SMING_HOME=#Write Sming folder here. Ex: /home/vicen/src/Sming/Sming/
```

* Compilation
```
make
```

* Flash to ESP8266

Make sure ESP8266 boots in flash mode (plug FLASH jumper in control PCB and reboot).

Then type:

```
make flash
```

* Debug

You can debug using serial port at 115200bps 8n1 or by ethernet network using netcat. To get traces with netcat make the following commands:

```
nc -u [open-synscan-ip] 6667
log on #To start receiving traces
log off #To stop receiving traces
```
* Usage

After boot, open-synscan will create a WIFI network "Synscan" with password "12345678". You can then connect PC computer/tablet or phone, start the control application (EQMOD, SynScan PRO), connect the application to the mount and control it using Synscan protocol.

* Hardware schematics are available here: 

https://circuitmaker.com/Projects/Details/Vicente-Sirvent/SynscanMotor

* 3d print models available also available in "3dFiles" folder.

* Demo video using Cartes du Ciel + EQMOD + SynscanCom2Wifi (https://github.com/vsirvent/SynscanCom2Wifi) to control an Exos-2 mount using WIFI connection:

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/-moP3M088pw/0.jpg)](https://www.youtube.com/watch?v=-moP3M088pw)
