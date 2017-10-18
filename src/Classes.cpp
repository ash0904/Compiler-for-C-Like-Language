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

Prog::Prog(class DeclList* decls){
  this->fields = decls;
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

void Variable::setDataType(string datatype){
  this->dataType = datatype;
}
