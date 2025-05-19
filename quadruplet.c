#include "quadruplet.h"

QuadrupletTable quad_table;
int temp_var_count = 0;
int label_count = 0;
// quadruplet.c
QuadrupletTable quadList;

void quad_init() {
    quad_table.capacity = 100;  
    quad_table.count = 0;
    quad_table.quadruplets = (Quadruplet*)malloc(quad_table.capacity * sizeof(Quadruplet));
    if (!quad_table.quadruplets) {
        fprintf(stderr, "Error: Memory allocation failed for quadruplet table\n");
        exit(EXIT_FAILURE);
    }
}

void quad_free() {
    if (quad_table.quadruplets) {
        for (int i = 0; i < quad_table.count; i++) {
            if (quad_table.quadruplets[i].arg1) free(quad_table.quadruplets[i].arg1);
            if (quad_table.quadruplets[i].arg2) free(quad_table.quadruplets[i].arg2);
            if (quad_table.quadruplets[i].result) free(quad_table.quadruplets[i].result);
        }
        free(quad_table.quadruplets);
        quad_table.quadruplets = NULL;
    }
    quad_table.count = 0;
    quad_table.capacity = 0;
}

int quad_add(OperationType op, char* arg1, char* arg2, char* result) {
    if (quad_table.count >= quad_table.capacity) {
        quad_table.capacity *= 2;
        quad_table.quadruplets = (Quadruplet*)realloc(quad_table.quadruplets, 
                                                     quad_table.capacity * sizeof(Quadruplet));
        if (!quad_table.quadruplets) {
            fprintf(stderr, "Error: Memory reallocation failed for quadruplet table\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // Add the new quadruplet
    quad_table.quadruplets[quad_table.count].op = op;
    quad_table.quadruplets[quad_table.count].arg1 = arg1 ? strdup(arg1) : NULL;
    quad_table.quadruplets[quad_table.count].arg2 = arg2 ? strdup(arg2) : NULL;
    quad_table.quadruplets[quad_table.count].result = result ? strdup(result) : NULL;
    
    return quad_table.count++;
}

// Generate a new temporary variable name
char* new_temp() {
    char* temp = (char*)malloc(15);  // Enough space for "T" + number
    if (!temp) {
        fprintf(stderr, "Error: Memory allocation failed for temporary variable\n");
        exit(EXIT_FAILURE);
    }
    sprintf(temp, "T%d", temp_var_count++);
    return temp;
}

// Generate a new label
char* new_label() {
    char* label = (char*)malloc(15);  // Enough space for "L" + number
    if (!label) {
        fprintf(stderr, "Error: Memory allocation failed for label\n");
        exit(EXIT_FAILURE);
    }
    sprintf(label, "L%d", label_count++);
    return label;
}

// Convert operation type to string
char* get_op_name(OperationType op) {
    switch (op) {
        case OP_ADD: return "ADD";
        case OP_SUB: return "SUB";
        case OP_MUL: return "MUL";
        case OP_DIV: return "DIV";
        case OP_AFFECT: return "AFFECT";
        case OP_READ: return "READ";
        case OP_DISPLAY: return "DISPLAY";
        case OP_CMP_EQ: return "EQ";
        case OP_CMP_NE: return "NE";
        case OP_CMP_L: return "L";
        case OP_CMP_G: return "G";
        case OP_CMP_LE: return "LE";
        case OP_CMP_GE: return "GE";
        case OP_AND: return "AND";
        case OP_OR: return "OR";
        case OP_NOT: return "NOT";
        case OP_GOTO: return "GOTO";
        case OP_BZ: return "BZ";
        case OP_BNZ: return "BNZ";
        case OP_LABEL: return "LABEL";
        case OP_ARRAY_ACCESS: return "ARRAY_ACCESS";
        case OP_ARRAY_ASSIGN: return "ARRAY_ASSIGN";
        default: return "UNKNOWN";
    }
}

// Print all quadruplets in a readable table format
void quad_print() {
    printf("\n----- Quadruplets -----\n");

    // Print table header
    printf("| %-5s | %-10s | %-15s | %-15s | %-15s |\n", "N°", "Op", "Arg1", "Arg2", "Result");
    printf("|-------|------------|-----------------|-----------------|-----------------|\n");

    // Print each quadruplet
    for (int i = 0; i < quad_table.count; i++) {
        printf("| %-5d | %-10s | %-15s | %-15s | %-15s |\n", 
            i,
            get_op_name(quad_table.quadruplets[i].op),
            quad_table.quadruplets[i].arg1 ? quad_table.quadruplets[i].arg1 : "_",
            quad_table.quadruplets[i].arg2 ? quad_table.quadruplets[i].arg2 : "_",
            quad_table.quadruplets[i].result ? quad_table.quadruplets[i].result : "_"
        );
    }

    printf("-----------------------\n");
}


int quad_arithmetic(OperationType op, char* arg1, char* arg2) {
    char* temp = new_temp();
    int quad_index = quad_add(op, arg1, arg2, temp);
    free(temp);  
    return quad_index;
}

int quad_assign(char* target, char* source) {
    return quad_add(OP_AFFECT, source, NULL, target);
}

int quad_vector_assign(char* vector, char* index, char* value) {
    // Create temporary for array access: temp = vector[index]
    char* temp = new_temp();
    quad_add(OP_ARRAY_ASSIGN, vector, index, value);
    free(temp);
    return quad_table.count - 1;
}

int quad_vector_access(char* vector, char* index) {
    char* temp = new_temp();
    quad_add(OP_ARRAY_ACCESS, vector, index, temp);
    free(temp);
    return quad_table.count - 1;
}

int quad_read(char* format, char* var) {
    return quad_add(OP_READ, format, NULL, var);
}

int quad_display(char* format, char* var) {
    return quad_add(OP_DISPLAY, format, var, NULL);
}

int quad_comparison(OperationType op, char* arg1, char* arg2) {
    char* temp = new_temp();
    int quad_index = quad_add(op, arg1, arg2, temp);
    free(temp);
    return quad_index;
}

int quad_logical(OperationType op, char* arg1, char* arg2) {
    char* temp = new_temp();
    int quad_index = quad_add(op, arg1, arg2, temp);
    free(temp);
    return quad_index;
}

int quad_goto(char* label) {
    return quad_add(OP_GOTO, NULL, NULL, label);
}

int quad_bz(char* condition, char* label) {
    return quad_add(OP_BZ, condition, NULL, label);
}

int quad_bnz(char* condition, char* label) {
    return quad_add(OP_BNZ, condition, NULL, label);
}

int quad_label(char* label) {
    return quad_add(OP_LABEL, NULL, NULL, label);
}