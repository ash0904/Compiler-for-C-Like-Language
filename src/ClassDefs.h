#include <bits/stdc++.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/Verifier.h>
//#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/IRBuilder.h>
//#include <llvm/ModuleProvider.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/raw_ostream.h>

using namespace std;
using namespace llvm;

enum exprType { binary = 1, location = 2, literal = 3, enclExpr = 4 , Unexpr = 5};
enum literalType { Int = 1, Bool = 2, Char = 3, String = 4 };
enum stmtType { Return = 1, NonReturn = 2};
union Node{
  int num;
  char* lit;
  class Prog* prog;
  class DeclList* decls;
  class Decl* decl;
  class Variables* vars;
  class Variable* var;
  class Statement* stmt;
  class StatementList* stmts;
  class Terminal* term;
  class Expr* expr;
  class ArithExpr* arithExpr;
  class AssignExpr* assignExpr;
  class BoolExpr* boolExpr;
  class IfStmt* ifStmt;
  class WhileStmt* whileStmt;
  class GotoStmt* gotoStmt;
  class ForStmt* forStmt;
  class PrintStmt* printStmt;
  class ReadStmt* readStmt;
  Node()
  {
    num = 0;
    lit = NULL;
    prog = NULL;
    decls = NULL;
    decl = NULL;
    vars = NULL;
    var = NULL;
    stmts = NULL;
    term = NULL;
    expr = NULL;
    arithExpr = NULL;
    assignExpr = NULL;
    boolExpr = NULL;
    ifStmt = NULL;
    whileStmt = NULL;
    gotoStmt = NULL;
    forStmt = NULL;
    printStmt = NULL;
    readStmt = NULL;
  }
  ~Node(){}
};
typedef union Node YYSTYPE;

#define YYSTYPE_IS_DECLARED 1

class BaseAst{

};

class Variable:public BaseAst{
private:
  string declType; /* Array or Normal */
  string name; /* Name of the variable */
  string dataType; /* type of variable */
  unsigned int length; /* if it is an Array then length */
  int initial_value;
public:
  /* Constructors */
  Variable(string,string);
  Variable(string,string,unsigned int);
  Variable(string,string,string,int);
  void setDataType(string); /* Set the data Type */
};

class Variables:public BaseAst{
private:
	vector<class Variable*> vars_list;
  int cnt;
public:
	Variables(){}
	void push_back(class Variable*);
  vector<class Variable*> getVarsList();
};

class Decl:public BaseAst{
private:
	string dataType; /* Field declaration can have datatype and vaariables */
	vector<class Variable*> var_list;
public:
  Decl(string);
	Decl(string,class Variables*);
	vector<class Variable*> getVarsList();
};

class DeclList:public BaseAst{
private:
	vector<class Decl*> decl_list;
	int cnt;
public:
	DeclList();
	void push_back(class Decl*);
};

class Statement:public BaseAst{
private:
  string label;
public:
  void setLabel(string label);
};

class StatementList:public BaseAst{
private:
	vector<class Statement*> stmt_list;
public:
	StatementList(){}
  void push_back(class Statement*);
	void push_back(class Statement*,string label);
};

class Terminal:public BaseAst{
private:
  string type,name;
  int value;
  class Terminal* arr_pos;
public:
  Terminal(string,string);
  Terminal(string,int);
  Terminal(string,string,class Terminal*);
};

class Expr:public BaseAst{
private:
  class Terminal* term;
  class ArithExpr* arith_expr;
public:
  Expr(){};
  Expr(class Terminal*);
  Expr(class ArithExpr*);
};

class ArithExpr:public BaseAst{
private:
  class Expr* expr1;
  class Expr* expr2;
  string oper;
public:
  ArithExpr(class Expr*,string,class Expr*);
};

class AssignExpr:public Statement{
private:
  class Terminal* array_val;
  class Expr* rhs;
  string lhs;
  bool arr;
public:
  AssignExpr(string,class Expr*);
  AssignExpr(string,class Expr*,class Terminal*);
};


class BoolExpr:public BaseAst{
private:
  class Expr  *expr1,*expr2;
  class BoolExpr *bexpr1,*bexpr2;
  string oper;
public:
  BoolExpr(class Expr*,string,class Expr*);
  BoolExpr(class BoolExpr*,string,class BoolExpr*);
};

class IfStmt:public Statement{
private:
  string type;
  class BoolExpr* cond;
  class StatementList *if_part, *else_part;
public:
  IfStmt(string,class BoolExpr*,class StatementList*);
  IfStmt(string,class BoolExpr*,class StatementList*,class StatementList*);
};

class WhileStmt:public Statement{
private:
  class BoolExpr* cond;
  class StatementList *stmts;
public:
  WhileStmt(class BoolExpr*,class StatementList*);
};

class GotoStmt:public Statement{
private:
  string type,label;
  class BoolExpr* cond;
public:
  GotoStmt(string,string);
  GotoStmt(string,string,class BoolExpr*);
};

class ForStmt:public Statement{
private:
  class AssignExpr* intial;
  class Terminal *end_cond, * inc_value;
  class StatementList* stmts;
public:
  ForStmt(class AssignExpr*,class Terminal*,class StatementList*);
  ForStmt(class AssignExpr*,class Terminal*,class Terminal*,class StatementList*);
};

class ReadStmt:public Statement{
private:
  class Terminal* obj;
public:
  ReadStmt(class Terminal*);
};

class PrintStmt:public Statement{
private:
  vector< class Terminal* > outs;
public:
  bool line;
  PrintStmt(){}
  void push_back(class Terminal*);
};


class Prog:public BaseAst{
private:
  class DeclList* decls;
	class StatementList* stmt;
public:
	Prog(class DeclList*,class StatementList*);
};
