%{
#include <bits/stdc++.h>
#include "ClassDefs.h"
#include "common.h"
#include "parser.tab.h"
#define YY_DECL extern "C" int yylex()
	extern union Node yylval;
	using namespace std;
	int line_num = 0;

void yyerror (char const *s)
{
			fl=1;
      fprintf (stderr, "%s at statement -> %d\nNot parsed completely\n", s,yylineno);
}
%}
%option yylineno

IDENTIFIER  		[a-zA-Z][a-zA-Z0-9]*
NUMBER					[0-9][0-9]*
OB							"["
CB							"]"
STRING					\"(\\.|[^\\"])*\"
COLON						":"
%%

"declblock"				return DECL_BLOCK;
"codeblock"				return CODE_BLOCK;

"if"							{
										printf("IF condition : %s\n", yytext);
										return IF;
									}

"goto"						{
										printf("GOTO condition : %s\n", yytext);
										return GOTO;
									}
"for"							{
										printf("FOR condition : %s\n", yytext);
										return FOR;
									}

"while"						{
										printf("WHILE condition : %s\n", yytext);
										return WHILE;
									}
"else"						return ELSE;
"break"						return BREAK;
"continue"				return CONTINUE;
"return"					return RETURN;
{STRING}					{
										printf("string\n");
										return STRING_LITERAL;
									}
"print"						{
										printf("print\n");
										return PRINT;
									}
"read"						return READ;

"int"             {
										yylval.lit = strdup(yytext);
										printf("Token type: INT, Lexeme/Token Value: %s\n", yytext);
										return TYPE;
									}
"["								return '[';
"]"								return ']';
":"								return ':';
","								return ',';
";"								{ yylineno ++; return ';'; }
"+"								return '+';
"-"								return '-';
"*"								return '*';
"{"								return '{';
"}"								return '}';
"=="							return EQUAL_EQUAL;
"<="							return LT_EQUAL;
">="							return GT_EQUAL;
"!="							return NOT_EQUAL;
"="								{printf("Equal to\n");return '=';}
">"								return '>';
"<"								return '<';
"!"								return '!';
"||"							return OR;
"&&"							return AND;


{IDENTIFIER}			{
												printf("Token type: IDENTIFIER, Lexeme/Token Value: %s\n", yytext);
												return IDENTIFIER;
                	}


{NUMBER}					{
												printf("Token type: Number, Lexeme/Token Value: %s\n", yytext);
												return NUMBER;
                	}
{IDENTIFIER}{COLON} 			{
												printf("Token type: LABEL, Lexeme/Token Value: %s\n", yytext);
												return LABEL;
		     					}

[ \t]							{ /* Do nothing */ }
.									{
											fl = 1;
		  								printf("Unexpected token encountered: %s\n", yytext);
		  								return ETOK;
									}
