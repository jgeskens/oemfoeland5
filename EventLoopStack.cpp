#include "EventLoopStack.h"

stack<void (*)(void)> EVENT_LOOP_STACK;
int EVENT_LOOP_STACK_RETURN_VALUE = 0;
void (* EVENT_LOOP_STACK_LAST)(void) = NULL;
