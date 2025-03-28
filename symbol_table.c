#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>  

// declaration
TableSymboles tableSymboles;

     

// initialisation
void initialiserTable() {
    tableSymboles.capacite = 100;  
    tableSymboles.taille = 0;     
    tableSymboles.entrees = malloc(tableSymboles.capacite * sizeof(EntreeSymbole));
}

void inserer(const char *type, const char *nom) {
    // Vérifier si la table doit être agrandie
    if (tableSymboles.taille >= tableSymboles.capacite) {
        tableSymboles.capacite *= 2;
        tableSymboles.entrees = realloc(tableSymboles.entrees, 
                                        tableSymboles.capacite * sizeof(EntreeSymbole));
    }

    // ajout
    tableSymboles.entrees[tableSymboles.taille] = (EntreeSymbole){
        .nom = strdup(nom),
        .type = strdup(type),
        .ligne = nb_ligne,  
        .colonne = col 
    };
    tableSymboles.taille++; 
}

// liberation
void liberere() {
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
