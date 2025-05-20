
/* registres.h */
#ifndef REGISTRES_H
#define REGISTRES_H

#include <stdbool.h>
#include "registres.h"
#define MAX_REGISTERS 10 // or however many $t registers you support

typedef struct {
    bool is_free;
    bool is_dirty;
    char var_name[32]; // name of the variable in the register
} RegisterAllocation;

int allocateRegister(RegisterAllocation* regs, int count, const char* var_name);
int getRegister(RegisterAllocation* regs, int count, const char* var_name);
void freeRegister(RegisterAllocation* regs, int count, int index);
void initRegisters(RegisterAllocation* regs, int count);
#endif /* REGISTRES_H */
