#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>  

// declaration
TableSymboles tableSymboles;

     

// initialisation
void initialiser() {
    tableSymboles.capacite = 100;  
    tableSymboles.taille = 0;     
    tableSymboles.entrees = malloc(tableSymboles.capacite * sizeof(EntreeSymbole));
}

void inserer(const char *nom, const char *type) {
    
    if (tableSymboles.taille >= tableSymboles.capacite) {
        tableSymboles.capacite *= 2;
        tableSymboles.entrees = realloc(tableSymboles.entrees,
                                        tableSymboles.capacite * sizeof(EntreeSymbole));
    }

    tableSymboles.entrees[tableSymboles.taille].nom = strdup(nom);
    tableSymboles.entrees[tableSymboles.taille].type = strdup(type);
    tableSymboles.entrees[tableSymboles.taille].ligne = nb_ligne;
    tableSymboles.entrees[tableSymboles.taille].colonne = col;
    tableSymboles.taille++;
}


// liberation
void liberer() {
    for (int i = 0; i < tableSymboles.taille; i++) {
        free(tableSymboles.entrees[i].nom);
        free(tableSymboles.entrees[i].type);
    }
    free(tableSymboles.entrees);
    tableSymboles.taille = 0;
    tableSymboles.capacite = 0;
}

//afficher la table
void afficher() {
    printf("Table des symboles :\n");
    for (int i = 0; i < tableSymboles.taille; i++) {
        printf("%-10s %-8s Ligne:%-3d Colonne:%-3d\n", 
               tableSymboles.entrees[i].nom,
               tableSymboles.entrees[i].type,
               tableSymboles.entrees[i].ligne,
               tableSymboles.entrees[i].colonne);
    }
}
