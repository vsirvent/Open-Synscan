/*
 * This file is part of the Open Synscan Project distribution (https://github.com/vsirvent/Synscan).
 * Copyright (c) 2019 Vicente Sirvent Orts.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "Logger.h"
#include "MainApp.h"

MainApp* app = NULL;


#ifdef WIFI_STATION
void networkScanCompleted(bool succeeded, BssList list) {
	bool relay_ok = false;
	Serial.print("Synscan::networkScanCompleted start\n");
	if (succeeded) {
		for (int i = 0; i < list.count(); i++) {
			if (list[i].ssid == WIFI_STATION_SSID) {
				Serial.print("Focuser found, connecting...\n");
				WifiStation.config(list[i].ssid, WIFI_STATION_PASS);
				relay_ok = true;
				break;
			}
		}
	}

	if (relay_ok) {
		Serial.print("Synscan::networkScanCompleted: Connecting...\n");
		WifiStation.connect();
	}
	Serial.print("Synscan::networkScanCompleted end\n");
}
#endif

/**
 * Will be called when WiFi hardware and software initialization was finished
 * and system initialization was completed
 */
void ready() {

	Logger::notice("READY!");
	Logger::notice("------------------------");
	Logger::notice("SynScan init");
	Logger::notice("------------------------");

	WifiAccessPoint.setIP(IPAddress("192.168.5.1"));
	app = new MainApp();

	// Set system ready callback method
	Logger::notice("SynScan Init done");
}

/**
 * Initialization function.
 */
void init() {
	System.setCpuFrequency(CPU_FREQ);
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(false);
	Logger::setLogLevel(Logger::NOTICE);

	// Soft access point
	WifiAccessPoint.enable(true);
	WifiAccessPoint.setIP(IPAddress("192.168.5.1"));
	WifiAccessPoint.config("SynScan", "12345678", AUTH_WPA2_PSK);

#ifdef WIFI_STATION
	WifiStation.enable(true);
	WifiStation.startScan(&networkScanCompleted);
#else
	WifiStation.enable(false);
#endif
	System.onReady(ready);
}
