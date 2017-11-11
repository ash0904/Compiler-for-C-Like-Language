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
										return IF;
									}

"goto"						{
										return GOTO;
									}
"for"							{
										return FOR;
									}

"while"						{
										return WHILE;
									}
"else"						return ELSE;
"break"						return BREAK;
"continue"				return CONTINUE;
"return"					return RETURN;
{STRING}					{
										yylval.lit = strdup(yytext);
										return STRING_LITERAL;
									}
"print"						{
										return PRINT;
									}
"println"					{
										return PRINTLN;
									}
"read"						return READ;

"int"             {
										yylval.lit = strdup(yytext);
										return TYPE;
									}
"["								{ return '['; }
"]"								return ']';
":"								return ':';
","								return ',';
";"								{ yylineno ++; return ';'; }
"+"								return '+';
"-"								return '-';
"*"								return '*';
"/"								return '/';
"%"								return '%';
"{"								return '{';
"}"								return '}';
"=="							{yylval.lit = strdup(yytext); return EQUAL_EQUAL;}
"<="							{yylval.lit = strdup(yytext); return LT_EQUAL;}
">="							{yylval.lit = strdup(yytext); return GT_EQUAL;}
"!="							{yylval.lit = strdup(yytext); return NOT_EQUAL;}
"="								{return '=';}
">"								{yylval.lit = strdup(yytext); return '>';}
"<"								{yylval.lit = strdup(yytext); return '<';}
"!"								return '!';
"||"							{yylval.lit = strdup(yytext); return OR;}
"&&"							{yylval.lit = strdup(yytext); return AND;}


{IDENTIFIER}			{
												yylval.lit = strdup(yytext);
												return IDENTIFIER;
                	}


{NUMBER}					{
												yylval.num = stoi(yytext);
												return NUMBER;
                	}

[ \t\n]							{ /* Do nothing */ }
.									{
											fl = 1;
		  								return ETOK;
									}
