%{
  #include <stdio.h>
  #include <stdlib.h>
  FILE *yyin;
  int yylex (void);
  void yyerror (char const *s);
%}
%token IF
%token FOR
%token WHILE
%token ELSE
%token BREAK
%token CONTINUE
%token RETURN
%token AND
%token OR
%token ARRAY_VALUE
%token DECL_BLOCK
%token CODE_BLOCK
%token TYPE
%token NUMBER
%token IDENTIFIER
%token ETOK
%token EQUAL_EQUAL
%token LT_EQUAL
%token GT_EQUAL
%token GOTO
%token STRING_LITERAL
%token PRINT
%token READ
%token LABEL

%left EQUAL_EQUAL
%left LT_EQUAL
%left GT_EQUAL
%left '<'
%left '>'
%left OR
%left AND
%left '+' '-'
%left '*' '/'
%left '='
%left '!'
%%

program           :	     decl_block  code_block

/* decl_block starts*/
decl_block        :      DECL_BLOCK '{' declaration_list '}'

declaration_list  :      /* epsilon */
                  |      single_line ';' declaration_list


single_line       :      TYPE IDENTIFIER variables

variables         :      /* epsilon */
                  |      ',' IDENTIFIER variables


/* code_block starts */

code_block        : CODE_BLOCK '{' statement_list '}'

statement_list    :     /* epsilon */
                  |     LABEL                statement_list
                  |     assign_expr     ';'  statement_list
                  |     if_statement         statement_list
                  |     while_statement      statement_list
                  |     for_statement        statement_list
                  |     goto_statement  ';'  statement_list
                  |     print_statement ';'  statement_list
                  |     read_statement  ';'  statement_list

assign_expr       :     IDENTIFIER  '=' expr
                  |     ARRAY_VALUE '=' expr

terminal          :     IDENTIFIER
                  |     NUMBER
                  |     ARRAY_VALUE

expr              :     terminal
                  |     arith_expr

arith_expr        :     expr '+' expr
                  |     expr '-' expr
                  |     expr '/' expr
                  |     expr '*' expr

/*
bexpr             :     terminal
                  |     bool_expr
*/
bool_op           :     EQUAL_EQUAL
                  |     GT_EQUAL
                  |     LT_EQUAL
                  |     '>'
                  |     '<'

bool_expr         :     terminal bool_op terminal
                  |     bool_expr OR bool_expr
                  |     bool_expr AND bool_expr

if_statement      :     IF bool_expr '{' statement_list '}'
                  |     IF bool_expr '{' statement_list '}' ELSE '{' statement_list '}'

goto_statement    :     GOTO IDENTIFIER
                  |     GOTO IDENTIFIER IF bool_expr

while_statement   :     WHILE bool_expr '{' statement_list '}'

for_statement     :     FOR assign_expr ',' NUMBER '{' statement_list '}'
                  |     FOR assign_expr ',' NUMBER ',' NUMBER '{' statement_list '}'

read_statement    :     READ IDENTIFIER

print_statement   :     PRINT STRING_LITERAL content
                  |     PRINT terminal content

content           :      /* epsilon */
                  |     ',' STRING_LITERAL  content
                  |     ',' terminal  content


%%

void yyerror (char const *s)
{
       fprintf (stderr, "%s\n", s);
}

int main(int argc, char *argv[])
{
	if (argc == 1 ) {
		fprintf(stderr, "Correct usage: bcc filename\n");
		exit(1);
	}

	if (argc > 2) {
		fprintf(stderr, "Passing more arguments than necessary.\n");
		fprintf(stderr, "Correct usage: bcc filename\n");
	}

	yyin = fopen(argv[1], "r");

	yyparse();
}
