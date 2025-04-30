#ifndef TYPES_H
#define TYPES_H

// Define DataType
typedef enum {
    TYPE_INTEGER, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL,
    mc, sep, constante, TYPE_UNKNOWN, idf
} DataType;

// Define ExprAttr
typedef struct {
    char* nom;
    DataType type;
    char* quad_addr;
    char* true_label;
    char* false_label;
} ExprAttr;

#endif // TYPES_H