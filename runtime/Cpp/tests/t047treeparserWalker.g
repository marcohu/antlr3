tree grammar t047treeparserWalker;
options {
    language=Cpp;
    tokenVocab=t047treeparser;
    //ASTLabelType=CommonTree;
	//output = AST;
    filter=true;
    backtrack=true;
}

@includes {
#include "UserTestTraits.hpp"
#include "t047treeparserParser.hpp"
}
@namespace
{ Antlr3Test }

declarator
    :   ID 
    ;

type
    :   'int'
    |   'char'
    |   'void'
    |   ID        
    ;

topdown
    :   enterBlock
    |   varDeclaration
    ;

bottomup
    :   exitBlock
    |   varDeclaration        
    ;

// D e f i n e  s y m b o l s

// START: var
varDeclaration // global, parameter, or local variable
    :   ^(VAR_DEF type declarator)
        {
            std::cout << "def " << $declarator.text << std::endl;
        }
    ;
// END: var

// START: block
enterBlock
    :   BLOCK { /*currentScope = new LocalScope(currentScope); */ }// push scope
    ;
exitBlock
    :   BLOCK
        {
                    //System.out.println("locals: "+currentScope);
                    //currentScope = currentScope.getEnclosingScope();    // pop scope
        }
    ;
// END: block
