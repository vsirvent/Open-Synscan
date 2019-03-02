#ifndef _TICK_TIMER_H_
#define _TICK_TIMER_H_

#include <Interrupts.h>
#include <Delegate.h>
#include <WVector.h>
#include "Defines.h"

class ITickListener {
public:
	virtual ~ITickListener(){}

	virtual void IRAM_ATTR onTick() = 0;
};

class TickTimer
{
private:
    uint32_t mInternal = 0;
    uint32_t mTicks = 0;
    Vector<ITickListener*> mListener;
    bool mRepeating = false;
    bool mStarted = false;

public:
    static const u32 TICK_FREQ = (CPU_FREQ/16)*1000000;//80mhz => 5000000 Hz;

	TickTimer();
	virtual ~TickTimer();

	void IRAM_ATTR addListener(ITickListener* listener);
	void IRAM_ATTR removeListener(ITickListener* listener);

	bool IRAM_ATTR start(bool repeating = true);
	bool __forceinline IRAM_ATTR startOnce() { return start(false); }
	bool IRAM_ATTR stop();
	bool IRAM_ATTR restart();
	bool isStarted();

	uint32_t getIntervalTicks() const;
    bool IRAM_ATTR setIntervalTicks(uint32_t ticks);

    void IRAM_ATTR onTick();
};

extern TickTimer AppTimer;

#endif
