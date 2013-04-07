
#include "pasync.h"


PTYPES_BEGIN


static void msgerror()
{
	fatal(CRIT_FIRST + 42, "Invalid message object");
}


message::message(int iid, pintptr iparam)
	: next(NULL), sync(NULL), id(iid), param(iparam), result(0)
{
}


message::~message()	 
{
}


jobqueue::jobqueue(int ilimit)
	: limit(ilimit), head(NULL), tail(NULL), qcount(0), sem(0), ovrsem(ilimit), qlock()
{
}


jobqueue::~jobqueue()
{
	purgequeue();
}


bool jobqueue::enqueue(message* msg, int timeout)
{
	if (!msg)
		msgerror();

	if (!ovrsem.wait(timeout))
		return false;
	qlock.enter();
	msg->next = NULL;
	if (head)
		head->next = msg;
	head = msg;
	if (!tail)
		tail = msg;
	qcount++;
	qlock.leave();
	sem.post();
	return true;
}


bool jobqueue::push(message* msg, int timeout)
{
	if (!msg)
		msgerror();

	if (!ovrsem.wait(timeout))
		return false;
	qlock.enter();
	msg->next = tail;
	tail = msg;
	if (!head)
		head = msg;
	qcount++;
	qlock.leave();
	sem.post();
	return true;
}


message* jobqueue::dequeue(bool safe, int timeout)
{
	if (!sem.wait(timeout))
		return NULL;
	if (safe)
		qlock.enter();
	message* msg = tail;
	tail = msg->next;
	qcount--;
	if (!tail)
		head = NULL;
	if (safe)
		qlock.leave();
	ovrsem.post();
	return msg;
}


void jobqueue::purgequeue()
{
	qlock.enter();
	while (get_count() > 0)
		delete dequeue(false);
	qlock.leave();
}


bool jobqueue::post(message* msg, int timeout)
{
	return enqueue(msg, timeout);
}


bool jobqueue::post(int id, pintptr param, int timeout)
{
	return post(new message(id, param), timeout);
}


bool jobqueue::posturgent(message* msg, int timeout)
{
	return push(msg, timeout);
}


bool jobqueue::posturgent(int id, pintptr param, int timeout)
{
	return posturgent(new message(id, param), timeout);
}


message* jobqueue::getmessage(int timeout)
{
	return dequeue(true, timeout);
}


msgqueue::msgqueue(int ilimit)
	: jobqueue(ilimit), thrlock(), owner(0), quit(false)
{ 
}


msgqueue::~msgqueue()
{
}


void msgqueue::takeownership()
{
	if (owner != pthrself())
	{
		thrlock.enter();	// lock forever
//	if (owner != 0)
//		fatal(CRIT_FIRST + 45, "Ownership of the message queue already taken");
		owner = pthrself();
	}
}


pintptr msgqueue::finishmsg(message* msg)
{
	if (msg)
	{
		pintptr result = msg->result;

		// if the message was sent by send(), 
		// just signale the semaphore
		if (msg->sync)
			msg->sync->post();

		// otherwise finish it
		else
			delete msg;
		
		return result;
	}
	else
		return 0;
}


pintptr msgqueue::send(message* msg)
{
	if (!msg)
		msgerror();

	try 
	{
		// if we are in the main thread, 
		// immediately handle the msg
		if (pthrequal(owner))
			handlemsg(msg);
		
		// if this is called from a concurrent thread,
		// sync through a semaphore
		else 
		{
			if (msg->sync)
				msgerror();
			semaphore sync(0);
			msg->sync = &sync;
			push(msg);
			msg->sync->wait();
			msg->sync = 0;
		}
	}
	catch (...)
	{
		finishmsg(msg);
		throw;
	}

	return finishmsg(msg);
}


pintptr msgqueue::send(int id, pintptr param)
{
	return send(new message(id, param));
}


void msgqueue::processone()
{
	takeownership();
	message* msg = dequeue();
	try 
	{
		handlemsg(msg);
	}
	catch(...)
	{
		finishmsg(msg);
		throw;
	}
	finishmsg(msg);
}


void msgqueue::processmsgs()
{
	while (!quit && get_count() > 0)
		processone();
}


void msgqueue::run()
{
	quit = false;
	do 
	{
		processone();
	} 
	while (!quit);
}


void msgqueue::handlemsg(message* msg)
{
	msghandler(*msg);
}


void msgqueue::defhandler(message& msg)
{
	switch(msg.id)
	{
	case MSG_QUIT:
		quit = true;
		break;
	}
}


PTYPES_END
