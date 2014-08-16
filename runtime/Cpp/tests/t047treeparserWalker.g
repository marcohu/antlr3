tree grammar t047treeparserWalker;
options {
    language=Cpp;
    tokenVocab=t047treeparser;
    //ASTLabelType=CommonTree;
}

@includes {
#include "UserTestTraits.hpp"
#include "t047treeparserParser.hpp"
}
@namespace
{ Antlr3Test }

program
    :   declaration+
    ;

declaration
    :   variable
    |   ^(FUNC_DECL functionHeader)
    |   ^(FUNC_DEF functionHeader block)
    ;

variable returns [t047treeparserWalker::StringType res]
    :   ^(VAR_DEF type declarator)
        { 
            $res = $declarator.text; 
        }
    ;

declarator
    :   ID 
    ;

functionHeader
    :   ^(FUNC_HDR type ID formalParameter+)
    ;

formalParameter
    :   ^(ARG_DEF type declarator)
    ;

type
    :   'int'
    |   'char'
    |   'void'
    |   ID        
    ;

block
    :   ^(BLOCK variable* stat*)
    ;

stat: forStat
    | expr
    | block
    ;

forStat
    :   ^('for' expr expr expr block)
    ;

expr:   ^(T_EQEQ expr expr)
    |   ^(T_LT expr expr)
    |   ^(T_PLUS expr expr)
    |   ^(T_EQ ID expr)
    |   atom
    ;

atom
    : ID      
    | INT      
    ; 
