%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"  


int nb_ligne= 1, col=1;
void yyerror(const char *s);
int yylex();
%}

%union {
    float reel;
    char* strVal;
}

%token <strVal> mc_data mc_end mc_code mc_vector mc_integer mc_float mc_character mc_string mc_const
%token <strVal> mc_read mc_display mc_if mc_else mc_for
%token <strVal> deux_points affectation point_virgule virgule barre_verticale
%token <strVal> add sous mul division et ou non sup inf supeg infeg diff egal
%token <strVal> par_ouvrante par_fermante croch_ouvrant croch_fermant
%token <strVal> arobase guillemet_fermant guillemet_ouvrant
/* guillemet_ouvrant guillemet_fermant  */
%token <strVal> idf_entier idf_float idf_string idf_char
%token <strVal> identificateur entier character 
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
    | mc_const deux_points identificateur affectation guillemet_ouvrant valeur guillemet_fermant point_virgule
    | mc_vector deux_points identificateur croch_ouvrant cnst virgule cnst deux_points type croch_fermant point_virgule
    ;

liste_identificateurs:
      identificateur
    | identificateur barre_verticale liste_identificateurs 
    ;

type:
      mc_integer
    | mc_float
    | mc_character
    | mc_string
    ;

valeur:
      entier
    | reel
    | chaine
    | character
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
      { printf("Affectation: IDF = expression;\n"); }
    ;
expression:
      cnst
    | valeur
    | identificateur
    | identificateur op_arithmetique cnst
    | identificateur op_arithmetique reel
    | identificateur op_arithmetique identificateur
    | par_ouvrante expression par_fermante

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
    | non
    ;

/* expression_logique:
    expression deux_points op_logique deux_points expression
    | op_logique deux_points expression 
    ; */


io_read:
      mc_read par_ouvrante guillemet_ouvrant signe_de_formatiage guillemet_fermant deux_points arobase identificateur par_fermante point_virgule
      { printf("Read Instruction\n"); }
    ;

io_display:
      mc_display par_ouvrante  chaine  deux_points identificateur par_fermante point_virgule
      { printf("Display Instruction\n"); }
    ;

signe_de_formatiage:
      idf_entier
    | idf_float
    | idf_char
    | idf_string
    ; 

if_else:
      mc_if par_ouvrante condition par_fermante deux_points instructions mc_else deux_points instructions mc_end
        { printf("IF-ELSE parsed\n"); }

    ;

condition:
      expression  op_comparaison expression
    | expression op_logique expression
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
      { printf("FOR Loop parsed\n"); }
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