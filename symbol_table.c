// symbol_table.c
#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define TAILLE_HASH 100

TableSymboles tableSymboles;

extern int nb_ligne;
extern int col;



// === Fonction de hachage simple (djb2) ===
unsigned int hash(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % tableSymboles.capacite;
}

// === Utilitaires ===
const char* type_to_string(DataType type) {
    switch(type) {
        case TYPE_INTEGER: return "integer";
        case TYPE_FLOAT: return "float";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "string";
        case TYPE_BOOL: return "bool";
        case mc: return "mot_cle";
        case sep: return "separateur";
        case constante: return "constante";
        default: return "unknown";
    }
}

DataType string_to_type(const char* type_str) {
    if (strcmp(type_str, "integer") == 0) return TYPE_INTEGER;
    if (strcmp(type_str, "float") == 0) return TYPE_FLOAT;
    if (strcmp(type_str, "char") == 0) return TYPE_CHAR;
    if (strcmp(type_str, "string") == 0) return TYPE_STRING;
    if (strcmp(type_str, "bool") == 0) return TYPE_BOOL;
    return TYPE_UNKNOWN;
}

// === Initialisation ===
void initialiser() {
    tableSymboles.capacite = TAILLE_HASH;
    tableSymboles.table = calloc(tableSymboles.capacite, sizeof(EntreeSymbole*));
}

// Set a variable as initialized
void set_initialized(char *nom) {
    EntreeSymbole *sym = get_symbol(nom);
    if (sym != NULL) {
        sym->est_initialise = 1;
    }
}

// Check if a variable is initialized
int is_initialized(char *nom) {
    EntreeSymbole *sym = get_symbol(nom);
    if (sym != NULL) {
        return sym->est_initialise;
    }
    return 0;  // Not found or not initialized
}




// === Insertion dans la table ===
void inserer(const char *nom, DataType type, int is_const, int array_size) {
    unsigned int index = hash(nom);
    EntreeSymbole *nouveau = malloc(sizeof(EntreeSymbole));
    
    nouveau->nom = strdup(nom);
    nouveau->type = type;
    nouveau->type_name = strdup(type_to_string(type));
    nouveau->ligne = nb_ligne;
    nouveau->colonne = col;
    nouveau->is_const = is_const;
    nouveau->array_size = array_size;
    nouveau->est_initialise = 0;
    nouveau->suivant = tableSymboles.table[index]; // Chaine sur l'existant
    tableSymboles.table[index] = nouveau;
}

// === Rechercher un symbole ===
EntreeSymbole* get_symbol(const char *nom) {
    unsigned int index = hash(nom);
    EntreeSymbole *actuel = tableSymboles.table[index];
    while (actuel != NULL) {
        if (strcmp(actuel->nom, nom) == 0) {
            return actuel;
        }
        actuel = actuel->suivant;
    }
    return NULL;
}

// === Vérifie l'existence d'un symbole ===
int symbol_exists(const char *nom) {
    return get_symbol(nom) != NULL;
}

// === Obtenir le type d’un symbole ===
DataType get_symbol_type(const char *nom) {
    EntreeSymbole *sym = get_symbol(nom);
    if (sym != NULL) {
        return sym->type;
    }
    return TYPE_UNKNOWN;
}


// === Obtenir la taille d’un tableau (vecteur) ===
int get_symbol_size(const char *nom) {
    EntreeSymbole *sym = get_symbol(nom);
    if (sym != NULL) {
        return sym->array_size;  // La taille du vecteur est stockée ici
    }
    return -1; 
}

// === Affichage de la table ===
void afficher() {
    for (int i = 0; i < tableSymboles.capacite; i++) {
        EntreeSymbole *actuel = tableSymboles.table[i];
        while (actuel != NULL) {
            printf("Nom: %s, Type: %s, Ligne: %d, Colonne: %d\n",
                actuel->nom, actuel->type_name, actuel->ligne, actuel->colonne);
            actuel = actuel->suivant;
        }
    }
}

// === Libération mémoire ===
void liberer() {
    for (int i = 0; i < tableSymboles.capacite; i++) {
        EntreeSymbole *actuel = tableSymboles.table[i];
        while (actuel != NULL) {
            EntreeSymbole *tmp = actuel;
            actuel = actuel->suivant;
            free(tmp->nom);
            free(tmp->type_name);
            free(tmp);
        }
    }
    free(tableSymboles.table);
    tableSymboles.table = NULL;
    tableSymboles.capacite = 0;
}

