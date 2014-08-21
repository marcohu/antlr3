grammar t047treeparser;
options {
    language=Cpp;
    output=AST;
}

tokens {
    VAR_DEF;
    ARG_DEF;
    FUNC_HDR;
    FUNC_DECL;
    FUNC_DEF;
    BLOCK;
}

@lexer::includes
{
#include "UserTestTraits.hpp"
}
@lexer::namespace
{ Antlr3Test }

@parser::includes {
#include "UserTestTraits.hpp"
#include "t047treeparserLexer.hpp"
#include "t047treeparserWalker.hpp"
}
@parser::namespace
{ Antlr3Test }

program
    :   declaration+
    ;

declaration
    :   variable
    |   functionHeader ';' -> ^(FUNC_DECL functionHeader)
    |   functionHeader block -> ^(FUNC_DEF functionHeader block)
    ;

variable
    :   type declarator ';' -> ^(VAR_DEF type declarator)
    ;

declarator
    :   ID 
    ;

functionHeader
    :   type ID '(' ( formalParameter ( ',' formalParameter )* )? ')'
        -> ^(FUNC_HDR type ID formalParameter+)
    ;

formalParameter
    :   type declarator -> ^(ARG_DEF type declarator)
    ;

type
    :   'int'   
    |   'char'  
    |   'void'
    |   ID        
    ;

block
    :   lc='{'
            variable*
            stat*
        '}'
        -> ^(BLOCK[$lc,"BLOCK"] variable* stat*)
    ;

stat: forStat
    | expr ';'!
    | block
    | assignStat ';'!
    | ';'!
    ;

forStat
    :   'for' '(' start=assignStat ';' expr ';' next=assignStat ')' block
        -> ^('for' $start expr $next block)
    ;

assignStat
    :   ID T_EQ expr -> ^(T_EQ ID expr)
    ;

expr:   condExpr
    ;

condExpr
    :   aexpr ( ('=='^ | '<'^) aexpr )?
    ;

aexpr
    :   atom ( '+'^ atom )*
    ;

atom
    : ID      
    | INT      
    | '(' expr ')' -> expr
    ; 

KW_FOR : 'for' ;
KW_INT_TYPE : 'int' ;
KW_CHAR: 'char';
KW_VOID: 'void';

ID  :   ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

INT :	('0'..'9')+
    ;

T_EQ   : '=' ;
T_EQEQ : '==' ;
T_LT   : '<' ;
T_PLUS : '+' ;

WS  :   (   ' '
        |   '\t'
        |   '\r'
        |   '\n'
        )+
        { $channel=HIDDEN; }
    ;    
