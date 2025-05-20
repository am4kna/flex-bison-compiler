/* assembler.c */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "quadruplet.h"
#include "registres.h"
#include "assembleur.h"

/* Check if a string is a numeric constant */
bool isNumeric(const char* str) {
    // Keep this function as is
    if (str == NULL || *str == '\0') return false;
    
    // Check if it's a negative number
    if (*str == '-') str++;
    
    // Check for decimal point
    bool hasDecimal = false;
    
    while (*str) {
        if (*str == '.') {
            if (hasDecimal) return false; // More than one decimal point
            hasDecimal = true;
        } else if (*str < '0' || *str > '9') {
            return false; // Not a digit
        }
        str++;
    }
    
    return true;
}

void generateArithmeticInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count) {
    
    int op1_reg = getRegister(regAllocations, reg_count, quad->arg1);
    int op2_reg = getRegister(regAllocations, reg_count, quad->arg2);
    int res_reg = allocateRegister(regAllocations, reg_count, quad->result);
    
    // 8086 16-bit registers
    const char* regNames[] = {"ax", "bx", "cx", "dx", "si", "di"};
    
    switch (quad->op) {
        case OP_ADD:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    add %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            break;
        case OP_SUB:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    sub %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            break;
        case OP_MUL:
            // 8086 multiplication is different
            fprintf(fp, "    mov ax, %s\n", regNames[op1_reg]);
            fprintf(fp, "    imul %s\n", regNames[op2_reg]);  // result in DX:AX
            fprintf(fp, "    mov %s, ax\n", regNames[res_reg]); // Only keep lower word
            break;
        case OP_DIV:
            // 8086 division requires specific registers
            fprintf(fp, "    mov ax, %s\n", regNames[op1_reg]);
            fprintf(fp, "    cwd\n");                      // Sign-extend ax into dx
            fprintf(fp, "    idiv %s\n", regNames[op2_reg]); // Result in ax
            fprintf(fp, "    mov %s, ax\n", regNames[res_reg]);
            break;
        default:
            fprintf(stderr, "Unknown arithmetic op code\n");
    }
    
    // Store result to memory
    fprintf(fp, "    mov [_var_%s], %s\n", quad->result, regNames[res_reg]);
    
    // Free registers if operands were numeric constants
    if (isNumeric(quad->arg1)) freeRegister(regAllocations, reg_count, op1_reg);
    if (isNumeric(quad->arg2)) freeRegister(regAllocations, reg_count, op2_reg);
}

void generateAssignmentInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count) {
    const char* regNames[] = {"ax", "bx", "cx", "dx", "si", "di"};
    int value_reg = getRegister(regAllocations, reg_count, quad->arg1);
    
    if (quad->op == OP_ARRAY_ASSIGN) {
        // Handle array assignment: result[arg2] = arg1
        int index_reg = getRegister(regAllocations, reg_count, quad->arg2);
        
        // Calculate array offset
        fprintf(fp, "    mov cx, %s\n", regNames[index_reg]);    
        fprintf(fp, "    shl cx, 1\n");                  // Multiply index by 2 (16-bit words)
        fprintf(fp, "    mov bx, cx\n");
        fprintf(fp, "    mov [_var_%s + bx], %s\n", quad->result, regNames[value_reg]); // Store value at array[index]
        
        // Free registers if operands were numeric constants
        if (isNumeric(quad->arg2)) freeRegister(regAllocations, reg_count, index_reg);
    } else {
        // Regular assignment: result = arg1
        fprintf(fp, "    mov [_var_%s], %s\n", quad->result, regNames[value_reg]);
    }
    
    // Free register if value was a numeric constant
    if (isNumeric(quad->arg1)) freeRegister(regAllocations, reg_count, value_reg);
}

void generateComparisonInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count) {
    const char* regNames[] = {"ax", "bx", "cx", "dx", "si", "di"};
    int op1_reg = getRegister(regAllocations, reg_count, quad->arg1);
    int op2_reg = getRegister(regAllocations, reg_count, quad->arg2);
    int res_reg = allocateRegister(regAllocations, reg_count, quad->result);
    
    // 8086 doesn't have setcc instructions, need to use conditional jumps
    switch (quad->op) {
        case OP_CMP_EQ:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    cmp %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            fprintf(fp, "    mov %s, 0\n", regNames[res_reg]);  // Assume false
            fprintf(fp, "    je _%d_equal\n");
            fprintf(fp, "    jmp _%d_end\n");
            fprintf(fp, "_%d_equal:\n");
            fprintf(fp, "    mov %s, 1\n", regNames[res_reg]);
            fprintf(fp, "_%d_end:\n");
            break;
        case OP_CMP_NE:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    cmp %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            fprintf(fp, "    mov %s, 0\n", regNames[res_reg]);
            fprintf(fp, "    jne _%d_not_equal\n");
            fprintf(fp, "    jmp _%d_end\n");
            fprintf(fp, "_%d_not_equal:\n");
            fprintf(fp, "    mov %s, 1\n", regNames[res_reg]);
            fprintf(fp, "_%d_end:\n");
            break;
        case OP_CMP_G:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    cmp %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            fprintf(fp, "    mov %s, 0\n", regNames[res_reg]);
            fprintf(fp, "    jg");
            fprintf(fp, "    jmp ");
            fprintf(fp, "_greater:\n");
            fprintf(fp, "    mov %s, 1\n", regNames[res_reg]);
            fprintf(fp, "__end:\n");
            break;
        case OP_CMP_L:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    cmp %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            fprintf(fp, "    mov %s, 0\n", regNames[res_reg]);
            fprintf(fp, "    jl _%d_less\n");
            fprintf(fp, "    jmp _%d_end\n");
            fprintf(fp, "__less:\n");
            fprintf(fp, "    mov %s, 1\n", regNames[res_reg]);
            fprintf(fp, "__end:\n");
            break;
        case OP_CMP_GE:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    cmp %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            fprintf(fp, "    mov %s, 0\n", regNames[res_reg]);
            fprintf(fp, "    jge _%d_greater_equal\n");
            fprintf(fp, "    jmp _%d_end\n");
            fprintf(fp, "_%d_greater_equal:\n");
            fprintf(fp, "    mov %s, 1\n", regNames[res_reg]);
            fprintf(fp, "_%d_end:\n");
            break;
        case OP_CMP_LE:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    cmp %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            fprintf(fp, "    mov %s, 0\n", regNames[res_reg]);
            fprintf(fp, "    jle _less_equal\n");
            fprintf(fp, "    jmp __end\n" );
            fprintf(fp, "__less_equal:\n" );
            fprintf(fp, "    mov %s, 1\n", regNames[res_reg]);
            fprintf(fp, "_%_end:\n" );
            break;
        case OP_AND:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    and %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            break;
        case OP_OR:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    or %s, %s\n", regNames[res_reg], regNames[op2_reg]);
            break;
        case OP_NOT:
            fprintf(fp, "    mov %s, %s\n", regNames[res_reg], regNames[op1_reg]);
            fprintf(fp, "    xor %s, 1\n", regNames[res_reg]); // Toggle bit 0
            break;
        default:
            fprintf(stderr, "Unknown comparison op code\n");
    }
    
    fprintf(fp, "    mov [_var_%s], %s\n", quad->result, regNames[res_reg]);
    
    if (isNumeric(quad->arg1)) freeRegister(regAllocations, reg_count, op1_reg);
    if (isNumeric(quad->arg2)) freeRegister(regAllocations, reg_count, op2_reg);
}

void generateJumpInstructions(FILE* fp, const Quadruplet* quad) {
    switch (quad->op) {
        case OP_GOTO:
            fprintf(fp, "    jmp label_%s\n", quad->result);
            break;
        case OP_BNZ:
            fprintf(fp, "    mov ax, [_var_%s]\n", quad->arg1);
            fprintf(fp, "    cmp ax, 0\n");
            fprintf(fp, "    jne label_%s\n", quad->result);
            break;
        case OP_LABEL:
            fprintf(fp, "label_%s:\n", quad->result);
            break;
        default:
            fprintf(stderr, "Unknown jump op code\n");
    }
}

void generateIOInstructions(FILE* fp, const Quadruplet* quad, RegisterAllocation* regAllocations, int reg_count) {
    // For 8086 we'll use BIOS interrupts or DOS services for I/O
    const char* regNames[] = {"ax", "bx", "cx", "dx", "si", "di"};
    
    switch (quad->op) {
        case OP_READ: {
            int reg = allocateRegister(regAllocations, reg_count, quad->result);
            
            if (strcmp(quad->arg1, "$") == 0) {  // Integer
                // Use DOS INT 21h function 0Ah to read string
                fprintf(fp, "    mov ah, 0Ah\n");
                fprintf(fp, "    lea dx, [input_buffer]\n");
                fprintf(fp, "    int 21h\n");
                // Convert string to integer (simplified)
                fprintf(fp, "    lea si, [input_buffer+2]\n");
                fprintf(fp, "    call string_to_int\n");
                fprintf(fp, "    mov [_var_%s], ax\n", quad->result);
                fprintf(fp, "    mov %s, ax\n", regNames[reg]);
            } else if (strcmp(quad->arg1, "&") == 0) {  // Character
                // Use DOS INT 21h function 01h to read a character
                fprintf(fp, "    mov ah, 1\n");
                fprintf(fp, "    int 21h\n");          // Character in AL
                fprintf(fp, "    mov [_var_%s], ax\n", quad->result);
                fprintf(fp, "    mov %s, ax\n", regNames[reg]);
            } else if (strcmp(quad->arg1, "#") == 0) {  // String
                // Use DOS INT 21h function 0Ah to read string
                fprintf(fp, "    mov ah, 0Ah\n");
                fprintf(fp, "    lea dx, [input_buffer]\n");
                fprintf(fp, "    int 21h\n");
                // Copy to destination
                fprintf(fp, "    lea si, [input_buffer+2]\n");
                fprintf(fp, "    lea di, [_var_%s]\n", quad->result);
                fprintf(fp, "    call copy_string\n");
            }
            break;
        }
        case OP_DISPLAY: {
            if (quad->result[0] != '\0') {
                int reg = getRegister(regAllocations, reg_count, quad->result);
                fprintf(fp, "    mov %s, [_var_%s]\n", regNames[reg], quad->result);
                
                if (strstr(quad->arg1, "$") != NULL) {  // Integer
                    // Convert integer to string and display
                    fprintf(fp, "    mov ax, %s\n", regNames[reg]);
                    fprintf(fp, "    lea di, [output_buffer]\n");
                    fprintf(fp, "    call int_to_string\n");
                    fprintf(fp, "    lea dx, [output_buffer]\n");
                    fprintf(fp, "    call print_string\n");
                } else if (strstr(quad->arg1, "&") != NULL) {  // Character
                    // Display character using DOS INT 21h function 02h
                    fprintf(fp, "    mov dl, %s\n", regNames[reg]);
                    fprintf(fp, "    mov ah, 2\n");
                    fprintf(fp, "    int 21h\n");
                } else if (strstr(quad->arg1, "#") != NULL) {  // String
                    fprintf(fp, "    lea dx, [_var_%s]\n", quad->result);
                    fprintf(fp, "    call print_string\n");
                }
                
                // Add newline
                fprintf(fp, "    mov ah, 2\n");
                fprintf(fp, "    mov dl, 0Dh\n");  // Carriage return
                fprintf(fp, "    int 21h\n");
                fprintf(fp, "    mov dl, 0Ah\n");  // Line feed
                fprintf(fp, "    int 21h\n");
            } else {
                // Just print the format string directly
                fprintf(fp, "    lea dx, [%s]\n", quad->arg1);
                fprintf(fp, "    call print_string\n");
                // Add newline
                fprintf(fp, "    mov ah, 2\n");
                fprintf(fp, "    mov dl, 0Dh\n");
                fprintf(fp, "    int 21h\n");
                fprintf(fp, "    mov dl, 0Ah\n");
                fprintf(fp, "    int 21h\n");
            }
            break;
        }
        default:
            fprintf(stderr, "Unknown I/O op code\n");
    }
}

/* Generate assembly code from optimized quadruplets */
void generateAssembly(QuadrupletTable* quadList, const char* output_file) {
    FILE* fp = fopen(output_file, "w");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open output file %s\n", output_file);
        return;
    }
    
    // Initialize registers
    RegisterAllocation regAllocations[MAX_REGISTERS];
    initRegisters(regAllocations, MAX_REGISTERS);
    
    // Write assembly prologue
    fprintf(fp, "; PHYLOG Assembly Code 8086\n");
   
    
    // Process quadruplets to identify message strings
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        if (quad->op == OP_DISPLAY && quad->arg1[0] == '"') {
            // Extract string literal without quotes
            char msg[256];
            strncpy(msg, quad->arg1 + 1, strlen(quad->arg1) - 2);
            msg[strlen(quad->arg1) - 2] = '\0';
            
            // Replace format specifiers with proper output
            char *dollar = strstr(msg, "$");
            if (dollar) *dollar = '$';  // Replace $ with DOS string terminator
            
            // Define the string
            fprintf(fp, "msg%d db \"%s\", '$'\n", i, msg);
        }
    }
    
    // Variables space - declare each variable separately with a unique name
    fprintf(fp, "\n; Variables space\n");
    
    // Process quadruplets to identify all variables
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        
        // Check result for variable
        if (quad->result[0] != '\0' && quad->op != OP_LABEL && quad->op != OP_GOTO && quad->op != OP_BNZ) {
            fprintf(fp, "_var_%s dw 0\n", quad->result);
        }
        
        // Check arg1 and arg2 for variables if they're not numeric
        if (quad->arg1[0] != '\0' && !isNumeric(quad->arg1)) {
            fprintf(fp, "_var_%s dw 0\n", quad->arg1);
        }
        
        if (quad->arg2[0] != '\0' && !isNumeric(quad->arg2)) {
            fprintf(fp, "_var_%s dw 0\n", quad->arg2);
        }
    }
    
    // Code section
    fprintf(fp, "\n.code\n");
    
    // Main procedure
    fprintf(fp, "main proc\n");
    fprintf(fp, "    mov ax, @data\n");
    fprintf(fp, "    mov ds, ax\n\n");
    
    // Process each quadruplet
    for (int i = 0; i < quadList->count; i++) {
        Quadruplet* quad = &quadList->quadruplets[i];
        fprintf(fp, "; Quadruplet %d: op=%d, arg1=%s, arg2=%s, result=%s\n", 
                i, quad->op, quad->arg1, quad->arg2, quad->result);
                
        switch (quad->op) {
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
                generateArithmeticInstructions(fp, quad, regAllocations, MAX_REGISTERS);
                break;
            case OP_ARRAY_ASSIGN:
                generateAssignmentInstructions(fp, quad, regAllocations, MAX_REGISTERS);
                break;
            case OP_CMP_G:
            case OP_CMP_L:
            case OP_CMP_GE:
            case OP_CMP_LE:
            case OP_CMP_EQ:
            case OP_CMP_NE:
            case OP_AND:
            case OP_OR:
            case OP_NOT:
                generateComparisonInstructions(fp, quad, regAllocations, MAX_REGISTERS);
                break;
            case OP_GOTO:
            case OP_BNZ:
            case OP_LABEL:
                generateJumpInstructions(fp, quad);
                break;
            case OP_READ:
            case OP_DISPLAY:
                generateIOInstructions(fp, quad, regAllocations, MAX_REGISTERS);
                break;
            default:
                fprintf(stderr, "Unknown operation in quadruplet at index %d\n", i);
                break;
        }
    }
    
    // Epilogue
    fprintf(fp, "\n    ; Exit program\n");
    fprintf(fp, "    mov ax, 4C00h\n");    // DOS exit function
    fprintf(fp, "    int 21h\n");
    fprintf(fp, "main endp\n\n");
    

    
    fclose(fp);
    printf("8086 Assembly code generated successfully to %s\n", output_file);
}