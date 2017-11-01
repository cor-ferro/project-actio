/*
 * stats.h
 *
 *  Created on: Oct 8, 2017
 *      Author: demitriy
 */

#ifndef STATS_H_
#define STATS_H_

#include <boost/chrono/chrono.hpp>
#include <boost/timer/timer.hpp>
#include "../lib/console.h"
#include "../core/geometry.h"

namespace Renderer {

typedef boost::chrono::system_clock SystemClock;

struct Stats {
	Stats();

	void startTime();
	void updateTime();

	double fps;
	double msFrame;

private:
	SystemClock::time_point timeTick1_;
	SystemClock::time_point timeTick2_;
	SystemClock::time_point timeFrame_;
};

}

#endif /* STATS_H_ */
