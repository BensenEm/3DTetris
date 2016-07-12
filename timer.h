#pragma once
#include <ctime>

class timer
{
	std::clock_t start;

public:
	timer();
	clock_t getStart();
	double duration();
	void reset();
	
	~timer();
};

