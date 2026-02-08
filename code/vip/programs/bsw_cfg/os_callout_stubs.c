/*
 * os_callout_stubs.c
 *
 * Weak default implementations for OS pre-task and post-task callouts.
 * Applications can provide their own (strong) implementations to override.
 */

#include "os_callout_stubs.h"
/* STUB_TEST */
#include <stdio.h>
#include "pico/stdlib.h"

#if defined(__GNUC__)
void __attribute__((weak)) Os_Callout_PretaskHook(void)
{
	printf("Task Enter\n");
}

void __attribute__((weak)) Os_Callout_PosttaskHook(void)
{
	printf("Task Exit\n");
}
#else
/* Fallback if the compiler doesn't support weak attribute */
void Os_Callout_PretaskHook(void) { }
void Os_Callout_PosttaskHook(void) { }
#endif
