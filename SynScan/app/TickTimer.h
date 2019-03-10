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
    static const u32 TICK_FREQ = 5000000;//(80000000/16)*1000000;//80mhz => 5000000 Hz;

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
