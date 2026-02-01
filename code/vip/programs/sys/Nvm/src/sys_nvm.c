/* sys_nvm.c */
#include "sys_nvm.h"

/* Launch the sys NVM main function on core1. */
/* Initialize NVM resources. This is called before the scheduler starts or
 * can be invoked from a task context. Keep minimal init here.
 */
void sys_nvm_init(void)
{
	
}
/* STUB_TEST */
#include <stdio.h>
#include "pico/stdlib.h"
void sys_nvm_mainfunction(void)
{
printf("SYS NVM mainfunction running on core %u\n", get_core_num());
}
