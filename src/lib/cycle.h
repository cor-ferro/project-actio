/*
 * cycle.h
 *
 *  Created on: Oct 9, 2017
 *      Author: demitriy
 */

#ifndef CYCLE_H_
#define CYCLE_H_

#include <GL/glew.h>
#include <GL/glut.h>
#include <boost/signals2.hpp>
#include "./console.h"

#define CYCLE_DELAY (1000 / 60)
#define CYCLE_TIMER_ID 1

void timerFunction(int timerId);
typedef std::function<void()> callback;

struct Cycle {
	Cycle(int id) : id_(id) {}

	void static mailLoop() {
		glutTimerFunc(CYCLE_DELAY, timerFunction, CYCLE_TIMER_ID);
		glutMainLoop();
	}

	void onFrame() {
		frameSignals_();
	}

	void addTickHandler(callback handler) {
		console::warn("add frame handler");
		frameSignals_.connect(handler);
	}

private:
	int id_;
	boost::signals2::signal<void ()> frameSignals_;
};

Cycle renderCycle(1);
Cycle inputCycle(2);

void timerFunction(int timerId) {
	renderCycle.onFrame();
	inputCycle.onFrame();
	glutTimerFunc(CYCLE_DELAY, timerFunction, CYCLE_TIMER_ID);
}

#endif /* CYCLE_H_ */
