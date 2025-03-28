#ifndef TABLE_SYMBOLES_H
#define TABLE_SYMBOLES_H

typedef struct {
    char *nom;
    char *type;
    int ligne;
    int colonne;
} EntreeSymbole;

typedef struct {
    EntreeSymbole *entrees;
    int taille;
    int capacite;
} TableSymboles;

extern TableSymboles tableSymboles; 
extern int nb_ligne;    
extern int col;  

void initialiser();
void inserer(const char *type, const char *nom);
void liberer();
void afficher();  

#endif
