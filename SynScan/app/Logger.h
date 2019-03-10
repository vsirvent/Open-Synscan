

#ifndef _LOGGER_H__
#define _LOGGER_H__

#include "Arduino.h"

class Logger
{
public:
    enum Level
    {
        VERBOSE = 0,
        NOTICE,
        WARNING,
        ERROR,
        FATAL,
        SILENT
    };

    const char* mLevelStrings[6] = {
    		"VERBOSE", "INFO", "WARNING", "ERROR", "FATAL", "SILENT"
    };

    static void setLogLevel(Level level);

    static Level getLogLevel();

    static void verbose(const char* message, ...);
	static void notice(const char* message, ...);
	static void warning(const char* message, ...);
	static void error(const char* message, ...);
	static void fatal(const char* message, ...);

    static void log(Level level, const char* message);

    static Logger* sLogger;
    static Logger* getInstance();

    static const char* asString(Level level);

	IPAddress mRemoteIP;
	uint16_t mRemotePort = 0;
	UdpConnection* mUdp = NULL;

	void onUdpRx(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort);

private:
    Logger();
    virtual ~Logger();
    void operator = (const Logger&);

    void defaultLog(Level level, const char* message);

    Level mLevel = NOTICE;
};

#endif
