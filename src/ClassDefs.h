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

class Visitor{
public:
  virtual int visit(class Prog* e) {}
  virtual int visit(class DeclList* e) {}
  virtual int visit(class Variables* e) {}
  virtual int visit(class Variable* e) {}
  // virtual int visit(class Statement* e) {}
  virtual int visit(class StatementList* e) {}
  virtual int visit(class Terminal* e) {}
  virtual int visit(class Expr* e) {}
  virtual int visit(class ArithExpr* e) {}
  virtual int visit(class AssignExpr* e) {}
  virtual int visit(class BoolExpr* e) {}
  virtual int visit(class IfStmt* e) {}
  virtual int visit(class WhileStmt* e) {}
  virtual int visit(class GotoStmt* e) {}
  virtual int visit(class ForStmt* e) {}
  virtual int visit(class PrintStmt* e) {}
  virtual int visit(class ReadStmt* e) {}
};

class Interpreter:public Visitor{
public:
  Interpreter(){}
  int visit(class Prog* e);
  int visit(class DeclList* e);
  int visit(class Variables* e);
  int visit(class Variable* e);
  // int visit(class Statement* e);
  int visit(class StatementList* e);
  int visit(class Terminal* e);
  int visit(class Expr* e);
  int visit(class ArithExpr* e);
  int visit(class AssignExpr* e);
  int visit(class BoolExpr* e);
  int visit(class IfStmt* e);
  int visit(class WhileStmt* e);
  int visit(class GotoStmt* e);
  int visit(class ForStmt* e);
  int visit(class PrintStmt* e);
  int visit(class ReadStmt* e);
};

class BaseAst{
public:
  virtual int accept(Visitor* v){};
};

class Variable:public BaseAst{
private:
public:
  string declType;
  string name;
  string dataType;
  unsigned int length;
  int initial_value;
  Variable(string,string);
  Variable(string,string,unsigned int);
  Variable(string,string,string,int);
  void setDataType(string);
  int accept(Visitor* v){ return v->visit(this); }
};

class Variables:public BaseAst{
private:
public:
  vector<class Variable*> vars_list;
  int cnt;
  string dataType;
	Variables(){}
	void push_back(class Variable*);
  vector<class Variable*> getVarsList();
  int accept(Visitor* v){ return v->visit(this); }
};

class DeclList:public BaseAst{
private:
public:
  vector<class Variables*> decl_list;
  int cnt;
	DeclList(){}
	void push_back(class Variables*);
  vector<class Variables*> getDeclList();
  int accept(Visitor* v){ return v->visit(this); }
};

class Statement:public BaseAst{
private:
public:
  string label;
  void setLabel(string label);
  // int accept(Visitor* v){ return v->visit(this); }
};

class StatementList:public BaseAst{
private:
public:
  vector<class Statement*> stmt_list;
	StatementList(){}
  void push_back(class Statement*);
	void push_back(class Statement*,string label);
  int accept(Visitor* v){ return v->visit(this); }
};

class Terminal:public BaseAst{
private:
public:
  string type,name;
  int value;
  class Terminal* arr_pos;
  Terminal(string,string);
  Terminal(string,int);
  Terminal(string,string,class Terminal*);
  int accept(Visitor* v){ return v->visit(this); }
};

class Expr:public BaseAst{
private:
public:
  class Terminal* term;
  class ArithExpr* arith_expr;
  Expr(){};
  Expr(class Terminal*);
  Expr(class ArithExpr*);
  int accept(Visitor* v){ return v->visit(this); }
};

class ArithExpr:public BaseAst{
private:
public:
  class Expr* expr1;
  class Expr* expr2;
  string oper;
  ArithExpr(class Expr*,string,class Expr*);
  int accept(Visitor* v){ return v->visit(this); }
};

class AssignExpr:public Statement{
private:
public:
  class Terminal* array_val;
  class Expr* rhs;
  string lhs;
  bool arr;
  AssignExpr(string,class Expr*);
  AssignExpr(string,class Expr*,class Terminal*);
  int accept(Visitor* v){ return v->visit(this); }
};


class BoolExpr:public BaseAst{
private:
public:
  class Expr  *expr1,*expr2;
  class BoolExpr *bexpr1,*bexpr2;
  string oper;
  BoolExpr(class Expr*,string,class Expr*);
  BoolExpr(class BoolExpr*,string,class BoolExpr*);
  int accept(Visitor* v){ return v->visit(this); }
};

class IfStmt:public Statement{
private:
public:
  string type;
  class BoolExpr* cond;
  class StatementList *if_part, *else_part;
  IfStmt(string,class BoolExpr*,class StatementList*);
  IfStmt(string,class BoolExpr*,class StatementList*,class StatementList*);
  int accept(Visitor* v){ return v->visit(this); }
};

class WhileStmt:public Statement{
private:
public:
  class BoolExpr* cond;
  class StatementList *stmts;
  WhileStmt(class BoolExpr*,class StatementList*);
  int accept(Visitor* v){ return v->visit(this); }
};

class GotoStmt:public Statement{
private:
  string type,label;
  class BoolExpr* cond;
public:
  GotoStmt(string,string);
  GotoStmt(string,string,class BoolExpr*);
  int accept(Visitor* v){ return v->visit(this); }
};

class ForStmt:public Statement{
private:
public:
  class AssignExpr* initial;
  class Terminal *end_cond, *inc_value;
  class StatementList* stmts;
  ForStmt(class AssignExpr*,class Terminal*,class StatementList*);
  ForStmt(class AssignExpr*,class Terminal*,class Terminal*,class StatementList*);
  int accept(Visitor* v){ return v->visit(this); }
};

class ReadStmt:public Statement{
private:
public:
  class Terminal* obj;
  ReadStmt(class Terminal*);
  int accept(Visitor* v){ return v->visit(this); }
};

class PrintStmt:public Statement{
private:
public:
  vector< class Terminal* > outs;
  bool line;
  PrintStmt(){}
  void push_back(class Terminal*);
  int accept(Visitor* v){ return v->visit(this); }
};


class Prog:public BaseAst{
private:
public:
  class DeclList* decls;
  class StatementList* stmt;
	Prog(class DeclList*,class StatementList*);
  int accept(Visitor* v){ return v->visit(this); }
};
