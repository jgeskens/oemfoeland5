
#include <stack>

#ifndef __EVENTLOOPSTACK_H__
#define __EVENTLOOPSTACK_H__

using namespace std;

extern "C" {
	extern stack<void (*)(void)> EVENT_LOOP_STACK;
	extern int EVENT_LOOP_STACK_RETURN_VALUE;
	extern void (* EVENT_LOOP_STACK_LAST)(void);
};

#endif
