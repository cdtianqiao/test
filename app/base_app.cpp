#include <boost/thread.hpp>
#include "base_app.h"
#include "../timer/time_ex.h"
#include "../log/log.h"
#ifdef _LINUX
	#include<fcntl.h>
	#include<sys/types.h>
	#include<unistd.h>
	#include<signal.h>
	#include<sys/wait.h>
	#include<sys/stat.h>
	#define MAXFILE 65535
#endif
CLIB_NAMESPACE_START

base_app::base_app():is_exit_(false)
{

}
base_app::~base_app()
{

}
bool base_app::run(bool is_demon)
{
	if(create()==false)
	{
		return false;
	}

	unsigned long long pre_tick = clib::time_ex::get_tick();
	unsigned long long current_tick = 0;
	//run front
	if(is_demon == false)
	{
		while(is_exit_ == false)
		{
			
			current_tick = clib::time_ex::get_tick();
			{
				FUNCTION_START_WARN(33)
				update( static_cast<unsigned int >(current_tick - pre_tick));
			}
			
			pre_tick = current_tick;

			boost::this_thread::sleep(boost::posix_time::milliseconds(APPLICATION_SLEEP_TIMES));
		}
	}
	else 
	{
		//todo need to do //add whith fork
		while(is_exit_ == false)
		{
			current_tick = clib::time_ex::get_tick();
			{
				FUNCTION_START_WARN(33)
				update(static_cast<unsigned int>(current_tick - pre_tick));
			}
			pre_tick = current_tick;
			boost::this_thread::sleep(boost::posix_time::milliseconds(APPLICATION_SLEEP_TIMES));
		}
	}
	return close();
}

void base_app::exit()
{
	is_exit_ = true;
}

CLIB_NAMESPACE_END

#ifdef _LINUX

//void signal_callback_handler(int signum)
//{
//	DEBUG_LOG("Caught signal %d\n",signum);
//	// Cleanup and close up stuff here
//	// Terminate program
//	exit(signum);
//}

void linux_start()
{
	signal(2, signal_callback_handler);
	//signal(9, signal_callback_handler);

	pid_t pc;
	pc = fork(); //step 1
	if(pc < 0){
		printf("error fork\n");
		exit(1);
	}
	else if(pc > 0)
		exit(0);
	setsid(); 	//step 2

	char file[32];
	char buf[256];
	memset(buf, 0, sizeof(buf));
	pid_t pid = getpid();
	sprintf(file, "/proc/%i/cmdline", pid);
	FILE *f = fopen(file, "r");
	if(f != 0)
	{
		fgets(buf, 256, f);
		if(buf[0] == '.')
		{
			char *true_path = realpath(buf, 0);
            clib::string temp(true_path);
			temp = temp.substr(0, temp.find_last_of('/'));
			chdir(temp.c_str()); //step 3
			free(true_path);
		}
		else
		{
            clib::string temp(buf);
			temp = temp.substr(0, temp.find_last_of('/'));
			chdir(temp.c_str()); //step 3
		}
		fclose(f);
	}
	umask(0); 	//step 4
	for(int i=0; i < MAXFILE; ++i) //step 5
		close(i);
}
#endif
