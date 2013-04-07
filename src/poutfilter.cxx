
#include <string.h>

#include "pstreams.h"


PTYPES_BEGIN


outfilter::outfilter(outstm* istm, int ibufsize)
: outstm(false, ibufsize), stm(istm)
{
	if (stm)
		stm->addnotification(this);
}


outfilter::~outfilter()
{
	if (stm)
		stm->delnotification(this);
}


void outfilter::freenotify(component* sender)
{
	if (sender == stm)
	{
		stm = NULL;
		close();
	}
}


void outfilter::doopen()
{
	if (stm && !stm->get_active())
		stm->open();
}


void outfilter::doclose()
{
}


string outfilter::get_errstmname()
{
	if (!stm)
		return get_streamname();
	else
		return get_streamname() + ": " + stm->get_errstmname();
}


void outfilter::set_stm(outstm* istm)
{
	close();
	if (stm)
		stm->delnotification(this);
	stm = istm;
	if (stm)
		stm->addnotification(this);
}


PTYPES_END
