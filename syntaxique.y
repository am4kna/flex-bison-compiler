%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symbol_table.h"
    int yylex(); 
    int yyerror(const char* msg);  
%}

%union {
    float reel;
    char* strVal;
}

%token <strVal> mc_data mc_end mc_code mc_vector mc_integer mc_float mc_character mc_string mc_const
%token <strVal> mc_read mc_display mc_if mc_else mc_for mc_endfor
%token <strVal> deux_points affectation point_virgule virgule barre_verticale
%token <strVal> add sous mul division et ou non sup inf supeg infeg diff egal
%token <strVal> par_ouvrante par_fermante croch_ouvrant croch_fermant
%token <strVal> guillemet_ouvrant guillemet_fermant arobase
%token <strVal> idf_entier idf_float idf_string idf_char
%token <strVal> identificateur entier character 
%token <reel> reel
%token  <strVal> chaine cnst

/* Operator precedence to resolve conflicts */
%left et ou
%left egal diff
%left sup inf supeg infeg
%left add sous
%left mul division
%right affectation
%precedence mc_else

%%

programme : mc_data section_data mc_code section_code mc_end { printf("Syntax is correct.\n"); };

section_data : declarations |  ;

section_code : instructions |  ;

/* --- Declarations --- */
declarations : declarations declaration | declaration ;

declaration : mc_const identificateur affectation valeur point_virgule
            | type liste_identificateurs point_virgule
            | mc_vector identificateur croch_ouvrant entier virgule entier deux_points type  croch_fermant point_virgule ;
            
liste_identificateurs : identificateur 
                      | liste_identificateurs virgule identificateur ;

type : mc_integer | mc_float | mc_character | mc_string ;

/* --- Instructions --- */
instructions : instructions instruction | instruction ;

instruction : mc_read par_ouvrante guillemet_ouvrant chaine guillemet_fermant deux_points arobase identificateur par_fermante point_virgule
            | mc_display par_ouvrante guillemet_ouvrant chaine guillemet_fermant deux_points identificateur par_fermante point_virgule
            | mc_if par_ouvrante condition par_fermante instructions mc_end
            | mc_if par_ouvrante condition par_fermante instructions mc_else instructions mc_end
            | mc_for par_ouvrante identificateur deux_points entier deux_points condition par_fermante instructions mc_endfor
            | identificateur affectation expr point_virgule ;

/* --- Conditions --- */
condition : expr egal expr
          | expr diff expr
          | expr sup expr
          | expr inf expr
          | expr supeg expr
          | expr infeg expr
          | non expr
          | condition et condition
          | condition ou condition ;

/* --- Expressions --- */
expr : identificateur
     | entier
     | reel
     | chaine
     | character
     | expr add expr
     | expr sous expr
     | expr mul expr
     | expr division expr
     | par_ouvrante expr par_fermante ;

valeur : entier | reel | chaine | character ;

%%

int yyerror(const char* msg) {
    fprintf(stderr, "Syntax error: %s at line %d, column %d\n", msg, nb_ligne, col);
}

int main() {
    return yyparse();
}
