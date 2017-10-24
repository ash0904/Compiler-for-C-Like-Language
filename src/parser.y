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
%token <num> NUMBER
%token <lit> IDENTIFIER
%token ETOK
%token <lit> EQUAL_EQUAL LT_EQUAL GT_EQUAL NOT_EQUAL '>' '<'
%token GOTO
%token <lit> STRING_LITERAL
%token PRINT
%token PRINTLN
%token READ
%token LABEL

/* -------------	Left Precedence		------------- */
%left NOT_EQUAL
%left EQUAL_EQUAL
%left LT_EQUAL
%left GT_EQUAL
%left '<'
%left '>'
%left OR
%left AND
%left '+' '-'
%left '*' '/' '%'
%left '='
%left '!'

/* -------------	Non-Terminal Types		------------- */

%type <prog> program;
%type <decls> declaration_list;
%type <vars> single_line ;
%type <vars> variables;
%type <var> variable;
%type <stmt> statement;
%type <stmts> statement_list;
%type <term> terminal content;
%type <expr> expr;
%type <arithExpr> arith_expr;
%type <assignExpr> assign_expr;
%type <lit> bool_op;
%type <boolExpr>  bool_expr;
%type <ifStmt> if_statement;
%type <whileStmt> while_statement;
%type <gotoStmt> goto_statement;
%type <forStmt> for_statement;
%type <readStmt> read_statement;
%type <printStmt> print_statement contents;
%%

program           :	    DECL_BLOCK '{' declaration_list '}'  CODE_BLOCK '{' statement_list '}'
                                                          { $$ = new Prog($3,$7); start = $$;}

/* ------------ decl_block starts   ---------------- */

declaration_list  :      /* epsilon */                    { $$ = new DeclList(); }
                  |      declaration_list single_line ';' { $$->push_back($2); }

single_line       :      TYPE variables                   { $$ = $2; $$->dataType = $1; }

variables         :      variable                         { $$ = new Variables(); $$->push_back($1); }
                  |      variables ',' variable           { $$->push_back($3);}

variable          :      IDENTIFIER                       { $$ = new Variable(string("simple"),$1);    }
                  |      IDENTIFIER '[' NUMBER ']'        { $$ = new Variable(string("array"),$1,$3);  }
                  |      IDENTIFIER '=' NUMBER            { $$ = new Variable(string("initialised"),$1,"0",$3); }

/* code_block starts */

statement_list    :     /* epsilon */                           { $$ = new StatementList();}
                  |     statement_list  statement               { $$->push_back($2);}
                  |     statement_list IDENTIFIER ':' statement { $$->push_back($4,string($2));}

statement         :     assign_expr      ';'  { $$ = $1; }
                  |     if_statement          { $$ = $1; }
                  |     while_statement       { $$ = $1; }
                  |     for_statement         { $$ = $1; }
                  |     goto_statement   ';'  { $$ = $1; }
                  |     print_statement  ';'  { $$ = $1; }
                  |     read_statement   ';'  { $$ = $1; }

assign_expr       :     IDENTIFIER                  '=' expr    { $$ = new AssignExpr($1,$3);   }
                  |     IDENTIFIER '[' terminal ']' '=' expr    { $$ = new AssignExpr($1,$6,$3);}

terminal          :     IDENTIFIER  { $$ = new Terminal("id",$1);  }
                  |     NUMBER      { $$ = new Terminal("num",$1); }

expr              :     terminal                      { $$ = new Expr($1); }
                  |     IDENTIFIER '[' terminal ']'   { $$ = new Expr(new Terminal("array",$1,$3)); }
                  |     arith_expr                    { $$ = new Expr($1); }

arith_expr        :     expr '+' expr { $$ = new ArithExpr($1,"+",$3); }
                  |     expr '-' expr { $$ = new ArithExpr($1,"-",$3); }
                  |     expr '/' expr { $$ = new ArithExpr($1,"/",$3); }
                  |     expr '*' expr { $$ = new ArithExpr($1,"*",$3); }
                  |     expr '%' expr { $$ = new ArithExpr($1,"%",$3); }

bool_op           :     EQUAL_EQUAL { $$ = $1; }
                  |     GT_EQUAL    { $$ = $1; }
                  |     LT_EQUAL    { $$ = $1; }
                  |     NOT_EQUAL   { $$ = $1; }
                  |     '>'         { $$ = $1; }
                  |     '<'         { $$ = $1; }

bool_expr         :     expr bool_op expr         { $$ = new BoolExpr($1,$2,$3); }
                  |     bool_expr OR bool_expr    { $$ = new BoolExpr($1,"OR",$3); }
                  |     bool_expr AND bool_expr   { $$ = new BoolExpr($1,"AND",$3); }

if_statement      :     IF bool_expr '{' statement_list '}'                             { $$ = new IfStmt("single",$2,$4);  }
                  |     IF bool_expr '{' statement_list '}' ELSE '{' statement_list '}' { $$ = new IfStmt("else",$2,$4,$8); }

goto_statement    :     GOTO IDENTIFIER                        { $$ = new GotoStmt("single",$2);  }
                  |     GOTO IDENTIFIER IF bool_expr           { $$ = new GotoStmt("cond",$2,$4); }

while_statement   :     WHILE bool_expr '{' statement_list '}' { $$ = new WhileStmt($2,$4); }

for_statement     :     FOR assign_expr ',' terminal '{' statement_list '}'              { $$ = new ForStmt($2,$4,$6);    }
                  |     FOR assign_expr ',' terminal ',' terminal '{' statement_list '}' { $$ = new ForStmt($2,$4,$6,$8); }

read_statement    :     READ terminal                    { $$ = new ReadStmt($2);   }
                  |     READ IDENTIFIER '[' terminal ']' { $$ = new ReadStmt(new Terminal("array",$2,$4)); }

print_statement   :     PRINT   contents  { $2->line = false; $$=$2;}
                  |     PRINTLN contents  { $2->line = true; $$=$2;}

contents          :      content              { $$ = new PrintStmt(); $$->push_back($1); }
                  |      contents ',' content { $$->push_back($3); }

content           :      STRING_LITERAL             { $$ = new Terminal("strlit",$1);   }
                  |      IDENTIFIER                 { $$ = new Terminal("id",$1);       }
                  |      IDENTIFIER '[' terminal ']'{ $$ = new Terminal("array",$1,$3); }


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
  {
    printf("Parsing Done\n");
    Interpreter *it = new Interpreter();
    start->accept(it);
  }
}
