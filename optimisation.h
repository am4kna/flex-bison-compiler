/* optimization.h */
#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "quadruplet.h"



/* Optimization functions */

/* Constant folding: evaluate constant expressions at compile time */
void constantFolding(QuadrupletTable* quadList);

/* Dead code elimination: remove code that will never be executed */
void deadCodeElimination(QuadrupletTable* quadList);

/* Copy propagation: replace variables with their values when possible */
void 	propagationCopies(QuadrupletTable* quadList);

/* Constant propagation: replace variables with constant values when possible */
void constantPropagation(QuadrupletTable* quadList);

/* Common subexpression elimination: avoid computing the same expression twice */
void eliminationSousExpressionsCommune(QuadrupletTable* quadList);

/* Loop optimization: optimize code inside loops for better performance */
void loopOptimization(QuadrupletTable* quadList);

/* Apply all optimizations to the intermediate code */
void optimizeCode(QuadrupletTable* quadList);

#endif /* OPTIMIZATION_H */