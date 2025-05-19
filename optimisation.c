/* optimization.c */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h> /* Added for isdigit() function */
#include "optimisation.h" /* Make sure this matches your actual header file name */
#include "quadruplet.h"

bool isConstant(const char* str) {
    char* endptr;
    strtod(str, &endptr);
    return *endptr == '\0';
}

double evaluateConstantExpression(OperationType op, const char* op1, const char* op2) {
    double val1 = atof(op1);
    double val2 = atof(op2);
    switch (op) {
        case OP_ADD: return val1 + val2;
        case OP_SUB: return val1 - val2;
        case OP_MUL: return val1 * val2;
        case OP_DIV:
            if (val2 == 0) {
                fprintf(stderr, "Warning: Division by zero detected during optimization\n");
                return 0;
            }
            return val1 / val2;
        case OP_AND: return (val1 != 0 && val2 != 0) ? 1 : 0;
        case OP_OR:  return (val1 != 0 || val2 != 0) ? 1 : 0;
        case OP_CMP_G:   return (val1 > val2) ? 1 : 0;
        case OP_CMP_L:   return (val1 < val2) ? 1 : 0;
        case OP_CMP_GE:  return (val1 >= val2) ? 1 : 0;
        case OP_CMP_LE:  return (val1 <= val2) ? 1 : 0;
        case OP_CMP_EQ:  return (val1 == val2) ? 1 : 0;
        case OP_CMP_NE:  return (val1 != val2) ? 1 : 0;
        default:         return 0;
    }
}

void constantFolding(QuadrupletTable* quadList) {
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        if ((quad->op == OP_ADD || quad->op == OP_SUB ||
             quad->op == OP_MUL || quad->op == OP_DIV ||
             quad->op == OP_AND || quad->op == OP_OR  ||
             quad->op == OP_CMP_G   || quad->op == OP_CMP_L   ||
             quad->op == OP_CMP_GE  || quad->op == OP_CMP_LE  ||
             quad->op == OP_CMP_EQ  || quad->op == OP_CMP_NE) &&
            isConstant(quad->arg1) && isConstant(quad->arg2)) {
            double result = evaluateConstantExpression(quad->op, quad->arg1, quad->arg2);
            quad->op = OP_AFFECT;
            sprintf(quad->arg1, "%g", result);
            quad->arg2[0] = '\0';
        }
    }
}

void 	propagationCopies(QuadrupletTable* quadList) {
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        if (quad->op == OP_AFFECT && quad->arg2[0] == '\0') {
            const char* var = quad->result;
            const char* val = quad->arg1;
            if (!isdigit(val[0]) && val[0] != 't') {
                for (int j = i + 1; j < quadList->count; j++) {
                    Quadruplet* next_quad = &quadList->quadruplets[j];
                    if (strcmp(next_quad->result, var) == 0) {
                        break;
                    }
                    if (strcmp(next_quad->arg1, var) == 0) {
                        strcpy(next_quad->arg1, val);
                    }
                    if (strcmp(next_quad->arg2, var) == 0) {
                        strcpy(next_quad->arg2, val);
                    }
                }
            }
        }
    }
}

void deadCodeElimination(QuadrupletTable* quadList) {
    bool* isLive = (bool*)calloc(quadList->count, sizeof(bool));
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        if (quad->op == OP_GOTO || quad->op == OP_BZ || quad->op == OP_BNZ ||
            quad->op == OP_LABEL || quad->op == OP_READ || quad->op == OP_DISPLAY) {
            isLive[i] = true;
        }
    }
    for (int i = quadList->count - 1; i >= 0; i--) {
        if (isLive[i]) {
            Quadruplet* quad = &quadList->quadruplets[i];
            for (int j = 0; j < i; j++) {
                if ((strcmp(quadList->quadruplets[j].result, quad->arg1) == 0) ||
                    (strcmp(quadList->quadruplets[j].result, quad->arg2) == 0)) {
                    isLive[j] = true;
                }
            }
        }
    }
    int writeIdx = 0;
    for (int readIdx = 0; readIdx < quadList->count; readIdx++) {
        if (isLive[readIdx]) {
            if (writeIdx != readIdx) {
                quadList->quadruplets[writeIdx] = quadList->quadruplets[readIdx];
            }
            writeIdx++;
        }
    }
    quadList->count = writeIdx;
    free(isLive);
}

void eliminationSousExpressionsCommune(QuadrupletTable* quadList) {
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* current = &quadList->quadruplets[i];
        if (current->op == OP_AFFECT || current->op == OP_GOTO ||
            current->op == OP_BZ || current->op == OP_BNZ || current->op == OP_LABEL) {
            continue;
        }
        for (int j = 0; j < i; j++) {
            Quadruplet* previous = &quadList->quadruplets[j];
            if (previous->op == current->op &&
                strcmp(previous->arg1, current->arg1) == 0 &&
                strcmp(previous->arg2, current->arg2) == 0) {
                current->op = OP_AFFECT;
                strcpy(current->arg1, previous->result);
                current->arg2[0] = '\0';
                break;
            }
        }
    }
}

void constantPropagation(QuadrupletTable* quadList) {
    typedef struct {
        char var[50];
        char value[50];
        bool isConstant;
    } VarInfo;
    VarInfo* varMap = (VarInfo*)malloc(1000 * sizeof(VarInfo));
    int mapSize = 0;
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        if (quad->op == OP_AFFECT && quad->arg2[0] == '\0' && isConstant(quad->arg1)) {
            int found = -1;
            for (int j = 0; j < mapSize; j++) {
                if (strcmp(varMap[j].var, quad->result) == 0) {
                    found = j;
                    break;
                }
            }
            if (found >= 0) {
                strcpy(varMap[found].value, quad->arg1);
                varMap[found].isConstant = true;
            } else {
                strcpy(varMap[mapSize].var, quad->result);
                strcpy(varMap[mapSize].value, quad->arg1);
                varMap[mapSize].isConstant = true;
                mapSize++;
            }
        } else if (quad->result[0] != '\0') {
            for (int j = 0; j < mapSize; j++) {
                if (strcmp(varMap[j].var, quad->result) == 0) {
                    varMap[j].isConstant = false;
                    break;
                }
            }
        }
    }
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        for (int j = 0; j < mapSize; j++) {
            if (varMap[j].isConstant && strcmp(quad->arg1, varMap[j].var) == 0) {
                strcpy(quad->arg1, varMap[j].value);
                break;
            }
        }
        for (int j = 0; j < mapSize; j++) {
            if (varMap[j].isConstant && strcmp(quad->arg2, varMap[j].var) == 0) {
                strcpy(quad->arg2, varMap[j].value);
                break;
            }
        }
    }
    free(varMap);
}

void loopOptimization(QuadrupletTable* quadList) {
    int loopStart = -1;
    int loopEnd = -1;
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        if (quad->op == OP_LABEL && strstr(quad->result, "loop_start") != NULL) {
            loopStart = i;
        }
        if (quad->op == OP_GOTO && strstr(quad->result, "loop_start") != NULL) {
            loopEnd = i;
            if (loopStart >= 0) {
                for (int j = loopStart + 1; j < loopEnd; j++) {
                    Quadruplet* loopQuad = &quadList->quadruplets[j];
                    if (loopQuad->op != OP_ADD && loopQuad->op != OP_SUB &&
                        loopQuad->op != OP_MUL && loopQuad->op != OP_DIV) {
                        continue;
                    }
                    bool isInvariant = true;
                    if (isInvariant) {
                        Quadruplet temp = quadList->quadruplets[j];
                        for (int k = j; k > loopStart; k--) {
                            quadList->quadruplets[k] = quadList->quadruplets[k - 1];
                        }
                        quadList->quadruplets[loopStart + 1] = temp;
                        loopStart++;
                    }
                }
            }
            loopStart = -1;
            loopEnd = -1;
        }
    }
}

/* Export the function for use in other files */
void optimizeCode(QuadrupletTable* quadList) {
    constantFolding(quadList);
    constantPropagation(quadList);
    propagationCopies(quadList);
    eliminationSousExpressionsCommune(quadList);
    deadCodeElimination(quadList);
    loopOptimization(quadList);
    constantFolding(quadList);
    	propagationCopies(quadList);
    deadCodeElimination(quadList);
}