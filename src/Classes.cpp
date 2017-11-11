#include <bits/stdc++.h>

using namespace std;
#include "ClassDefs.h"

using namespace llvm;

/* Usefull Variables */
static Module *TheModule = new Module("Compiler for flatB language",llvm::getGlobalContext());
static LLVMContext &Context = getGlobalContext();
static IRBuilder<> Builder(Context);
// static std::map<std::string, llvm::AllocaInst *> NamedValues;
// static FunctionPassManager *TheFPM;

/* Get the function type and create a Function */
  FunctionType *FT = llvm::FunctionType::get(Builder.getVoidTy(),false);
  Function *F = llvm::Function::Create(FT, Function::ExternalLinkage, "CodeBlock", TheModule);

/* defined by me for Interpreter */
map < string, int > stable;
map < string, int> ltable;

/* for IR */
map <string, BasicBlock* > lab_table;

/* Usefull Functions */
// static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const std::string &VarName, string type) {
//   /* Allocates memory for local variables  on the stack of the function */
//
//   /* Get the builder for current context */
//   IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
//   AllocaInst* Alloca;
//   if(type == "int"){
//     Alloca = TmpB.CreateAlloca(Type::getInt32Ty(getGlobalContext()), 0, VarName.c_str());
//   }
//   else if (type == "boolean"){
//     Alloca = TmpB.CreateAlloca(Type::getInt1Ty(getGlobalContext()), 0, VarName.c_str());
//   }
//   return Alloca;
// }

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
  stmt->setLabel("NULL");
  this->cnt++;
}

void StatementList::push_back(class Statement* stmt,string label){
  if(label=="NULL")
  {
    printf("Error! cannot name the Label as NULL\n");
    exit(1);
  }
  stmt->setLabel(label);
  stmt_list.push_back(stmt);
  ltable[label] = cnt;
  this->cnt++;
}

void PrintStmt::push_back(class Terminal* cont){
  outs.push_back(cont);
}

string GotoStmt::getLabel(){
  return this->label;
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

string getLabel(class GotoStmt* obj)
{
  return obj->label;
}
int Interpreter::visit(class StatementList* obj){
  Interpreter *it = new Interpreter();
  int num=0,i=0;
  for(i=0;i<obj->cnt;i++)
  {
    num = obj->stmt_list[i]->accept(it);
    if(num==-10)
    {
      string label = obj->stmt_list[i]->getLabel();
      i = ltable[label]-1;
    }
  }
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
  if(obj->oper == "%")
    return obj->expr1->accept(it) % obj->expr2->accept(it);
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
  else
  {
    int cur = obj->array_val->accept(it);
    if(stable.find(to_string(cur)+obj->lhs)==stable.end())
    {
      cout<<"Error: variable "<<obj->lhs<<"["<<cur<<"] not defined\n";
      return -1;
    }
    else
    {
      stable[to_string(cur)+obj->lhs] = obj->rhs->accept(it);
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
  if(obj->type=="single")
    return -10;
  if(obj->cond->accept(it))
    return -10;
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
  if(obj->obj->type=="array")
    stable[to_string(obj->obj->accept(it)) + obj->obj->name] = ans;
  else
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
    return stable[to_string(obj->arr_pos->accept(it))+obj->name];
  if(obj->type=="strlit")
    cout<<obj->name.substr(1,obj->name.length()-2);
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

/* ---------------------------- Code Generation ---------------- */

Value* Prog::codegen(){
  cout << "Generating LLVM IR Code\n";
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  V = decls->codegen();

  /* Create a New block for this Function */
  BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", F);
  Builder.SetInsertPoint(BB);
  V = stmt->codegen();
  return V;
}

void Prog::generateCode(){
  cout << "Displaying the generated LLVM IR Code\n";
  TheModule->dump();
}

Value* DeclList::codegen(){
  for(int i = 0; i < cnt; i++){
    decl_list[i]->codegen();
  }
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  return V;
}

Value* Variables::codegen(){
  class Variable* var;
  llvm::Type *ty;
  for(int i = 0; i < cnt; i++){
    var = vars_list[i];
    if(dataType == "int"){
      ty = Type::getInt32Ty(Context);
    }
    PointerType* ptrTy = PointerType::get(ty,0);
    GlobalVariable* gv = new GlobalVariable(*TheModule,ptrTy , false,GlobalValue::ExternalLinkage, 0, var->name);
  }
  Value* v = ConstantInt::get(getGlobalContext(), APInt(32,1));
  return v;
}

Value* StatementList::codegen(){
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  for(int i = 0; i < cnt; i++){
    if(stmt_list[i]->label!="NULL"){
      BasicBlock *newLabel = BasicBlock::Create(getGlobalContext(),stmt_list[i]->label,F);
      Builder.SetInsertPoint(newLabel);
      lab_table[stmt_list[i]->label] = newLabel;
    }
    stmt_list[i]->codegen();
  }
  return V;
}

Value* Terminal::codegen(){
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  if(type == "id")
  {
    Value* v = TheModule->getNamedGlobal(name);
    v = Builder.CreateLoad(v);
    return v;
  }
  else if(type == "num")
  {
    Value *v = ConstantInt::get(getGlobalContext(), APInt(32,value));
    return v;
  }
}

Value* AssignExpr::codegen(){
  Value* v = TheModule->getNamedGlobal(lhs);
  Value* rhs_eval = rhs->codegen();
  return Builder.CreateStore(rhs_eval,v);
}

Value* Expr::codegen(){
  if(!term)
    return arith_expr->codegen();
  else
    return term->codegen();

}

Value* ArithExpr::codegen(){
  Value *v1 = expr1->codegen();
  Value *v2 = expr2->codegen(),*v;

  if(oper=="+")
    v = Builder.CreateAdd(v1,v2,"addtmp");
  if(oper=="-")
    v = Builder.CreateSub(v1,v2,"subtmp");
  if(oper=="*")
    v = Builder.CreateMul(v1,v2,"multmp");
  if(oper=="/")
    v = Builder.CreateUDiv(v1,v2,"divtmp");
  if(oper=="%")
    v = Builder.CreateURem(v1,v2,"modtmp");
  return v;
}

Value* IfStmt::codegen(){
  Value *cond_check = cond->codegen();
  BasicBlock* if_block   = BasicBlock::Create(Context,"if_part",F);
  BasicBlock* else_block = BasicBlock::Create(Context,"else_part",F);
  BasicBlock* after_part = BasicBlock::Create(Context,"after_ifelse",F);

  Builder.CreateCondBr(cond_check,if_block,else_block);

  Builder.SetInsertPoint(if_block);
  Value *v = if_part->codegen();

  Builder.CreateBr(after_part);

  F->getBasicBlockList().push_back(else_block);
  Builder.SetInsertPoint(else_block);

  if(else_part){
    v = else_part->codegen();
  }
  Builder.CreateBr(after_part);

  F->getBasicBlockList().push_back(after_part);
  Builder.SetInsertPoint(after_part);

  return v;
}

Value* BoolExpr::codegen(){
  Value* left  = expr1->codegen();
  Value* right = expr2->codegen();
  Value* v;

  if (oper == "<"){
    v = Builder.CreateICmpULT(left,right,"ltcomparetmp");
  }
  if (oper == ">"){
    v = Builder.CreateICmpUGT(left,right,"gtcomparetmp");
  }
  if (oper == "<="){
    v = Builder.CreateICmpULE(left,right,"lecomparetmp");
  }
  if (oper == ">="){
    v = Builder.CreateICmpUGE(left,right,"gecomparetmp");
  }
  if (oper == "=="){
    v = Builder.CreateICmpEQ(left,right,"equalcomparetmp");
  }
  if (oper == "!="){
    v = Builder.CreateICmpNE(left,right,"notequalcomparetmp");
  }
  return v;
}

Value* ForStmt::codegen(){
  initial->codegen();
  Value* starteval = initial->rhs->codegen();

  Value *ending = end_cond->codegen();
  Value *inc_cond = inc_value->codegen();

  Value *cond_check = Builder.CreateICmpULE(starteval, ending, "loop_cond");
  BasicBlock *start_for = Builder.GetInsertBlock();
  BasicBlock *body = BasicBlock::Create(getGlobalContext(),"forBody",F);
  BasicBlock* afterloop = BasicBlock::Create(getGlobalContext(),"afterloop",F);
//  Builder.CreateBr(body);
  Builder.CreateCondBr(cond_check,body,afterloop);

  Builder.SetInsertPoint(body);

  PHINode *Variable = Builder.CreatePHI(Type::getInt32Ty(llvm::getGlobalContext()), 2, initial->lhs);
  Variable->addIncoming(starteval, start_for);

  stmts->codegen();

  Value *iter_val = TheModule->getNamedGlobal(initial->lhs);
  Value *load_var = Builder.CreateLoad(iter_val);
  Value *inc_var  = Builder.CreateAdd(load_var,inc_cond,"UpdatedIterator");

  cond_check = Builder.CreateICmpULE(inc_var, ending, "loop_cond");
  BasicBlock *endBB = Builder.GetInsertBlock();
  Builder.CreateCondBr(cond_check,body,afterloop);
  Builder.SetInsertPoint(afterloop);
  Variable->addIncoming(inc_var, endBB);

  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  return V;
}

Value* WhileStmt::codegen(){
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));

  BasicBlock *start_while = Builder.GetInsertBlock();
  BasicBlock *body = BasicBlock::Create(getGlobalContext(),"whileBody",F);
  BasicBlock *afterloop = BasicBlock::Create(getGlobalContext(),"afterloop",F);

  Value *cond_check = cond->codegen();
  Builder.CreateCondBr(cond_check,body,afterloop);

  Builder.SetInsertPoint(body);
  stmts->codegen();
  cond_check = cond->codegen();
  Builder.CreateCondBr(cond_check,body,afterloop);

  Builder.SetInsertPoint(afterloop);

  return V;
}

Value* GotoStmt::codegen(){
  BasicBlock *after_goto = BasicBlock::Create(getGlobalContext(),"after_goto",F);
  if(type == "single")
    Builder.CreateBr(lab_table[label]);
  else if(type == "cond")
  {
    Value* cond_check = cond->codegen();
    Builder.CreateCondBr(cond_check,lab_table[label],after_goto);
  }
  Builder.SetInsertPoint(after_goto);

  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  return V;
}

/* ------------ yet to define ------------ */

Value* Variable::codegen(){
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  return V;
}

Value* ReadStmt::codegen(){
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  return V;
}

Value* PrintStmt::codegen(){
  Value *V = ConstantInt::get(getGlobalContext(), APInt(32,0));
  return V;
}
