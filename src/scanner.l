%{
#include "parser.tab.h"
%}

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
										printf("Token type: INT, Lexeme/Token Value: %s\n", yytext);
										return TYPE;
									}
","								return ',';
";"								return ';';
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


{IDENTIFIER}{OB}{NUMBER}{CB}	{
												printf("Token type: INTEGER ARRAY, Lexeme/Token Value: %s\n", yytext);
												return IDENTIFIER;
                	}

{IDENTIFIER}{OB}{IDENTIFIER}{CB}	{
												printf("Token type: ACCESSING ARRAY, Lexeme/Token Value: %s\n", yytext);
												return ARRAY_VALUE;
                	}

{NUMBER}					{
												printf("Token type: Number, Lexeme/Token Value: %s\n", yytext);
												return NUMBER;
                	}
{IDENTIFIER}{COLON} 			{
												printf("Token type: LABEL, Lexeme/Token Value: %s\n", yytext);
												return LABEL;
		     					}
{IDENTIFIER} 			{
													printf("Token type: Identifier, Lexeme/Token Value: %s\n", yytext);
													return IDENTIFIER;
		     					}

[ \t]							{ /* Do nothing */ }
.									{
		  								printf("Unexpected token encountered: %s\n", yytext);
		  								return ETOK;
									}
