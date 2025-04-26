#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "symbol_table.h"


void check_declaration(const char *id, int line, int colonne);
void check_duplicate_symbol(const char *id, int line, int colonne);
void check_const_assignment(const char *id, int line , int colonne);
void check_type_compatibility(DataType expected, DataType actual, int line, int colonne);
void check_division_by_zero(const char *value, DataType type, int line, int colonne);
void check_read_format(const char *format, DataType var_type, int line, int colonne);
void check_display_format(const char *format, DataType var_type, int line, int colonne);
void check_comparison_operation(DataType type1, DataType type2, const char *op, int line, int colonne) ;
void check_boolean_operation(DataType type1, DataType type2, const char *op, int line, int colonne) ;
#endif