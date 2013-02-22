
#include "ptypes.h"


PTYPES_BEGIN


component::component()
	: unknown(), refcount(0), freelist(nil) {}


component::~component()
{
	if (freelist != nil)
	{
		for (int i = 0; i < freelist->get_count(); i++)
			(*freelist)[i]->freenotify(this);
		delete freelist;
		freelist = nil;
	}
}


void component::freenotify(component*)
{
}


void component::addnotification(component* obj)
{
	if (freelist == nil)
		freelist = new tobjlist<component>(false);
	freelist->add(obj);
}


void component::delnotification(component* obj)
{
	int i = -1;
	if (freelist != nil)
	{
		i = freelist->indexof(obj);
		if (i >= 0)
		{
			freelist->del(i);
			if (freelist->get_count() == 0)
			{
				delete freelist;
				freelist = nil;
			}
		}
	}
	if (i == -1)
		fatal(CRIT_FIRST + 1, "delnotification() failed: no such object");
}


component* component::addref()
{
	if (this)
#ifdef PTYPES_ST
		refcount++;
#else
		pincrement(&refcount);
#endif
	return this;
}


bool ptdecl component::release()
{
	if (this)
	{
#ifdef PTYPES_ST
		if (--refcount == 0)
#else
		if (pdecrement(&refcount) == 0)
#endif
			delete this;
		else
			return false;
	}
	return true;
}


PTYPES_END
