/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SYNTAXIQUE_TAB_H_INCLUDED
# define YY_YY_SYNTAXIQUE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    mc_data = 258,                 /* mc_data  */
    mc_end = 259,                  /* mc_end  */
    mc_code = 260,                 /* mc_code  */
    mc_vector = 261,               /* mc_vector  */
    mc_integer = 262,              /* mc_integer  */
    mc_float = 263,                /* mc_float  */
    mc_character = 264,            /* mc_character  */
    mc_string = 265,               /* mc_string  */
    mc_const = 266,                /* mc_const  */
    mc_read = 267,                 /* mc_read  */
    mc_display = 268,              /* mc_display  */
    mc_if = 269,                   /* mc_if  */
    mc_else = 270,                 /* mc_else  */
    mc_for = 271,                  /* mc_for  */
    deux_points = 272,             /* deux_points  */
    affectation = 273,             /* affectation  */
    point_virgule = 274,           /* point_virgule  */
    virgule = 275,                 /* virgule  */
    barre_verticale = 276,         /* barre_verticale  */
    add = 277,                     /* add  */
    sous = 278,                    /* sous  */
    mul = 279,                     /* mul  */
    division = 280,                /* division  */
    et = 281,                      /* et  */
    ou = 282,                      /* ou  */
    non = 283,                     /* non  */
    sup = 284,                     /* sup  */
    inf = 285,                     /* inf  */
    supeg = 286,                   /* supeg  */
    infeg = 287,                   /* infeg  */
    diff = 288,                    /* diff  */
    egal = 289,                    /* egal  */
    par_ouvrante = 290,            /* par_ouvrante  */
    par_fermante = 291,            /* par_fermante  */
    croch_ouvrant = 292,           /* croch_ouvrant  */
    croch_fermant = 293,           /* croch_fermant  */
    arobase = 294,                 /* arobase  */
    guillemet_fermant = 295,       /* guillemet_fermant  */
    guillemet_ouvrant = 296,       /* guillemet_ouvrant  */
    idf_entier = 297,              /* idf_entier  */
    idf_float = 298,               /* idf_float  */
    idf_string = 299,              /* idf_string  */
    idf_char = 300,                /* idf_char  */
    identificateur = 301,          /* identificateur  */
    entier = 302,                  /* entier  */
    character = 303,               /* character  */
    reel = 304,                    /* reel  */
    chaine = 305,                  /* chaine  */
    cnst = 306                     /* cnst  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 12 "syntaxique.y"

    float reel;
    char* strVal;

#line 120 "syntaxique.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SYNTAXIQUE_TAB_H_INCLUDED  */
