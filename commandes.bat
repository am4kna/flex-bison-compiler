flex lexicale.l
bison -d syntaxique.y
gcc lex.yy.c syntaxique.tab.c -o parser.exe
gcc lex.yy.c syntaxique.tab.c symbol_table.c semantic.c -o parser.exe
parser.exe < input.txt 

