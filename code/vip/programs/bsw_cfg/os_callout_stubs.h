/*
 * os_callout_stubs.h
 * Declarations for OS Hooks.
 */

#ifndef OS_CALLOUT_STUBS_H
#define OS_CALLOUT_STUBS_H

/* Called immediately before a task starts executing. */
void Os_Callout_PretaskHook(void);

/* Called immediately after a task stops executing. */
void Os_Callout_PosttaskHook(void);

#endif /* OS_CALLOUT_STUBS_H */
