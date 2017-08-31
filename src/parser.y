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
%token ARRAY
%token DECL_BLOCK
%token CODE_BLOCK
%token TYPE
%token NUMBER
%token IDENTIFIER
%token ETOK
%token EQUAL_EQUAL
%token LT_EQUAL
%token GT_EQUAL

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
                  |      TYPE ARRAY variables

variables         :      /* epsilon */
                  |      ',' IDENTIFIER variables
                  |      ',' ARRAY variables


/* code_block starts */

code_block        : CODE_BLOCK '{' statement_list '}'

statement_list    :     /* epsilon */
                  |     assign_expr ';' statement_list
                  |     if_statement    statement_list
                  |     while_statement statement_list
                  |     for_statement   statement_list


expr              :     IDENTIFIER
                  |     NUMBER
                  |     arith_expr

assign_expr       :     IDENTIFIER '=' arith_expr
                  |     IDENTIFIER '=' NUMBER
                  |     IDENTIFIER '=' IDENTIFIER

arith_expr        :     expr '+' expr
                  |     expr '-' expr
                  |     expr '/' expr
                  |     expr '*' expr

if_statement      :     IF bool_expr '{' statement_list '}'

bexpr             :     IDENTIFIER
                  |     NUMBER
                  |     bool_expr

bool_expr         :     bexpr EQUAL_EQUAL bexpr
                  |     bexpr GT_EQUAL bexpr
                  |     bexpr LT_EQUAL bexpr
                  |     bexpr '>'  bexpr
                  |     bexpr '<'  bexpr
                  |     bexpr OR   bexpr
                  |     bexpr AND  bexpr

while_statement   :     WHILE bool_expr '{' statement_list '}'

for_statement     :     FOR assign_expr ','





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
