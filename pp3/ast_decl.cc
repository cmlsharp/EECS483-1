/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"

using namespace std;
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    scope = new Slevel; 
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

void Decl::addLevel(Slevel *parent){
	scopd->Parent = parent; 
}	


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}

VarDecl::Check(){
	// if(type->IsPrimitive()){ // Only need to check for ADT's 
// 		return;   
// 	}
	
	Slevel *tempS = scope; 
	while(tempS != NULL){
		Decl *tempD; 
		tempD = tempS->stable->LookUp(type->fetchKey()); 
		if(tempD != NULL){
			ClassDecl *tempC = dynamic_cast<Classdecl*>(tempD); 
			InterfaceDecl *tempI = dynamic_cast<InterfaceDecl*>(tempD); 
			if(tempC == NULL && tempI == NULL){
				type->ReportNotDeclaredIdentifier(LookingForType); 
				return; 
			}
		}	
		tempS= tempS->Parent; 
	}
	//type->ReportNotDeclaredIdentifier(LookingForType); 
}

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    type = new Type(n->name);
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
}

void ClassDecl::addLevel(Slevel *parent){
	scope->Parent = parent; 
	scope->cDecl = this; 
	
	int numElem = members->NumElements(); 
	
	for(int i = 0; i<numElem; i++){
		scope->add(members->Nth(i)); 
	}

	for(int i = 0; i<numElem; i++){
		members->Nth(i)->addLevel(scope); 
	}	
}

void ClassDecl::Check(){
	int numElem = members->numElements(); 
	
	for(int i=0; i<numElem; i++){
		members->Nth(i)->Check(); 
	}
	
	
	if(extends != NULL ){
		Decl *temp = scope->Parent->sTable->LookUp(extends->fetchKey());
		ClassDecl *cDec = dynamic_cast<ClassDecl*>(temp); 
		if(cDec == NULL){
			extends->ReportNotDeclaredIdentifier(LookingForClass); 
		}
	}
	
	if(implements->NumElements() != 0){
		for(int i=0; i<implements->NumElements(); i++){
			Decl *temp = scope->Parent->sTable->LookUp(implements->Nth(i)->fetchKey());
			InterfaceDecl *itemp = dynamic_cast<InterfaceDecl*>(temp); 
			
			if(itemp == NULL){
				implements->Nth(i)->ReportNotDeclaredIdentifier(LookingForInterface);
			}
		}
	}
	//TODO
	//extended members
	//implemented members 
	// interface implementation 
	
	

	return;
}
InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    type = new Type(n->name);
    (members=m)->SetParentAll(this);
}

void InterfaceDecl::addLevel(Slevel *parent){
	scope->Parent = parent; 
	
	int numElem = members->NumElements(); 
	
	for(int i=0; i<numElem; i++){
		scope->add(members->Nth(i)); 
	}
	
	for(int i=0; i<numElem; i++){
		members->Nth(i)->addLevel(scope); 
	}
}
	
void InterfaceDecl::Check(){
	int numElem = members->NumElements(); 
	for(int i=0; i<numElem; i++){
		members->Nth(i)->Check(); 
	}
}

FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}

void FnDecl::addLevel(Slevel *parent){
	scope->Parent=parent; 
	scope->fDecl = this; 
	
	int numElem = formals->NumElements(); 
	for(int i=0; i<numElem; i++){
		scope->add(formals->Nth(i)); 
	}
	
	for(int i=0; i<numElem; i++){
		formals->Nth(i)->addLevel(scope); 
	}
	
	if(body != NULL){
		body->addLevel(scope); 
	}
}

void FnDecl::Check(){
	int numElem = formals->NumElements(); 
	for(int i =0; i<numElem; i++){
		formals->Nth(i)->Check(); 
	}
	
	if(body!= NULL){
		body->Check(); 
	}
}

bool FnDecl::match(Decl *compare){
	FnDecl *temp = dynamic_cast<FnDecl*>(compare); 
	int numElem = formals->NumElements(); 
	
	if(temp == NULL){
		return false; 
	}
	
	int compnumElem = temp->formals->NumElements(); 
	if(numElem != compnumElem || (returnType != temp->returnType)){
		return false; 
	}
	
	for(int i=0; i<numElem(); i++){
		if(!(formals->Nth(i)->type ==(temp->formals->Nth(i)->type))){
			return false; 
     	}
    }
    
    return true; 
}

