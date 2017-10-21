#include <bits/stdc++.h>

using namespace std;
#include "ClassDefs.h"

using namespace llvm;

/* Usefull Variables */
static Module *TheModule = new Module("Decaf compiler jit",llvm::getGlobalContext());
static LLVMContext &Context = getGlobalContext();
static IRBuilder<> Builder(Context);
static std::map<std::string, llvm::AllocaInst *> NamedValues;
static FunctionPassManager *TheFPM;

map < string, int > stable;
map < string, class Statement*> ltable;

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
  this->initial = initial;
  this->end_cond = end_cond;
  this->stmts = stmts;
}

ForStmt::ForStmt(class AssignExpr* initial,class Terminal* end_cond,class Terminal* inc_value,class StatementList* stmts){
  this->initial = initial;
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

void DeclList::push_back(class Variables* var){
  decl_list.push_back(var);
  cnt++;
}
vector<class Variables*> DeclList::getDeclList(){
  return decl_list;
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
  outs.push_back(cont);
}

/******* interpreting the AST formed **********/

int Interpreter::visit(class Prog* obj){
  Interpreter *it = new Interpreter();
  obj->decls->accept(it);
  obj->stmt->accept(it);
  return 0;
}

int Interpreter::visit(class DeclList* obj){
  Interpreter *it = new Interpreter();
  for(auto i : obj->decl_list)
    i->accept(it);
  // for(auto i = stable.begin();i!= stable.end();i++)
  // {
  //   cout<<i->first <<" ==> "<<i->second<<endl;
  // }
  return 0;
}

int Interpreter::visit(class Variables* obj){
  Interpreter *it = new Interpreter();
  for(auto i : obj->vars_list)
    i->accept(it);
  return 0;
}

int Interpreter::visit(class Variable* obj){
  if(obj->declType=="simple")
    stable[obj->name]=0;
  if(obj->declType=="initialised")
    stable[obj->name] = obj->initial_value;
  if(obj->declType=="array")
  {
    for(int i=0;i<obj->length;i++)
    {
      stable[to_string(i)+obj->name]=0;
    }
  }
  return 0;
}

// int Interpreter::visit(class Statement* obj){
//   Interpreter *it = new Interpreter();
//   return 0;
// }

int Interpreter::visit(class StatementList* obj){
  Interpreter *it = new Interpreter();
  for(auto i : obj->stmt_list)
    i->accept(it);
  return 0;
}

int Interpreter::visit(class ArithExpr* obj){
  Interpreter *it = new Interpreter();

  if(obj->oper == "+")
    return obj->expr1->accept(it) + obj->expr2->accept(it);
  if(obj->oper == "-")
    return obj->expr1->accept(it) - obj->expr2->accept(it);
  if(obj->oper == "*")
    return obj->expr1->accept(it) * obj->expr2->accept(it);
  if(obj->oper == "/")
    return obj->expr1->accept(it) / obj->expr2->accept(it);
}

int Interpreter::visit(class AssignExpr* obj){
  Interpreter *it = new Interpreter();
  if(!obj->arr)
  {
    if(stable.find(obj->lhs)==stable.end())
    {
      cout<<"Error: variable "<<obj->lhs<<" not defined\n";
      return -1;
    }
    else
    {
      stable[obj->lhs] = obj->rhs->accept(it);
    }
  }
  return 0;
}

int Interpreter::visit(class IfStmt* obj){
  Interpreter *it = new Interpreter();
  if(obj->type=="single")
  {
    if(obj->cond->accept(it))
    {
      obj->if_part->accept(it);
    }
  }
  else
  {
    if(obj->cond->accept(it))
    {
      obj->if_part->accept(it);
    }
    else
      obj->else_part->accept(it);
  }
  return 0;
}

int Interpreter::visit(class WhileStmt* obj){
  Interpreter *it = new Interpreter();
  while(obj->cond->accept(it))
    obj->stmts->accept(it);
  return 0;
}

int Interpreter::visit(class GotoStmt* obj){
  Interpreter *it = new Interpreter();
  return 0;
}

int Interpreter::visit(class ForStmt* obj){
  Interpreter *it = new Interpreter();
  obj->initial->accept(it);
  while(stable[obj->initial->lhs] <= obj->end_cond->accept(it))
  {
    obj->stmts->accept(it);
    if(!obj->inc_value)
      stable[obj->initial->lhs] += 1;
    else
      stable[obj->initial->lhs] += obj->inc_value->accept(it);
  }
  return 0;
}

int Interpreter::visit(class PrintStmt* obj){
  Interpreter *it = new Interpreter();
  int out;
  for(auto i : obj->outs)
  {
    out = i->accept(it);
    if(i->type!="strlit")
      cout<<out;
  }
  if(obj->line)
    cout<<endl;
  return 0;
}

int Interpreter::visit(class ReadStmt* obj){
  Interpreter *it = new Interpreter();
  int ans;
  cin>>ans;
  stable[obj->obj->name] = ans;
  return 0;
}

int Interpreter::visit(class Terminal* obj){
  Interpreter *it = new Interpreter();
  if(obj->type=="id")
    return stable[obj->name];
  if(obj->type == "num")
    return obj->value;
  if(obj->type=="array")
    return stable[obj->name];
  if(obj->type=="strlit")
    cout<<obj->name;
  return 0;
}

int Interpreter::visit(class Expr* obj){
  Interpreter *it = new Interpreter();
  if(!obj->term)
    return obj->arith_expr->accept(it);
  else
    return obj->term->accept(it);
  // return 0;
}

int Interpreter::visit(class BoolExpr* obj){
  Interpreter *it = new Interpreter();
  if(obj->oper=="AND")
    return obj->bexpr1->accept(it) && obj->bexpr2->accept(it);
  else if(obj->oper=="OR")
    return obj->bexpr1->accept(it) || obj->bexpr2->accept(it);
  else
  {
    if(obj->oper== ">")
      return obj->expr1->accept(it) >  obj->expr2->accept(it);
    if(obj->oper== "<")
      return obj->expr1->accept(it) <  obj->expr2->accept(it);
    if(obj->oper==">=")
      return obj->expr1->accept(it) >= obj->expr2->accept(it);
    if(obj->oper=="<=")
      return obj->expr1->accept(it) <= obj->expr2->accept(it);
    if(obj->oper=="!=")
      return obj->expr1->accept(it) != obj->expr2->accept(it);
    if(obj->oper=="==")
      return obj->expr1->accept(it) == obj->expr2->accept(it);
  }
  return 1;
}
