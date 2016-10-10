#include "expression_rationnelle.hpp"

// BEGIN Création d'ER simples //////////////////////////////////////////
sExpressionRationnelle variable(string nom) {
  sExpressionRationnelle res = new sExpressionRationnelle_p;
  res->op = o_variable;
  res->nom = new string(nom);
  return res;
}

sExpressionRationnelle variable(char* nom) {
  sExpressionRationnelle res = new sExpressionRationnelle_p;
  res->op = o_variable;
  res->nom = new string(nom);
  return res;
}

sExpressionRationnelle etoile(sExpressionRationnelle arg) {
  sExpressionRationnelle res = new sExpressionRationnelle_p;
  res->op = o_etoile;
  res->arg = arg;
  return res;
}

sExpressionRationnelle concat(sExpressionRationnelle arg1, sExpressionRationnelle arg2) {
  sExpressionRationnelle res = new sExpressionRationnelle_p;
  res->op = o_concat;
  res->arg1 = arg1;
  res->arg2 = arg2;
  return res;
}

sExpressionRationnelle ou(sExpressionRationnelle arg1, sExpressionRationnelle arg2) {
  sExpressionRationnelle res = new sExpressionRationnelle_p;
  res->op = o_ou;
  res->arg1 = arg1;
  res->arg2 = arg2;
  return res;
}

// END Création d'ER simples ////////////////////////////////////////////


// BEGIN Affichage d'une ER ///////////////////////////////////////////////
// Affichage des opérateurs
string operateur2string(eOperateur op) {
  switch (op) {
  case o_variable:
    return "var";
  case o_etoile:
    return "*";
  case o_concat:
    return ".";
  case o_ou:
    return "|";
  }
  return string();
}

// Affichage d'une ER par induction sur son arbre de syntaxe
string expression_rationnelle2string(sExpressionRationnelle r) {
  switch (r->op) {
  case o_variable: 
    return *(r->nom);
  case o_etoile:
    return expression_rationnelle2string(r->arg) + "*";
  case o_concat:
  case o_ou:
    return "(" + expression_rationnelle2string(r->arg1)
      + " " + operateur2string(r->op) + " "
      + expression_rationnelle2string(r->arg2) + ")";
  }
  return string();
}

// Définition de l'opérateur << pour le type sExpressionRationnelle
ostream& operator<<(ostream& out, sExpressionRationnelle r){
  out << expression_rationnelle2string(r);
  return out;
}
// END Affichage d'une ER /////////////////////////////////////////////////


// BEGIN Gestion de la mémoire dynamique des ER //////////////////////////
/*
formule copier(formule f) {
  switch (f->op) {
  case o_variable:
    return variable(* f->nom);
  case o_non:
    return non(copier(f->arg));
  case o_et:
    return et(copier(f->arg1),copier(f->arg2));
  case o_ou:
    return ou(copier(f->arg1),copier(f->arg2));
  case o_implique:
    return impl(copier(f->arg1),copier(f->arg2));
  case o_equivaut:
    return equiv(copier(f->arg1),copier(f->arg2));
  }
  return nullptr;
}
*/

void free_all(sExpressionRationnelle expression_rationnelle) {
  switch (expression_rationnelle->op) {
  case o_ou:
  case o_concat:
    free_all(expression_rationnelle->arg1);
    free_all(expression_rationnelle->arg2);
    break;
  case o_etoile:
    free_all(expression_rationnelle->arg);
    break;
  case o_variable:
    delete expression_rationnelle->nom;
    break;
  }
  delete expression_rationnelle;
}

// END Gestion de la mémoire dynamique des ER ////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//            A PARTIR D'ICI LES FONCTIONS SONT A IMPLEMENTER                 //
////////////////////////////////////////////////////////////////////////////////
/*
ostream& operator<<(ostream& out, const cls_t& cl){
  // A FAIRE
  // exemple 
  out << "{" << 42 << '}' << endl;
  return out;
}

ostream& operator<<(ostream& out, const cnf_t& c){
  // A FAIRE
  return out;
}


bool est_dans(const cls_t& c, const cnf_t& f){
  // A FAIRE
  return false;
}

void numerote(formule f, map<string,var_t> & corresp){
  // A FAIRE
}

lit_t var2lit(var_t v, bool p){
  // A FAIRE
  return 0;
}

lit_t neg(lit_t l){
  // A FAIRE
  return 0;
}
*/

