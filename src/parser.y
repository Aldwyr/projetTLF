%{
#include "parser_defs.hpp"
%}
%skeleton "lalr1.cc"
%{
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
 extern void set_yy_buffer(istream& input);
%}
%union{
  sExpressionRationnelle expression_rationnelle;
 };
%{ /* A DECLARER ABSOLUMENT APRES L'UNION */
YY_DECL;
sExpressionRationnelle lue;
%}

%token <expression_rationnelle> ID 
%token UNION CONCATENATION KLEENE LPAR RPAR END
%type <expression_rationnelle> EXPRESSIONRATIONNELLE R_OU R_CONCAT R_ETOILE SIMPLE START

%start EXPRESSIONRATIONNELLE
%%
EXPRESSIONRATIONNELLE :
START END {$$ = $1; lue = $$; YYACCEPT;}
;
START :
R_OU {$$ = $1;}
;
R_OU :
  R_CONCAT {$$ = $1;}
| R_CONCAT UNION R_OU {$$ = ou($1,$3);}
;
R_CONCAT :
  R_ETOILE {$$ = $1;}
| R_ETOILE CONCATENATION R_CONCAT {$$ = concat($1,$3);}
;
R_ETOILE :
  SIMPLE {$$ = $1;}
| R_ETOILE KLEENE {$$ = etoile($1);}
;
SIMPLE :
ID {$$=$1;}
| LPAR START RPAR {$$=$2;}
;
%%
void yy::parser::error(yy::location const& loc, std::string const& s){
  cerr<<endl<<s<<endl;
}

sExpressionRationnelle lit_expression_rationnelle() {
  yy::parser* pparser = new yy::parser();
  /* pparser->set_debug_level(1); */ 
  int i = pparser->parse();
  sExpressionRationnelle expression_rationnelle;
  if (i==0) {
    expression_rationnelle=lue;
  } else {
    expression_rationnelle=NULL;
  } 
  return expression_rationnelle;
}

sExpressionRationnelle lit_expression_rationnelle(istream & file) {
  set_yy_buffer(file);
  return lit_expression_rationnelle();
}

sExpressionRationnelle lit_expression_rationnelle(const string & s) {
  istringstream is (s);
  set_yy_buffer(is);
  return lit_expression_rationnelle();
}
