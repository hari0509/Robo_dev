#ifndef TESTMAIN_H
#define TESTMAIN_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "mcal_gpt.h"

void TestMain_init(void);
void TestMain_mainfunction(void);
void TestScheduler_mainfunction(void);
void sys_checkpointreached(void);
void asw_checkpointreached(void);
void cdd_checkpointreached(void);

#endif /* TESTMAIN_H */
