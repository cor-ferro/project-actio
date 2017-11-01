#include "./stats.h"
#include <iostream>
#include <iomanip>

namespace Renderer {

Stats::Stats()
{
	timeTick1_ = timeTick2_ = timeFrame_ = SystemClock::now();
	msFrame = 0.0;
	fps = 0;
}

void Stats::startTime()
{
	timeTick1_ = boost::chrono::system_clock::now();
}

void Stats::updateTime()
{
	fps++;
	timeTick2_ = boost::chrono::system_clock::now();

	boost::chrono::microseconds tick = boost::chrono::duration_cast<boost::chrono::microseconds>(timeTick2_ - timeTick1_);
	boost::chrono::milliseconds frameTick = boost::chrono::duration_cast<boost::chrono::milliseconds>(timeTick2_ - timeFrame_);

	if (frameTick.count() >= 1000.0) {
		std::cout << '\r' 
			<< std::setw(2) << msFrame << "ms, " 
			<< fps << "fps, "
			<< Geometry::GetTotalCountVertices() << " vertices("
			<< (double(Geometry::VertexSize() * Geometry::GetTotalCountVertices()) / 1024.0 / 1024.0) << "mb)"
			<< std::flush;

		timeFrame_ = timeTick2_;
		fps = 0;
	}

	msFrame = tick.count() / 1000.0;
}

}


