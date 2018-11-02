#include "../../inc/AAMath.h"

#include <boost/random.hpp>

#include <ctime>
#ifdef OS_LINUX
#include <sys/time.h>
#endif

namespace ArmyAnt{

namespace Math{

int32 getRandomInt32Number(int32 begin, int32 end){
	boost::mt19937 engine(time(0));
	boost::uniform_int<int32> dist(begin, end);
	boost::variate_generator<boost::mt19937, boost::uniform_int<int32>> generator(engine, dist);
	return generator();
}

}

}
