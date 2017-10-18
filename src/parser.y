%{
  #include <bits/stdc++.h>
  #include "ClassDefs.h"
  int fl = 0;

  extern "C" int yylex();
  extern "C" int yyparse();
  extern "C" FILE *yyin;
  extern "C" int line_num;
  extern union Node yylval;
  extern "C" int errors;
  void yyerror(const char *s);
  class Prog* start = NULL;
  int errors=0;

%}

/* -------------	Tokens 		------------- */
%start program

%token IF
%token FOR
%token WHILE
%token ELSE
%token BREAK
%token CONTINUE
%token RETURN
%token AND
%token OR
%token DECL_BLOCK
%token CODE_BLOCK
%token <lit> TYPE
%token <val> NUMBER
%token <lit> IDENTIFIER
%token ETOK
%token EQUAL_EQUAL
%token LT_EQUAL
%token GT_EQUAL
%token GOTO
%token STRING_LITERAL
%token PRINT
%token READ
%token LABEL

/* -------------	Left Precedence		------------- */
%left EQUAL_EQUAL
%left LT_EQUAL
%left GT_EQUAL
%left '<'
%left '>'
%left OR
%left AND
%left NOT_EQUAL
%left '+' '-'
%left '*' '/'
%left '='
%left '!'

/* -------------	Non-Terminal Types		------------- */

%type <prog> program;
%type <decls> declaration_list;
%type <decl> single_line ;
%type <vars> variables;
%type <var> variable;

%%

program           :	    DECL_BLOCK '{' declaration_list '}'  CODE_BLOCK '{' statement_list '}'
                                                          { $$ = new Prog($3,$7); start = $$;}

/* ------------ decl_block starts   ---------------- */

declaration_list  :      /* epsilon */                    { $$ = new DeclList(); }
                  |      declaration_list single_line ';' { $$->push_back($2); }

single_line       :      TYPE variables                   { $$ = new Decl(string($1),$2); }

variables         :      variable                         { $$ = new Variables(); $$->push_back($1); }
                  |      variables ',' variable           { $$->push_back($3);}

variable          :      IDENTIFIER                       { $$ = new Variable(string("simple"),$1);    }
                  |      IDENTIFIER '[' NUMBER ']'        { $$ = new Variable(string("array"),$1,$3);  }
                  |      IDENTIFIER '=' NUMBER            { $$ = new Variable(string("simple"),$1,"0",$3); }

/* code_block starts */

statement_list    :     /* epsilon */                           {$$ = new StatementList();}
                  |     statement_list  statement               {$$->push_back($2);}
                  |     statement_list IDENTIFIER ':' statement {$$->push_back($4,string($2));}

statement         :     assign_expr      ';'  { $$ = $1 } /* new AssignExpr($1); } */
                  |     if_statement          { $$ = $1 } /* new IfStmt(); } */
                  |     while_statement       { $$ = $1 } /* new WhileStmt(); } */
                  |     for_statement         { $$ = $1 } /* new ForStmt(); } */
                  |     goto_statement   ';'  { $$ = $1 } /* new GotoStmt(); } */
                  |     print_statement  ';'  { $$ = $1 } /* new PrintStmt(); } */
                  |     read_statement   ';'  { $$ = $1 } /* new ReadStmt(); } */

assign_expr       :     IDENTIFIER                  '=' expr    { $$ = new AssignExpr($1,$3);}
                  |     IDENTIFIER '[' terminal ']' '=' expr    { $$ = new AssignExpr($1,$5,$3);}

terminal          :     IDENTIFIER  { $$ = $1; }
                  |     NUMBER      { $$ = $1; }

expr              :     terminal    { $$=$1; }
                  |     arith_expr  { $$=$1; }

arith_expr        :     expr '+' expr { $$ = new ArithExpr($1,"+",$3);}
                  |     expr '-' expr { $$ = new ArithExpr($1,"-",$3);}
                  |     expr '/' expr { $$ = new ArithExpr($1,"/",$3);}
                  |     expr '*' expr { $$ = new ArithExpr($1,"*",$3);}

bool_op           :     EQUAL_EQUAL
                  |     GT_EQUAL
                  |     LT_EQUAL
                  |     '>'
                  |     '<'

bool_expr         :     expr bool_op expr
                  |     bool_expr OR bool_expr
                  |     bool_expr AND bool_expr

if_statement      :     IF bool_expr '{' statement_list '}'
                  |     IF bool_expr '{' statement_list '}' ELSE '{' statement_list '}'

goto_statement    :     GOTO IDENTIFIER
                  |     GOTO IDENTIFIER IF bool_expr

while_statement   :     WHILE bool_expr '{' statement_list '}'

for_statement     :     FOR assign_expr ',' NUMBER '{' statement_list '}'
                  |     FOR assign_expr ',' NUMBER ',' NUMBER '{' statement_list '}'

read_statement    :     READ terminal

print_statement   :     PRINT STRING_LITERAL content
                  |     PRINT terminal content

content           :      /* epsilon */
                  |     ',' STRING_LITERAL  content
                  |     ',' terminal  content


%%

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
  if(!fl)
  printf("Parsing Done\n");
}
