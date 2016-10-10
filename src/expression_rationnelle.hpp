#ifndef ExpressionRationnelle
#define ExpressionRationnelle
#include <iostream>
// string est la classe des chaînes de caractères en C++ que l'on privilégie aux char * du C.
#include <string>
// set est la classe des ensembles de la STL du C++. set garantit l'unicité et
// le tri lors du parcours : le nombre et l'ordre des insertions ne compte pas.
// L'accès direct avec [] n'est pas possible : l'utilisation des itérateurs est obligatoire.
#include <set>
// vector est la classe des tableaux dynamiques de la STL du C++. vector ne 
// garantit PAS l'unicité et le tri lors du parcours : le nombre et l'ordre
// des insertions compte. L'accès direct avec [] est possible.
#include <vector>
// map est la classe des tableaux associatifs de la STL du C++.
#include <map>

using namespace std;

// Identificateurs pour les opérateurs des expressions régulières
// ce sont les symboles associés aux noeuds de l'arbre de syntaxe des regexs
enum eOperateur {
  o_variable,
  o_ou,
  o_concat,
  o_etoile
};

// Structure des noeuds de l'arbre de syntaxe abstraite des expressions régulières
struct sExpressionRationnelle_p {
  // A chaque noeud est associé un opérateur
  eOperateur op;
  union {
    // Pour le cas o_variable, on associe une chaîne pour identifier la proposition
    string * nom;
    // Pour le cas du connecteur unaire o_etoile, on associe un seul sous-arbre arg.
    sExpressionRationnelle_p* arg;
    // Pour les cas des connecteurs binaires (o_ou et o_concat),
    // on associe deux sous-arbres : le fils gauche arg1 et le fils droit arg2.
    struct {
      sExpressionRationnelle_p * arg1;
      sExpressionRationnelle_p * arg2;
    };
  };
};

typedef struct sExpressionRationnelle_p* sExpressionRationnelle;

// Création d'ER simples ////////////////////////////////////////////////
sExpressionRationnelle variable(string nom);
sExpressionRationnelle variable(char* nom);
sExpressionRationnelle ou(sExpressionRationnelle arg1, sExpressionRationnelle arg2);
sExpressionRationnelle concat(sExpressionRationnelle arg1, sExpressionRationnelle arg2);
sExpressionRationnelle etoile(sExpressionRationnelle arg);

// Affichage d'une ER par induction sur son arbre de syntaxe //////////////
string expression_rationnelle2string(sExpressionRationnelle r);
ostream& operator<<(ostream& out, sExpressionRationnelle r);

// Gestion de la mémoire dynamique des ER ////////////////////////////////
/*
sExpressionRationnelle copier(sExpressionRationnelle r);
*/
void free_all(sExpressionRationnelle r);

/*
// Type des variables
typedef unsigned int  var_t;
// Type des littéraux 
typedef int           lit_t;
// Type des clauses, définies comme des ensembles de littéraux
typedef set<lit_t>    cls_t;
// Type des formules normales conjonctives (CNF), définies comme des tableaux
// dynamiques (vector en C++) de clauses
typedef vector<cls_t> cnf_t;

////////////////////////////////////////////////////////////////////////////////
//        A PARTIR D'ICI LES FONCTIONS SONT SIMPLEMENT DÉCLARÉES ET           //
//               DOIVENT ÊTRE IMPLÉMENTÉES DANS FORMULE.CPP                   //
////////////////////////////////////////////////////////////////////////////////
ostream& operator<<(ostream& out, const cls_t& cl);

ostream& operator<<(ostream& out, const cnf_t& c);

bool est_dans(const cls_t& cl, const cnf_t& c);

void numerote(formule f, map<string,var_t> & corresp);

lit_t var2lit(var_t v, bool p = true);
lit_t neg(lit_t l);
*/

#endif
