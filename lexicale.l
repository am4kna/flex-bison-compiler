%{
#include <string.h>  
#include "stdlib.h"
#include "stdio.h"
#include "symbol_table.h"
#include "syntaxique.tab.h"


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

"DATA"      { yylval.strVal = strdup(yytext); col += yyleng; return mc_data;}
"END"       { yylval.strVal = strdup(yytext); col += yyleng; return mc_end; }
"CODE"      { yylval.strVal = strdup(yytext); col += yyleng; return mc_code; }
"VECTOR"    { yylval.strVal = strdup(yytext); col += yyleng; return mc_vector; }
"INTEGER"   { yylval.strVal = strdup(yytext); col += yyleng; return mc_integer; }
"FLOAT"     { yylval.strVal = strdup(yytext); col += yyleng; return mc_float; }
"CHAR"      { yylval.strVal = strdup(yytext); col += yyleng; return mc_character; }
"STRING"    { yylval.strVal = strdup(yytext); col += yyleng; return mc_string; }
"CONST"     { yylval.strVal = strdup(yytext); col += yyleng; return mc_const; }
"READ"      { yylval.strVal = strdup(yytext); col += yyleng; return mc_read; }
"DISPLAY"   { yylval.strVal = strdup(yytext); col += yyleng; return mc_display; }
"IF"        { yylval.strVal = strdup(yytext); col += yyleng; return mc_if; }

"ELSE"      { yylval.strVal = strdup(yytext); col += yyleng; return mc_else; }
"FOR"       { yylval.strVal = strdup(yytext); col += yyleng; return mc_for; }

":"         { yylval.strVal = strdup(yytext); col += yyleng; return deux_points; }
"="         { yylval.strVal = strdup(yytext); col += yyleng; return affectation; }
";"         { yylval.strVal = strdup(yytext); col += yyleng; return point_virgule; }
","         { yylval.strVal = strdup(yytext); col += yyleng; return virgule; }
"|"         { yylval.strVal = strdup(yytext); col += yyleng; return barre_verticale; }
"+"         { yylval.strVal = strdup(yytext); col += yyleng; return add; }
"-"         { yylval.strVal = strdup(yytext); col += yyleng; return sous; }
"*"         { yylval.strVal = strdup(yytext); col += yyleng; return mul; }
"/"         { yylval.strVal = strdup(yytext); col += yyleng; return division; }
".AND."       { yylval.strVal = strdup(yytext); col += yyleng; return et; }
".OR."        { yylval.strVal = strdup(yytext); col += yyleng; return ou; }
".NOT."       { yylval.strVal = strdup(yytext); col += yyleng; return non; }
".G."         { yylval.strVal = strdup(yytext); col += yyleng; return sup; }
".L."         { yylval.strVal = strdup(yytext); col += yyleng; return inf; }
".GE."        { yylval.strVal = strdup(yytext); col += yyleng; return supeg; }
".LE."        { yylval.strVal = strdup(yytext); col += yyleng; return infeg; }
".DI."        { yylval.strVal = strdup(yytext); col += yyleng; return diff; }
".EQ."        { yylval.strVal = strdup(yytext); col += yyleng; return egal; }
"("         { yylval.strVal = strdup(yytext); col += yyleng; return par_ouvrante; }
")"         { yylval.strVal = strdup(yytext); col += yyleng; return par_fermante; }
"["         { yylval.strVal = strdup(yytext); col += yyleng; return croch_ouvrant; }
"]"         { yylval.strVal = strdup(yytext); col += yyleng; return croch_fermant; }
"“"         { yylval.strVal = strdup(yytext); col += yyleng; return guillemet_ouvrant; }
"”"         { yylval.strVal = strdup(yytext); col += yyleng; return guillemet_fermant; }
"@"         { yylval.strVal = strdup(yytext); col += yyleng; return arobase; }
"$"         { yylval.strVal = strdup(yytext); col += yyleng; return idf_entier; }
"%"         { yylval.strVal = strdup(yytext); col += yyleng; return idf_float; }
"#"         { yylval.strVal = strdup(yytext); col += yyleng; return idf_string; }
"&"         { yylval.strVal = strdup(yytext); col += yyleng; return idf_char; }

{chaine}    {
            printf("Matched chaine: %s\n", yytext);  // <== Add this line
 
            
             col += yyleng; 
             yylval.strVal = strdup(yytext); //changed yylval.chaine to strVal cause i noticed rahi deja nommee strVal dans ce doc
             return chaine;
         }

{idf} {
    printf("Lexing identifier: %s at line %d, column %d\n", yytext, nb_ligne, col);

    yylval.strVal = strdup(yytext);
    col += yyleng;
    return identificateur;
}


{reel}      {
             
             col += yyleng; 
             yylval.reel = atof(yytext); 
             return reel;
         }



{character} {
          
             col += yyleng; 
             return character;
         }

{cnst}      {
        
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
