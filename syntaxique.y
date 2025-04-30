%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol_table.h"  
#include "semantic.h"
#include "quadruplet.h"
#include <string.h>
#include "types.h"        

int nb_ligne= 1, col=1;
void yyerror(const char *s);
int yylex();
DataType type_courant;

%}

%union {
    float reel;
    char* strVal;
    DataType type;
    ExprAttr expr;
    struct {
        char* begin_label;
        char* true_label;
        char* false_label;
        char* end_label;
    } label_pair;
}
%type <type>  type condition if_else conditions
%type <expr> expression vecteur valeur 
%type <label_pair> if_header
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
      identificateur mc_data 
      { 
          quad_init();
      }
      declarations mc_end mc_code instructions mc_end mc_end
      { 
          printf("Program parsed successfully.\n");
          quad_print();
      }
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
        
        // generate quadruplet for constant assignment
        quad_assign($3, $5.quad_addr);
    }
    | mc_const deux_points identificateur affectation guillemet_ouvrant valeur guillemet_fermant point_virgule
    {
        check_duplicate_symbol($3, nb_ligne, col);
        inserer($3, constante, 1, 0);
        
        // generate quadruplet for string constant assignment
        quad_assign($3, $6.quad_addr);
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
    | mc_string    { type_courant = TYPE_STRING ;   }
    ;

valeur:
      entier { 
        $$.type = TYPE_INTEGER; 
        $$.nom = $1; 
        $$.quad_addr = strdup($1);
      } 
    | reel { 
        $$.type = TYPE_FLOAT; 
        char buffer[50];
        sprintf(buffer, "%f", $1); // convertir float en string
        $$.nom = strdup(buffer);
        $$.quad_addr = strdup(buffer);
      }
    | chaine { 
        $$.type = TYPE_STRING;
        $$.nom = $1;
        $$.quad_addr = strdup($1);
      }
    | character { 
        $$.type = TYPE_CHAR;
        $$.nom = $1;
        $$.quad_addr = strdup($1);
      }
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
        set_initialized($1);  
        
        // generate assignment quadruplet
        quad_assign($1, $3.quad_addr);
    }
    | vecteur affectation expression point_virgule 
    {
        check_type_compatibility($1.type, $3.type, nb_ligne, col);
        set_initialized($1.nom);  
        
        // generate vector assignment quadruplet
        char* index_start = strchr($1.nom, '[') + 1;
        char* index_end = strchr($1.nom, ']');
        size_t index_length = index_end - index_start;
        char* index = (char*)malloc(index_length + 1);
        strncpy(index, index_start, index_length);
        index[index_length] = '\0';
        
        char* base_name = (char*)malloc(strlen($1.nom) + 1);
        strncpy(base_name, $1.nom, index_start - $1.nom - 1);
        base_name[index_start - $1.nom - 1] = '\0';
        
        quad_vector_assign(base_name, index, $3.quad_addr);
        
        free(index);
        free(base_name);
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
        $$.nom = (char*)malloc(strlen($1) + strlen($3) + 3); // +3 for '[', ']', and '\0'
        sprintf($$.nom, "%s[%s]", $1, $3);
        
        // generate vector access quadruplet
        $$.quad_addr = new_temp();
        quad_add(OP_ARRAY_ACCESS, $1, $3, $$.quad_addr);
    }
    ;

expression:
      cnst { 
        DataType t1 = get_symbol_type($1); 
        $$.type = t1; 
        $$.nom = $1;
        $$.quad_addr = strdup($1);
      }
    | valeur { 
        $$.type = $1.type; 
        $$.nom = $1.nom; 
        $$.quad_addr = $1.quad_addr;
        inserer($1.nom, $1.type, 0, 0); 
      }
    | identificateur {        
        check_initialization($1, nb_ligne, col);  // Check initialization before use
        $$.type = get_symbol_type($1); 
        $$.nom = $1; 
        $$.quad_addr = strdup($1);
        inserer($1, $$.type, 0, 0); 
      }
    | vecteur {         
        check_initialization($1.nom, nb_ligne, col);  // Check initialization for vectors too
        $$.type = $1.type; 
        $$.nom = $1.nom; 
        $$.quad_addr = $1.quad_addr;
        inserer($1.nom, $1.type, 0, 0); 
      }
    | identificateur op_arithmetique expression {
        DataType t1 = get_symbol_type($1); 
        if (t1 == $3.type) {
            $$.type = t1;
        } else {
            if (t1 == TYPE_INTEGER && $3.type == TYPE_FLOAT) {
                $$.type = TYPE_FLOAT;
            } else if (t1 == TYPE_FLOAT && $3.type == TYPE_INTEGER) {
                $$.type = TYPE_FLOAT;
            } else {
                $$.type = t1;
            }
        }   
        $$.nom = $1;
        
        // generate arithmetic operation quadruplet
        OperationType op;
        if (strcmp($2, "+") == 0) op = OP_ADD;
        else if (strcmp($2, "-") == 0) op = OP_SUB;
        else if (strcmp($2, "*") == 0) op = OP_MUL;
        else if (strcmp($2, "/") == 0) op = OP_DIV;
        else op = OP_ADD; // Default
        
        if(strcmp($2, "/") == 0 && strcmp($3.nom, "0") == 0) {
            fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Division par zéro\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }
        
        $$.quad_addr = new_temp();
        quad_add(op, $1, $3.quad_addr, $$.quad_addr);
      }
    | valeur op_arithmetique expression {
        if ($1.type == $3.type) {
            $$.type = $1.type;
        } else {
            if ($1.type == TYPE_INTEGER && $3.type == TYPE_FLOAT) {
                $$.type = TYPE_FLOAT;
            } else if ($1.type == TYPE_FLOAT && $3.type == TYPE_INTEGER) {
                $$.type = TYPE_FLOAT;
            } else {
                printf("Erreur (ligne%d)(colonne%d) : Type incompatible dans l'expression.\n", nb_ligne, col);
            }
        }
        
        $$.nom = $1.nom;
        
        // generate arithmetic operation quadruplet
        OperationType op;
        if (strcmp($2, "+") == 0) op = OP_ADD;
        else if (strcmp($2, "-") == 0) op = OP_SUB;
        else if (strcmp($2, "*") == 0) op = OP_MUL;
        else if (strcmp($2, "/") == 0) op = OP_DIV;
        else op = OP_ADD; // Default
        
        if(strcmp($2, "/") == 0 && strcmp($3.nom, "0") == 0) {
            fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Division par zéro\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }
        
        $$.quad_addr = new_temp();
        quad_add(op, $1.quad_addr, $3.quad_addr, $$.quad_addr);
      }
    | vecteur op_arithmetique expression { 
        if ($1.type == $3.type) {
            $$.type = $1.type;
        } else {
            if ($1.type == TYPE_INTEGER && $3.type == TYPE_FLOAT) {
                $$.type = TYPE_FLOAT;
            } else if ($1.type == TYPE_FLOAT && $3.type == TYPE_INTEGER) {
                $$.type = TYPE_FLOAT;
            } else {
                printf("Erreur (ligne%d)(colonne%d) : Type incompatible dans l'expression.\n", nb_ligne, col);
            }
        }
        
        $$.nom = $1.nom;
        
        // generate arithmetic operation quadruplet
        OperationType op;
        if (strcmp($2, "+") == 0) op = OP_ADD;
        else if (strcmp($2, "-") == 0) op = OP_SUB;
        else if (strcmp($2, "*") == 0) op = OP_MUL;
        else if (strcmp($2, "/") == 0) op = OP_DIV;
        else op = OP_ADD; 
        
        if(strcmp($2, "/") == 0 && strcmp($3.nom, "0") == 0) {
            fprintf(stderr, "Erreur sémantique (ligne %d)(colonne%d): Division par zéro\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }
        
        $$.quad_addr = new_temp();
        quad_add(op, $1.quad_addr, $3.quad_addr, $$.quad_addr);
      }
    | par_ouvrante expression par_fermante { 
        $$.type = $2.type; 
        $$.nom = $2.nom;
        $$.quad_addr = $2.quad_addr;
      }
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

io_read:
    mc_read par_ouvrante guillemet_ouvrant signe_de_formatiage guillemet_fermant deux_points arobase identificateur par_fermante point_virgule
    {
        check_declaration($8, nb_ligne, col);
        check_read_format($4, get_symbol_type($8), nb_ligne, col);
        
        // Generate READ quadruplet
        quad_read($4, $8);
    }
    ;

io_display:
    mc_display par_ouvrante chaine deux_points identificateur par_fermante point_virgule
    { 
        check_declaration($5, nb_ligne, col);
        check_display_format($3, get_symbol_type($5), nb_ligne, col);
        
        // Generate DISPLAY quadruplet
        quad_display($3, $5);
    }
    ;

signe_de_formatiage:
    idf_entier
    | idf_float
    | idf_char
    | idf_string
    ; 

if_header:
    mc_if par_ouvrante conditions par_fermante
    {
        // Create all needed labels
        $$.true_label = new_label();  // Label for true branch
        $$.false_label = new_label(); // Label for false branch
        $$.end_label = new_label();   // Label for end of if-else
        
        // Generate branch on zero condition quadruplet
        quad_bz($3 == TYPE_BOOL ? "last_condition" : "0", $$.false_label);
        
        // Add true branch label
        quad_label($$.true_label);
    }
    ;

if_else:
    if_header deux_points instructions 
    {
        // Jump to end after true branch
        quad_goto($1.end_label);
        
        // Add false branch label
        quad_label($1.false_label);
    }
    mc_else deux_points instructions 
    {
        // Add end label
        quad_label($1.end_label);
    }
    mc_end
    {
        if ($1.true_label) free($1.true_label);
        if ($1.false_label) free($1.false_label);
        if ($1.end_label) free($1.end_label);
        $$ = TYPE_BOOL; // Return type for proper semantic checking
    }
    ;

conditions:
    condition {
        $$ = TYPE_BOOL;
    
    }
    | condition op_logique conditions
    {
        check_boolean_operation($1, $3, $2, nb_ligne, col);
        $$ = TYPE_BOOL;
        
        // Generate logical operation quadruplet
        OperationType op;
        if (strcmp($2, "&&") == 0) op = OP_AND;
        else if (strcmp($2, "||") == 0) op = OP_OR;
        else op = OP_AND; // Default
        
        char* temp = new_temp();
        // Generate quadruplet with the actual operands
        char* op1 = strdup("last_condition"); // Assuming last condition is stored here
        char* prev_condition = new_temp(); // Get result of previous condition
        quad_add(op, op1, prev_condition, temp);
        
        // Store the new condition result in "last_condition"
        quad_assign("last_condition", temp);
        
        free(temp);
        free(op1);
        free(prev_condition); 
    }
    ;

    
condition:
    expression op_comparaison expression {
        check_comparison_operation($1.type, $3.type, $2, nb_ligne, col);
        $$ = TYPE_BOOL;
          
        // Get correct operation type
        OperationType op;
        if (strcmp($2, ".GE.") == 0) op = OP_CMP_GE;
        else if (strcmp($2, ".L.") == 0) op = OP_CMP_L;
        else if (strcmp($2, ".G.") == 0) op = OP_CMP_G;
        else if (strcmp($2, ".LE.") == 0) op = OP_CMP_LE;
        else if (strcmp($2, ".DI.") == 0) op = OP_CMP_NE;
        else if (strcmp($2, ".EQ.") == 0) op = OP_CMP_EQ;
        else {
            printf("Erreur (ligne%d)(colonne%d) : Opération de comparaison inconnue.\n", nb_ligne, col);
            exit(EXIT_FAILURE);
        }


        
        // Generate comparison quadruplet
        char* temp = new_temp();
        quad_add(op, $1.quad_addr, $3.quad_addr, temp);
        
        // Store result in "last_condition"
        quad_assign("last_condition", temp);
        free(temp);
    }
    | expression op_logique expression
    {
        check_boolean_operation($1.type, $3.type, $2, nb_ligne, col);
        $$ = TYPE_BOOL;
          
        // Generate logical operation
        OperationType op;
        if (strcmp($2, "&&") == 0) op = OP_AND;
        else if (strcmp($2, "||") == 0) op = OP_OR;
        
        // Generate logical quadruplet
        char* temp = new_temp();
        quad_add(op, $1.quad_addr, $3.quad_addr, temp);
        
        // Store result in "last_condition"
        quad_assign("last_condition", temp);
        free(temp);
    }
    | non expression 
    {
        check_boolean_operation($2.type, $2.type, "non", nb_ligne, col);
        $$ = TYPE_BOOL;
          
        // Generate NOT operation
        char* temp = new_temp();
        quad_add(OP_NOT, $2.quad_addr, NULL, temp);
        
        // Store result in "last_condition"
        quad_assign("last_condition", temp);
        free(temp);
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
    mc_for par_ouvrante identificateur deux_points cnst deux_points identificateur par_fermante 
    {
        // Check loop variables are of correct type
        check_declaration($3, nb_ligne, col);
        check_declaration($7, nb_ligne, col);
        if (get_symbol_type($3) != TYPE_INTEGER || get_symbol_type($7) != TYPE_INTEGER) {
            yyerror("Variables de boucle doivent être de type INTEGER");
        }
        
        // Create labels for loop
        char* loop_start = new_label();
        char* loop_end = new_label();
        
        // Initialize loop counter
        quad_assign($3, $5);
        
        // Create label for loop start
        quad_label(loop_start);
        
        // Compare loop counter with limit
        char* temp = new_temp();
        quad_add(OP_CMP_LE, $3, $7, temp);
        
        // Create conditional branch to end if counter > limit
        quad_bz(temp, loop_end);
        
        free(temp);
        
        // Save labels for end of loop
        // We need to pass these, but we'll use a simpler approach for now
        // In a real implementation, you'd add fields to the yylval union
    }
    instructions mc_end
    {
        // Get the previously saved loop labels
        // In a real implementation, you'd retrieve them from a stack or passed value
        
        // Increment loop counter
        char* temp = new_temp();
        quad_add(OP_ADD, $3, "1", temp);
        quad_assign($3, temp);
        free(temp);
        
        // Jump back to loop start
        quad_goto("loop_start"); // Would be the actual saved label
        
        // Add loop end label
        quad_label("loop_end"); // Would be the actual saved label
        
        if (strcmp(type_to_string(get_symbol_type($3)), "INTEGER") != 0 || 
            strcmp(type_to_string(get_symbol_type($7)), "INTEGER") != 0) {
            yyerror("Variables de boucle doivent être de type INTEGER");
        }
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