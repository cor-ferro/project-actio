/*
 * cycle.h
 *
 *  Created on: Oct 9, 2017
 *      Author: demitriy
 */

#ifndef CYCLE_H_
#define CYCLE_H_

#include <GLFW/glfw3.h>
#include <boost/signals2.hpp>
#include "./console.h"

#define CYCLE_DELAY (1000 / 60)
#define CYCLE_TIMER_ID 1

void timerFunction(int timerId);
typedef std::function<void(float time)> CycleCallback;

struct Cycle {
	Cycle(int id) : id_(id) {}

	void static mailLoop() {
		// GLFWwindow * window = glfwGetCurrentContext();
		// while(!glfwWindowShouldClose(window))
		// {

		// 	glfwSwapBuffers(window);
		// 	glfwPollEvents();    
		// }
	}

	void onFrame() {
		
	}

	void addTickHandler(CycleCallback handler) {
		console::warn("add frame handler");
		frameSignals_.connect(handler);
	}

private:
	int id_;
	boost::signals2::signal<void (float time)> frameSignals_;
};

#endif /* CYCLE_H_ */
