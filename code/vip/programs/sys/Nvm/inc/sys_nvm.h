/* sys_nvm.h - System NVM interface for core1
 */
#ifndef SYS_NVM_H
#define SYS_NVM_H

/* Initialize system NVM handling (called from OS Init or driver init). */
void sys_nvm_init(void);

void sys_nvm_mainfunction(void);

#endif /* SYS_NVM_H */
