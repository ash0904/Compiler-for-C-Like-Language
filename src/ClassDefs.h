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
  int val;
  char* lit;
  class Prog* prog;
  class DeclList* decls;
  class Decl* decl;
  class Variables* vars;
  class Variable* var;
  Node()
  {
    val = 0;
    lit = NULL;
    prog = NULL;
    decls = NULL;
    decl = NULL;
    vars = NULL;
    var = NULL;
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
	string dataType;
	vector<class Variable*> var_list;
public:
  Statement(string);
	Statement(string,class Variables*);
	vector<class Variable*> getVarsList();
};

class StatementList:public BaseAst{
private:
	vector<class Statement*> decl_list;
	int cnt;
public:
	StatementList();
	void push_back(class Statement*);
};

class Prog:public BaseAst{
private:
  class DeclList* vars; /* list of fields */
	class StatementList* vars; /* list of fields */
public:
	Prog(class DeclList*);
};
