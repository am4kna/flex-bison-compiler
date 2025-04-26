// symbol_table.h
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef enum {
    TYPE_INTEGER, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_BOOL,
    mc, sep, constante, TYPE_UNKNOWN, idf
} DataType;

typedef struct EntreeSymbole {
    char *nom;
    DataType type;
    char *type_name;
    int ligne;
    int colonne;
    int is_const;
    int array_size;
    struct EntreeSymbole *suivant; // Pour le chaînage
} EntreeSymbole;

typedef struct {
    int capacite;
    EntreeSymbole **table; // Tableau de pointeurs vers des listes chaînées
} TableSymboles;

extern TableSymboles tableSymboles;
extern int nb_ligne, col;

void initialiser();
void inserer(const char *nom, DataType type, int is_const, int array_size);
int symbol_exists(const char *nom);
EntreeSymbole* get_symbol(const char *nom);
DataType get_symbol_type(const char *nom);
void afficher();
void liberer();
const char* type_to_string(DataType type);
DataType string_to_type(const char* type_str);
int get_symbol_size(const char *nom) ;
#endif
