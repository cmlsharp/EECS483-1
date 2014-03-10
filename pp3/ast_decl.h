/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp3: You will need to extend the Decl classes to implement 
 * semantic processing including detection of declaration conflicts 
 * and managing scoping issues.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "ast_type.h"
#include "list.h"
#include <unordered_map>
#include <vector>
#include <utility>

using namespace std;


class Identifier;
class Stmt;

class Decl : public Node 
{
  protected:
    Identifier *id;
  
  public:
    virtual Type* GetType() = 0;
    Decl(Identifier *name);
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    Type* GetType() { return type; }
    VarDecl(Identifier *name, Type *type);
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;
    Type* type;
    
  public:
    Type* GetType() { return type; }
    ClassDecl(Identifier *name, NamedType *extends, 
              List<NamedType*> *implements, List<Decl*> *members);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    Type* type;
  public:
    Type* GetType() { return type; }
    InterfaceDecl(Identifier *name, List<Decl*> *members);
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    
  public:
    Type* GetType() { return returnType; }
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
};


extern unordered_map<int, vector<Decl*> >variablesInScope; //map scope level to variables in scope

#endif