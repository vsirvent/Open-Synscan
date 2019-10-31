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


/**
 * Will be called when WiFi hardware and software initialization was finished
 * and system initialization was completed
 */
void ready() {

	Logger::notice("READY!");
	Logger::notice("------------------------");
	Logger::notice("SynScan init");
	Logger::notice("------------------------");
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
	Serial.systemDebugOutput(true);
	Logger::setLogLevel(Logger::NOTICE);

	// Soft access point
	WifiAccessPoint.enable(true);
	WifiAccessPoint.setIP(IPAddress("192.168.5.1"));
	WifiAccessPoint.config("SynScan", "12345678", AUTH_WPA2_PSK, false, 5);

	WifiStation.enable(true);
	WifiStation.enableDHCP(true);
	WifiStation.config(WIFI_STATION_SSID, WIFI_STATION_PASS);

	System.onReady(ready);
}
