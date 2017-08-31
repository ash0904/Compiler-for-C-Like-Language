%{
#include "parser.tab.h"
%}

IDENTIFIER  		[a-zA-Z][a-zA-Z0-9]*
NUMBER					[0-9][0-9]*
OB							"["
CB							"]"


%%
"decl_block"			return DECL_BLOCK;
"code_block"			return CODE_BLOCK;

"if"							{
										printf("IF condition : %s\n", yytext);
										return IF;
									}
"for"							return FOR;
"while"						{
										printf("WHILE condition : %s\n", yytext);
										return WHILE;
									}
"else"						return ELSE;
"break"						return BREAK;
"continue"				return CONTINUE;
"return"					return RETURN;

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
"="								return '=';
">"								return '>';
"<"								return '<';
"!"								return '!';
"||"							return OR;
"&&"							return AND;


{IDENTIFIER}{OB}{NUMBER}{CB}	{
												printf("Token type: INTEGER ARRAY, Lexeme/Token Value: %s\n", yytext);
												return ARRAY;
                	}

{NUMBER}					{
												printf("Token type: Number, Lexeme/Token Value: %s\n", yytext);
												return NUMBER;
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
