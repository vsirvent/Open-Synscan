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

#include "Logger.h"
#include <stdio.h>
#include "Defines.h"

Logger* Logger::sLogger = NULL;

Logger::Logger() {
	mUdp = new UdpConnection(UdpConnectionDataDelegate(&Logger::onUdpRx, this));
	mUdp->listen(Defines::LOG_PORT);
}

Logger::~Logger() {
	delete mUdp;
}

void
Logger::onUdpRx(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort) {
	if (memcmp(data, "log on", 6) == 0) {
		mRemoteIP = remoteIP;
		mRemotePort = remotePort;
	}else if (memcmp(data, "log off", 6) == 0) {
		mRemoteIP = (u32)0;
		mRemotePort = 0;
	}
}

void Logger::setLogLevel(Level level) {
	getInstance()->mLevel = level;
}

Logger::Level Logger::getLogLevel() {
	return getInstance()->mLevel;
}

#define MAX_MSG 128

void Logger::verbose(const char* message, ...) {
	char tmp[MAX_MSG];
	va_list args;
	va_start(args, message);
	m_vsnprintf(tmp, MAX_MSG, message, args);
	va_end(args);
	log(VERBOSE, tmp);
}

void Logger::notice(const char* message, ...) {
	char tmp[MAX_MSG];
	va_list args;
	va_start(args, message);
	m_vsnprintf(tmp, MAX_MSG, message, args);
	va_end(args);
	log(NOTICE, tmp);
}

void Logger::warning(const char* message, ...) {
	char tmp[MAX_MSG];
	va_list args;
	va_start(args, message);
	m_vsnprintf(tmp, MAX_MSG, message, args);
	va_end(args);
	log(WARNING, tmp);
}

void Logger::error(const char* message, ...) {
	char tmp[MAX_MSG];
	va_list args;
	va_start(args, message);
	m_vsnprintf(tmp, MAX_MSG, message, args);
	va_end(args);
	log(ERROR, tmp);
}

void Logger::fatal(const char* message, ...) {
	char tmp[MAX_MSG];
	va_list args;
	va_start(args, message);
	m_vsnprintf(tmp, MAX_MSG, message, args);
	va_end(args);
	log(FATAL, tmp);
}

void Logger::log(Level level, const char* message) {
	getInstance()->defaultLog(level, message);
}

Logger* Logger::getInstance() {
	if (sLogger == NULL) {
		sLogger = new Logger();
	}
	return sLogger;
}

void Logger::defaultLog(Level level, const char* message) {
	if (level >= mLevel && !mRemoteIP.isNull() && mRemotePort != 0) {
		String str = "[";
		str += mLevelStrings[level];
		str += "]";
		str += message;
		str += "\n";
		mUdp->sendStringTo(mRemoteIP, mRemotePort, str);
	}
//	Serial.print("[");
//	Serial.print(mLevelStrings[level]);
//	Serial.print("] ");
//	Serial.println(message);
}

// typedef Logger_<SerialOutput> SerialLogger;
