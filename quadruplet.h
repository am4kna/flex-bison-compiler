#ifndef QUADRUPLET_H
#define QUADRUPLET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "types.h" 


typedef enum {
    OP_ADD,     // Addition
    OP_SUB,     // Subtraction
    OP_MUL,     // Multiplication
    OP_DIV,     // Division
    OP_AFFECT,  // Assignment
    OP_READ,    // Read operation
    OP_DISPLAY, // Display operation
    OP_CMP_EQ,  // Equal comparison
    OP_CMP_NE,  // Not equal comparison
    OP_CMP_L,  // Less than comparison
    OP_CMP_GT,  // Greater than comparison
    OP_CMP_G,
    OP_CMP_LE,  // Less than or equal comparison
    OP_CMP_GE,  // Greater than or equal comparison
    OP_AND,     // Logical AND
    OP_OR,      // Logical OR
    OP_NOT,     // Logical NOT
    OP_GOTO,    // Unconditional jump
    OP_BZ,      // Branch if zero (conditional jump)
    OP_BNZ,     // Branch if not zero (conditional jump)
    OP_LABEL,   // Label for jumps
    OP_ARRAY_ACCESS, // Array access
    OP_ARRAY_ASSIGN  // Array assignment
} OperationType;

// Quadruplet structure
typedef struct {
    OperationType op;    // Operation
    char* arg1;          // First argument
    char* arg2;          // Second argument
    char* result;        // Result
} Quadruplet;

// Structure to manage quadruplets
typedef struct {
    Quadruplet* quadruplets;  // Array of quadruplets
    int count;                // Number of quadruplets
    int capacity;             // Capacity of the array
} QuadrupletTable;
// Additional fields for expression attributes


// Temporary variable management
extern int temp_var_count;
extern int label_count;

void quad_init();
void quad_free();
int quad_add(OperationType op, char* arg1, char* arg2, char* result);
char* new_temp();
char* new_label();
void quad_print();
char* get_op_name(OperationType op);

int quad_arithmetic(OperationType op, char* arg1, char* arg2);
int quad_assign(char* target, char* source);
int quad_vector_assign(char* vector, char* index, char* value);
int quad_vector_access(char* vector, char* index);
int quad_read(char* format, char* var);
int quad_display(char* format, char* var);
int quad_comparison(OperationType op, char* arg1, char* arg2);
int quad_logical(OperationType op, char* arg1, char* arg2);
int quad_goto(char* label);
int quad_bz(char* condition, char* label);
int quad_bnz(char* condition, char* label);
int quad_label(char* label);

#endif /* QUADRUPLET_H */