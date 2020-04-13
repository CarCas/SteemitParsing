#ifndef P19_SEQ_CLOCK_HPP
#define P19_SEQ_CLOCK_HPP 

#include <chrono>

using hrc = std::chrono::high_resolution_clock;

namespace timer
{

    hrc::time_point now;

    auto start(){
		now = hrc::now();
		return now;
	}

	auto micro_step(hrc::time_point timer_now){
		return std::chrono::duration_cast<std::chrono::microseconds>(start() - timer_now).count();
	}

	auto milli_step(hrc::time_point timer_now){
		return std::chrono::duration_cast<std::chrono::milliseconds>(start() - timer_now).count();
	}
}

#endif // P19_SEQ_CLOCK_HPP
