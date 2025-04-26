#include "semantic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"
#include <ctype.h>

void check_declaration(const char *id, int line, int colonne) {
    if (symbol_exists(id)<1) {
       
        // Vérifie si l'identifiant est une constante       
        fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d) : Variable '%s' non déclarée\n", line, colonne, id);
        exit(EXIT_FAILURE);
    }
    
   
}

void check_duplicate_symbol(const char *id, int line, int colonne) {
    if (symbol_exists(id)>1) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonne %d): '%s' déjà déclaré\n", line,colonne, id);
        exit(EXIT_FAILURE);
    }
}
// Vérifie l'affectation à une constante
void check_const_assignment(const char *id, int line , int colonne) {
    EntreeSymbole *s = get_symbol(id);
    if (s != NULL && strcmp(s->type_name, "constante") == 0) {
        fprintf(stderr, "Erreur ligne %d , colonne %d: Impossible de modifier la constante '%s'\n", line,colonne, id);
        exit(EXIT_FAILURE);
    }
}
// Vérifie la compatibilité des types
void check_type_compatibility(DataType expected, DataType actual, int line, int colonne) {
    if (expected != actual) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Types incompatibles (attendu: %d, reçu: %d)\n", 
                line,colonne, expected, actual);
        exit(EXIT_FAILURE);
    }
}
 
/// Vérifie la division par zéro
void check_division_by_zero(const char *value, DataType type, int line, int colonne) {
    if (type == TYPE_INTEGER && atoi(value) == 0) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonne %d): Division par zéro\n", line,colonne);
        exit(EXIT_FAILURE);
    }
    else if (type == TYPE_FLOAT && atof(value) == 0.0) {
        fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d): Division par zéro\n", line, colonne);   
        exit(EXIT_FAILURE);
    }else if (type == TYPE_STRING && strcmp(value, "0") == 0) {
        fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d): Division par zéro\n", line, colonne);   
        exit(EXIT_FAILURE);

}
}
// Vérifie le format de lecture
void check_read_format(const char *format, DataType var_type, int line, int colonne) {
   
    switch(var_type) {
        case TYPE_INTEGER: if (strcmp(format, "$") != 0) goto format_error; break;
        case TYPE_FLOAT: if (strcmp(format, "%") != 0) goto format_error; break;
        case TYPE_STRING: if (strcmp(format, "#") != 0) goto format_error; break;
        case TYPE_CHAR: if (strcmp(format, "&") != 0) goto format_error; break;
        default: goto format_error;
    }
    return;
    
format_error:
    fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d): Format '%s' incompatible avec le type %d\n", 
            line,colonne, format, var_type);
    exit(EXIT_FAILURE);
}
// Vérifie le format DISPLAY
void check_display_format(const char *format, DataType var_type, int line, int colonne) {
    char found_format = '\0'; // Pour stocker le signe de formatage trouvé

    // Parcourir la chaîne pour trouver un signe de formatage
    while (*format) {
        if (*format == '$' || *format == '%' || *format == '#' || *format == '&') {
            found_format = *format;
            break;
        }
        format++;
    }

    if (found_format == '\0') {
        // Aucun signe de formatage trouvé
        fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d): Aucun signe de formatage trouvé\n", line, colonne);
        exit(EXIT_FAILURE);
    }

    // Vérifier si le signe trouvé correspond au type attendu ou non 
    switch(var_type) {
        case TYPE_INTEGER: if (found_format != '$') goto format_error; break;
        case TYPE_FLOAT: if (found_format != '%') goto format_error; break;
        case TYPE_STRING: if (found_format != '#') goto format_error; break;
        case TYPE_CHAR: if (found_format != '&') goto format_error; break;
        default: goto format_error;
    }
    return;

format_error:
    fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d): Format '%c' incompatible avec le type %d\n", 
            line, colonne, found_format, var_type);
    exit(EXIT_FAILURE);
}

// Vérifie les opérations de comparaison

void check_comparison_operation(DataType type1, DataType type2, const char *op, int line, int colonne) {
    if (type1 != type2) {
        fprintf(stderr, "Erreur sémantique (ligne %d) (colonne %d): Comparaison '%s' entre types incompatibles (%d et %d)\n", 
                line,colonne,  op, type1, type2);
        exit(EXIT_FAILURE);
    }
}

void check_boolean_operation(DataType type1, DataType type2, const char *op, int line, int colonne) {
    if (type1 != TYPE_BOOL || type2 != TYPE_BOOL) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonned%d): L'opérateur '%s' nécessite des opérandes booléens\n", 
                line, colonne, op);
        exit(EXIT_FAILURE);
    }
}