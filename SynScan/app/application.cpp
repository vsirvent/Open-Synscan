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
	// Soft access point
	WifiStation.enable(false);
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("SynScan", "12345678", AUTH_WPA2_PSK);

	Logger::notice("READY!");
	Logger::notice("AP. ip: %s mac: %s", WifiAccessPoint.getIP().toString().c_str(), WifiAccessPoint.getMAC().c_str());

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
	Serial.systemDebugOutput(false);
	Logger::setLogLevel(Logger::NOTICE);

	System.onReady(ready);
}
