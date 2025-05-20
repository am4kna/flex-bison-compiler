/* assembler.h */
#ifndef ASSEMBLEUR_H
#define ASSEMBLEUR_H

#include <stdio.h>
#include <stdbool.h>
#include "quadruplet.h"
#include "registres.h"
/* Function prototypes */
bool isNumeric(const char* str);

void generateArithmeticInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count);

void generateAssignmentInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count);

void generateComparisonInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count);

void generateJumpInstructions(FILE* fp, const Quadruplet* quad);

void generateIOInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count);

void generateAssembly(QuadrupletTable* quadList, const char* output_file);

#endif /* ASSEMBLER_H */