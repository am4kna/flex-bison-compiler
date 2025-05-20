#include <string.h>
#include <stdio.h>
#include "registres.h"


int allocateRegister(RegisterAllocation* regs, int count, const char* var_name) {
    // Check if already allocated
    for (int i = 0; i < count; i++) {
        if (!regs[i].is_free && strcmp(regs[i].var_name, var_name) == 0) {
            return i; // Already allocated to this var
        }
    }

    // Find a free register
    for (int i = 0; i < count; i++) {
        if (regs[i].is_free) {
            regs[i].is_free = false;
            regs[i].is_dirty = false;
            strncpy(regs[i].var_name, var_name, 31);
            regs[i].var_name[31] = '\0';
            return i;
        }
    }

    fprintf(stderr, "Error: No free registers available for variable %s\n", var_name);
    return -1;
}

int getRegister(RegisterAllocation* regs, int count, const char* var_name) {
    for (int i = 0; i < count; i++) {
        if (!regs[i].is_free && strcmp(regs[i].var_name, var_name) == 0) {
            return i;
        }
    }

    // If not found, allocate a new register
    return allocateRegister(regs, count, var_name);
}

void freeRegister(RegisterAllocation* regs, int count, int index) {
    if (index >= 0 && index < count) {
        regs[index].is_free = true;
        regs[index].is_dirty = false;
        regs[index].var_name[0] = '\0';
    }
}

void initRegisters(RegisterAllocation* regs, int count) {
    for (int i = 0; i < count; i++) {
        regs[i].is_free = true;
        regs[i].is_dirty = false;
        regs[i].var_name[0] = '\0';
    }
}
