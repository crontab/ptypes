
#include "ptypes.h"


PTYPES_BEGIN


except::except(const char* imsg)
	: message(imsg)
{
}


except::except(const string& imsg)
	: message(imsg)
{
}


except::~except()
{
}


PTYPES_END
