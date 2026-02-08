/*
 * os_callout_stubs.h
 *
 * Declarations for OS pre-task and post-task callouts.
 * Default implementations are provided as weak symbols in
 * os_callout_stubs.c and can be overridden by the application.
 */

#ifndef OS_CALLOUT_STUBS_H
#define OS_CALLOUT_STUBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Called immediately before a task starts executing. */
void Os_Callout_PretaskHook(void);

/* Called immediately after a task stops executing. */
void Os_Callout_PosttaskHook(void);

#ifdef __cplusplus
}
#endif

#endif /* OS_CALLOUT_STUBS_H */
