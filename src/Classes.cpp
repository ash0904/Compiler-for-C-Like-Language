#include <bits/stdc++.h>

#include "ClassDefs.h"

using namespace std;
using namespace llvm;

/* Usefull Variables */
static Module *TheModule = new Module("Decaf compiler jit",llvm::getGlobalContext());
static LLVMContext &Context = getGlobalContext();
static IRBuilder<> Builder(Context);
static std::map<std::string, llvm::AllocaInst *> NamedValues;
static FunctionPassManager *TheFPM;

/* Usefull Functions */

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, string type) {
  /* Allocates memory for local variables  on the stack of the function */

  /* Get the builder for current context */
  IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
  AllocaInst* Alloca;
  if(type == "int"){
    Alloca = TmpB.CreateAlloca(Type::getInt32Ty(getGlobalContext()), 0, VarName.c_str());
  }
  else if (type == "boolean"){
    Alloca = TmpB.CreateAlloca(Type::getInt1Ty(getGlobalContext()), 0, VarName.c_str());
  }
  return Alloca;
}

/* all the definitions of Construtctors */

Variable::Variable(string declType, string name){
  this->declType = declType;
  this->name = name;
  // cout<<"vicky\n";
  // cout<<name<<" "<<declType<<"\n";
}

Variable::Variable(string declType, string name, unsigned int length){
  this->declType = declType;
  this->name = name;
  this->length = length;
}

Variable::Variable(string declType, string name,string flag,int initial_value){
  this->declType = declType;
  this->name = name;
  this->initial_value = initial_value;
  // cout<<"vicky\n";
  // cout<<name<<" "<<declType<<" "<<initial_value<<"\n";
}

Decl::Decl(string dataType){
  this->dataType = dataType;
}

Decl::Decl(string dataType, class Variables* vars){
  this->dataType = dataType;
  this->var_list = vars->getVarsList();
  for(int i = 0; i < var_list.size(); i++){
    var_list[i]->setDataType(dataType);
  }
}

DeclList::DeclList(){
  this->cnt = 0;
}

Terminal::Terminal(string type,string name){
    this->type = type;
    this->name = name;
}

Terminal::Terminal(string type,int value){
    this->type = type;
    this->value = value;
}

Terminal::Terminal(string type,string name,class Terminal* arr_pos){
    this->type = type;
    this->name = name;
    this->arr_pos = arr_pos;
}

Expr::Expr(class Terminal* term){
  this->term = term;
}

Expr::Expr(class ArithExpr* arith_expr){
  this->arith_expr = arith_expr;
}

ArithExpr::ArithExpr(class Expr* expr1,string oper,class Expr* expr2){
  this->expr1 = expr1;
  this->oper  = oper;
  cout<<oper<<endl;
  this->expr2 = expr2;
}

AssignExpr::AssignExpr(string lhs,class Expr* rhs){
  this->lhs = lhs;
  this->rhs = rhs;
  arr = false;
}

AssignExpr::AssignExpr(string lhs,class Expr* rhs,class Terminal* array_val){
  this->lhs = lhs;
  this->rhs = rhs;
  this->array_val = array_val;
  arr = true;
}

BoolExpr::BoolExpr(class Expr* expr1,string oper,class Expr* expr2){
  this->expr1 = expr1;
  this->oper = oper;
  this->expr2 = expr2;
}

BoolExpr::BoolExpr(class BoolExpr* bexpr1,string oper,class BoolExpr* bexpr2){
  this->bexpr1 = bexpr1;
  this->oper = oper;
  this->bexpr2 = bexpr2;
}

IfStmt::IfStmt(string type,class BoolExpr* cond,class StatementList* if_part){
  this->type = type;
  this->cond = cond;
  this->if_part = if_part;
}

IfStmt::IfStmt(string type,class BoolExpr* cond,class StatementList* if_part,class StatementList* else_part){
  this->type = type;
  this->cond = cond;
  this->if_part = if_part;
  this->else_part = else_part;
}

WhileStmt::WhileStmt(class BoolExpr* cond,class StatementList* stmts){
  this->cond = cond;
  this->stmts = stmts;
}


GotoStmt::GotoStmt(string type,string label){
  this->type = type;
  this->label = label;
}

GotoStmt::GotoStmt(string type,string label,class BoolExpr* cond){
  this->type = type;
  this->label = label;
  this->cond = cond;
}

ForStmt::ForStmt(class AssignExpr* initial,class Terminal* end_cond,class StatementList* stmts){
  this->intial = initial;
  this->end_cond = end_cond;
  this->stmts = stmts;
}

ForStmt::ForStmt(class AssignExpr* initial,class Terminal* end_cond,class Terminal* inc_value,class StatementList* stmts){
  this->intial = initial;
  this->end_cond = end_cond;
  this->inc_value = inc_value;
  this->stmts = stmts;
}

ReadStmt::ReadStmt(class Terminal* obj){
  this->obj = obj;
}


Prog::Prog(class DeclList* decls,class StatementList* stmt){
  this->decls = decls;
  this->stmt = stmt;
}


/* various functions of different classes */

void Variable::setDataType(string datatype){
  this->dataType = datatype;
}

void Variables::push_back(class Variable* var){
  vars_list.push_back(var);
  cnt++;
}

vector<class Variable*> Variables::getVarsList(){
  return vars_list;
}

void DeclList::push_back(class Decl* var){
  decl_list.push_back(var);
  cnt++;
}

void Statement::setLabel(string label){
  this->label = label;
}

void StatementList::push_back(class Statement* stmt){
  stmt_list.push_back(stmt);
}

void StatementList::push_back(class Statement* stmt,string label){
  stmt->setLabel(label);
  stmt_list.push_back(stmt);
}

void PrintStmt::push_back(class Terminal* cont){
  // cout<<cont<<endl;
  outs.push_back(cont);
}
