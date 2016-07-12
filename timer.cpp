#pragma once
#include "timer.h"
#include <ctime>

	timer::timer() {
		start = std::clock();
	};
	double timer::duration() {
		return  (double) (clock() - getStart())/1000;

	}
	void timer::reset() {
		start = std::clock();
	}
	clock_t timer::getStart() {
		return start;
	}
	timer::~timer() {};

