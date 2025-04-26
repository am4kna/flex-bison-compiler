%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"  
#include "semantic.h"
#include <string.h>
int nb_ligne= 1, col=1;
void yyerror(const char *s);
int yylex();
DataType type_courant;
%}

%union {
    float reel;
    char* strVal;
    DataType type;
    struct{
        char* nom;
        DataType type;
    } expr;
    
}
%type <type>  type condition if_else conditions
%type <expr> expression vecteur valeur 
%token <strVal> mc_data mc_end mc_code mc_vector mc_integer mc_float mc_character mc_string mc_const
%token <strVal> mc_read mc_display mc_if mc_else mc_for identificateur
%token <strVal> deux_points affectation point_virgule virgule barre_verticale
%token <strVal> add sous mul division et ou non sup inf supeg infeg diff egal
%token <strVal> par_ouvrante par_fermante croch_ouvrant croch_fermant
%token <strVal> arobase guillemet_fermant guillemet_ouvrant 
/* guillemet_ouvrant guillemet_fermant  */
%type <strVal> op_arithmetique op_logique signe_de_formatiage op_comparaison
%token <strVal> idf_entier idf_float idf_string idf_char
%token <strVal>  entier character 
%token <reel> reel
%token  <strVal> chaine cnst
%left et ou
%left egal diff
%left sup inf supeg infeg
%left add sous
%left mul division
%right affectation
%right mc_else

%start programme

%%
programme:
      identificateur mc_data declarations mc_end mc_code instructions mc_end mc_end
      { printf("Program parsed successfully.\n"); }
;


declarations:
      declaration
    | declarations declaration
    ;

declaration:
      type deux_points liste_identificateurs point_virgule
    | mc_const deux_points identificateur affectation valeur point_virgule
    {
        check_duplicate_symbol($3, nb_ligne, col);
        
        inserer($3, constante, 1, 0);
        
       
    }
    | mc_const deux_points identificateur affectation guillemet_ouvrant valeur guillemet_fermant point_virgule
    {
        check_duplicate_symbol($3, nb_ligne, col);
        inserer($3, constante, 1, 0);

    }
    | mc_vector deux_points identificateur croch_ouvrant cnst virgule cnst deux_points type croch_fermant point_virgule
    {
        check_duplicate_symbol($3, nb_ligne, col);
        if (atoi($5) > atoi($7)) {
            printf("Erreur (ligne%d)(colonne%d) : La taille du vecteur ne peut pas être supérieure à la taille maximale..\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }
        if (atoi($5) < 0 || atoi($7) < 0) {
            printf("Erreur (ligne%d)(colonne%d) : La taille du vecteur ne peut pas être négative.\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }
        inserer($3, type_courant, 0, atoi($7));
    }
    ;

liste_identificateurs:
      identificateur
      {
        check_duplicate_symbol($1, nb_ligne, col);
        inserer($1, type_courant, 0, 0);
    }
    | identificateur barre_verticale liste_identificateurs 
    {
        check_duplicate_symbol($1, nb_ligne, col);
         inserer($1, type_courant, 0, 0);
    }
    
    ;

type:
      mc_integer   { type_courant = TYPE_INTEGER ;  }
    | mc_float     { type_courant = TYPE_FLOAT ;    }
    | mc_character { type_courant = TYPE_CHAR  ;    }
    | mc_string    { type_courant = TYPE_STRING ; }
    ;



valeur:
      entier { $$.type = TYPE_INTEGER; $$.nom = $1; } 
    | reel { $$.type = TYPE_FLOAT ; char buffer[50];
    sprintf(buffer, "%f", $1); // convertir float en string
    $$.nom = strdup(buffer);   }
    | chaine { $$.type =TYPE_STRING ;    $$.nom = $1; }
    | character { $$.type =TYPE_CHAR ; $$.nom = $1; }
    ;

instructions:
      instruction
    | instructions instruction
    ;

instruction:
      affectations
      | io_read 
      | io_display
      | if_else
      | boucle
      ;

affectations:
    identificateur affectation expression point_virgule
    {
        check_declaration($1, nb_ligne, col);
        check_const_assignment($1, nb_ligne, col);
        DataType t = get_symbol_type($1);
       check_type_compatibility(t, $3.type, nb_ligne, col);
    }
    |vecteur affectation expression point_virgule 
    {
       check_type_compatibility($1.type, $3.type, nb_ligne, col);
    }
    ;


vecteur:
    identificateur croch_ouvrant cnst croch_fermant 
    {
        check_declaration($1, nb_ligne, col);
        int taille = get_symbol_size($1);
        int indice = atoi($3);

        if (indice < 0 || indice >= taille) {
            printf("Erreur (ligne%d)(colonne%d) : Indice du vecteur hors limites.\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }


        $$.type = get_symbol_type($1);
        $$.nom = $1;

    }
    ;
expression:
      cnst { DataType t1 = get_symbol_type($1); $$.type = t1; $$.nom = $1;  }
    | valeur { $$.type = $1.type; $$.nom = $1.nom; inserer($1.nom, $1.type, 0, 0); }
    | identificateur { $$.type = get_symbol_type($1); $$.nom = $1; inserer($1, $$.type, 0, 0); }
    | vecteur { $$.type = $1.type; $$.nom = $1.nom; inserer($1.nom, $1.type, 0, 0); }
    | identificateur op_arithmetique expression {
     DataType t1 = get_symbol_type($1); 
     if (t1==$3.type){
        $$.type = t1;

    }
    else{
        if (t1 == TYPE_INTEGER && $3.type == TYPE_FLOAT) {
            $$.type = TYPE_FLOAT;
        } else if (t1 == TYPE_FLOAT && $3.type == TYPE_INTEGER) {
            $$.type = TYPE_FLOAT;
        } else{
       $$.type = t1;
    }
    }   
    $$.nom = $1;

    if(strcmp($2, "/") == 0 ){
        if (strcmp($3.nom, "0") == 0) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Division par zéro\n", nb_ligne, col);
        
        exit(EXIT_FAILURE);
    }}
    
    }
    | valeur op_arithmetique expression
    {if ($1.type==$3.type){

        $$.type = $1.type;
    }
    else{
        if ($1.type == TYPE_INTEGER && $3.type == TYPE_FLOAT) {
            $$.type = TYPE_FLOAT;
        } else if ($1.type == TYPE_FLOAT && $3.type == TYPE_INTEGER) {
            $$.type = TYPE_FLOAT;
        } else {
            printf("Erreur (ligne%d)(colonne%d) : Type incompatible dans l'expression.\n", nb_ligne, col);
        }
    }
    
    $$.nom = $1.nom;
    
    if(strcmp($2, "/") == 0 ){
        if (strcmp($3.nom, "0") == 0) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Division par zéro\n", nb_ligne, col);
        exit(EXIT_FAILURE);
    }}
    }
    | vecteur op_arithmetique expression 
     { 
        if ($1.type==$3.type){

        $$.type = $1.type;
    }
    else{
        if ($1.type == TYPE_INTEGER && $3.type == TYPE_FLOAT) {
            $$.type = TYPE_FLOAT;
        } else if ($1.type == TYPE_FLOAT && $3.type == TYPE_INTEGER) {
            $$.type = TYPE_FLOAT;
        } else {
            printf("Erreur (ligne%d)(colonne%d) : Type incompatible dans l'expression.\n", nb_ligne, col);
        }
    }
    
    $$.nom = $1.nom;
    if(strcmp($2, "/") == 0 ){
        if (strcmp($3.nom, "0") == 0) {
        fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Division par zéro\n", nb_ligne, col);
        exit(EXIT_FAILURE);
    }}
    }
    | par_ouvrante expression par_fermante { $$.type = $2.type; $$.nom = $2.nom; }
    ;

op_arithmetique:
    add
  | sous
  | mul
  | division
    ;

op_logique:
    et
    | ou
    ;

/* expression_logique:
    expression deux_points op_logique deux_points expression
    | op_logique deux_points expression 
    ; */


io_read:
      mc_read par_ouvrante guillemet_ouvrant signe_de_formatiage guillemet_fermant deux_points arobase identificateur par_fermante point_virgule
      {
          check_declaration($8, nb_ligne, col);
          
          check_read_format($4, get_symbol_type($8), nb_ligne, col);
      }
    ;

io_display:
      mc_display par_ouvrante  chaine  deux_points identificateur par_fermante point_virgule
      { 
          check_declaration($5, nb_ligne, col);
          check_display_format($3, get_symbol_type($5), nb_ligne, col);
      }
    ;

signe_de_formatiage:
      idf_entier
    | idf_float
    | idf_char
    | idf_string
    ; 

if_else:
      mc_if par_ouvrante conditions par_fermante deux_points instructions mc_else deux_points instructions mc_end
        {
          if ($3 != TYPE_BOOL) yyerror("Condition IF non booléenne");
      }

    ;

conditions:
      condition
    | condition op_logique conditions
    {
          check_boolean_operation($1, $3, $2, nb_ligne, col);
          $$ = TYPE_BOOL;
      }
    
condition:
      expression  op_comparaison expression {
          check_comparison_operation($1.type, $3.type, $2, nb_ligne, col);
          $$ = TYPE_BOOL;
      }
    | expression op_logique expression
    {
          check_boolean_operation($1.type, $3.type, $2, nb_ligne, col);
          $$ = TYPE_BOOL;
      }
    |  non expression 
    {
          check_boolean_operation($2.type, $2.type, "non", nb_ligne, col);
          $$ = TYPE_BOOL;
      }
   
    ;

op_comparaison:
      sup
    | inf
    | supeg
    | infeg
    | diff
    | egal
    ;

boucle:
      mc_for par_ouvrante identificateur deux_points cnst deux_points identificateur par_fermante instructions mc_end
      {
          check_declaration($3, nb_ligne, col);
          check_declaration($7, nb_ligne,col);
          if (strcmp(type_to_string(get_symbol_type($3)) , TYPE_INTEGER)!=0 || strcmp(type_to_string(get_symbol_type($7)), TYPE_INTEGER)!=0)
              yyerror("Variables de boucle doivent être de type INTEGER");
      }
    ;

%%

void yyerror(const char *s) {
    printf("Syntax Error at line %d, column %d: %s\n", nb_ligne, col, s);
}

int main() {
 
    initialiser(); 
    yyparse();
    afficher();          
    liberer();          
    return 0;

}