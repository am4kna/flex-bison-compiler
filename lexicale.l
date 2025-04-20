%{
#include <string.h>  
#include "stdlib.h"
#include "stdio.h"
#include "syntaxique.tab.h"
#include "symbol_table.c"

extern int nb_ligne ;
extern int col ;
extern YYSTYPE yylval;
%}

%option noyywrap

lettre_maj      [A-Z]
lettre_min      [a-z]
chiffre         [0-9]
chaine      \"([^\"\n]|\\.)*\"
idf         ({lettre_maj})({lettre_min}|{lettre_maj}|{chiffre})*
entier      -?{chiffre}+

reel        -?{chiffre}+"."{chiffre}+
character   \'([^\'\n]|\\.)\'
cnst        ({entier})
comment     "§"[^\n]*
%%

"DATA"      {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_data;}
"END"       {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_end; }
"CODE"      {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_code; }
"VECTOR"    {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_vector; }
"INTEGER"   {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_integer; }
"FLOAT"     {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_float; }
"CHAR"      {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_character; }
"STRING"    {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_string; }
"CONST"     {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_const; }
"READ"      {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_read; }
"DISPLAY"   {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_display; }
"IF"        {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_if; }

"ELSE"      {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_else; }
"FOR"       {inserer(yytext, "mc"); yylval.strVal = strdup(yytext); col += yyleng; return mc_for; }

":"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return deux_points; }
"="         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return affectation; }
";"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return point_virgule; }
","         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return virgule; }
"|"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return barre_verticale; }
"+"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return add; }
"-"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return sous; }
"*"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return mul; }
"/"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return division; }
".AND."       {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return et; }
".OR."        {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return ou; }
".NOT."       {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return non; }
".G."         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return sup; }
".L."         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return inf; }
".GE."        {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return supeg; }
".LE."        {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return infeg; }
".DI."        {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return diff; }
".EQ."        {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return egal; }
"("         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return par_ouvrante; }
")"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return par_fermante; }
"["         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return croch_ouvrant; }
"]"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return croch_fermant; }
"“"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return guillemet_ouvrant; }
"”"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return guillemet_fermant; }
"@"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return arobase; }
"$"        { inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return idf_entier; }
"%"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return idf_float; }
"#"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return idf_string; }
"&"         {inserer(yytext, "sep"); yylval.strVal = strdup(yytext); col += yyleng; return idf_char; }

{chaine}    {
            printf("Matched chaine: %s\n", yytext);  // <== Add this line
 
             inserer(yytext, "chaine"); 
             col += yyleng; 
             yylval.strVal = strdup(yytext); //changed yylval.chaine to strVal cause i noticed rahi deja nommee strVal dans ce doc
             return chaine;
         }

{idf} {
    printf("Lexing identifier: %s at line %d, column %d\n", yytext, nb_ligne, col);
    inserer(yytext, "idf");
    yylval.strVal = strdup(yytext);
    col += yyleng;
    return identificateur;
}


{reel}      {
             inserer(yytext, "reel"); 
             col += yyleng; 
             yylval.reel = atof(yytext); 
             return reel;
         }



{character} {
             inserer(yytext, "character"); 
             col += yyleng; 
             return character;
         }

{cnst}      {
             inserer(yytext, "cnst"); 
             yylval.strVal=strdup(yytext); 
             col += yyleng; 
             return cnst;
         }




[ \t]+      { col += yyleng; }  /* Ignorer les espaces et tabulations */

{comment}   { printf("commentaire sur 1 seule ligne: %d\n", nb_ligne); }
\n          { nb_ligne++; col = 1; }  
. { 
    fprintf(stderr, "Erreur lexicale à la ligne %d, colonne %d: caractère inconnu '%s'\n", nb_ligne, col, yytext); 
    exit(1);  
}

%%

