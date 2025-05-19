flex lexicale.l
bison -d syntaxique.y
gcc lex.yy.c syntaxique.tab.c symbol_table.c semantic.c optimisation.c quadruplet.c -o parser.exe
parser.exe < input.txt
