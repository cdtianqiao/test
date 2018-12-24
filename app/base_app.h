#ifndef clib_app_base_app_h
#define clib_app_base_app_h

//linux
#ifdef _LINUX
	void signal_callback_handler(int signum);
	void linux_start();
#endif

#include "../clib.h"

CLIB_NAMESPACE_START

class base_app
{
public:
	base_app();
	virtual ~base_app();
public:
	bool run(bool is_demon= false);
	virtual void exit();
	virtual bool create(){return true;};
	virtual void update(unsigned int dt)=0;
	virtual bool close(){return true;};
protected:
	bool is_exit_;
};


CLIB_NAMESPACE_END

#endif
