    #include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <list>
#include <cassert>
#include <utility>

// pour la seconde partie du projet
#include "expression_rationnelle.hpp"
#include "parser.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const unsigned int ASCII_A = 97;
const unsigned int ASCII_Z = ASCII_A + 26;
const bool         DEBUG = false;

typedef size_t                            etat_t;
typedef unsigned char                     symb_t;
typedef set< etat_t >                     etatset_t;
typedef vector< vector< etatset_t > >     trans_t;
typedef vector< etatset_t >               epsilon_t;
typedef map< etatset_t, etat_t >          map_t;

////////////////////////////////////////////////////////////////////////////////

struct sAutoNDE{
  // caractéristiques
  size_t nb_etats;
  size_t nb_symbs;
  size_t nb_finaux;

  etat_t initial;
  // état initial

  etatset_t finaux;
  // états finaux : finaux_t peut être un int*, un tableau dynamique comme vector<int>
  // ou une autre structure de donnée de votre choix.

  trans_t trans;
  // matrice de transition : trans_t peut être un int***, une structure dynamique 3D comme vector< vector< set<int> > >
  // ou une autre structure de donnée de votre choix.
  
  epsilon_t epsilon; 
  // transitions spontanées : epsilon_t peut être un int**, une structure dynamique 2D comme vector< set<int> >
  // ou une autre structure de donnée de votre choix.
};

////////////////////////////////////////////////////////////////////////////////

bool FromFile(sAutoNDE& at, string path){
 ifstream myfile(path.c_str(), ios::in); 
  //un flux d'entree obtenu à partir du nom du fichier
  string line;
  // un ligne lue dans le fichier avec getline(myfile,line);
  istringstream iss;
  // flux associé à la chaine, pour lire morceau par morceau avec >> (comme cin)
  etat_t s(0), t(0);
  // deux états temporaires
  symb_t a(0);
  // un symbole temporaire

  if (myfile.is_open()){
    // la première ligne donne 'nb_etats nb_symbs nb_finaux'
    do{ 
      getline(myfile,line);
    } while (line.empty() || line[0]=='#');
    // on autorise les lignes de commentaires : celles qui commencent par '#'
    iss.str(line);
    if((iss >> at.nb_etats).fail() || (iss >> at.nb_symbs).fail() || (iss >> at.nb_finaux).fail())
        return false;
    // la deuxième ligne donne l'état initial
    do{ 
      getline (myfile,line);
    } while (line.empty() || line[0]=='#');    
    iss.clear();
    iss.str(line);
    if((iss >> at.initial).fail())
      return -1;
    
    // les autres lignes donnent les états finaux
    for(size_t i = 0; i < at.nb_finaux; i++){
        do{ 
          getline (myfile,line);
        } while (line.empty() || line[0]=='#');
        iss.clear();
        iss.str(line);
         if((iss >> s).fail())
          continue;
//        cerr << "s= " << s << endl;
        at.finaux.insert(s);
    }     

    // on alloue les vectors à la taille connue à l'avance pour éviter les resize dynamiques
    at.epsilon.resize(at.nb_etats);
    at.trans.resize(at.nb_etats);
    for(size_t i=0;i<at.nb_etats;++i)
      at.trans[i].resize(at.nb_symbs);   

  // lecture de la relation de transition 
    while(myfile.good()){
      line.clear();
      getline (myfile,line);
      if (line.empty() && line[0]=='#')
        continue;
      iss.clear();
      iss.str(line); 

      // si une des trois lectures echoue, on passe à la suite
      if((iss >> s).fail() || (iss >> a).fail() || (iss >> t).fail() || (a< ASCII_A ) || (a> ASCII_Z ))
        continue; 
	
	  if(DEBUG) { cout << s << "-" << a << "->" << t << "\n"; }
              
      //test espilon ou non
      if ((a-ASCII_A) >= at.nb_symbs){
//        cerr << "s=" << s<< ", (e), t=" << t << endl;
		at.epsilon[s].insert(t);
      }
      else{
//        cerr << "s=" << s<< ", a=" << a-ASCII_A << ", t=" << t << endl;
		at.trans[s][a-ASCII_A].insert(t);
      }
    }
    myfile.close();
    return true; 
 }
  return false;
  // on ne peut pas ouvrir le fichier
}


// -----------------------------------------------------------------------------
// Fonctions à compléter pour la première partie du projet
// -----------------------------------------------------------------------------


bool EstDeterministe(const sAutoNDE& at){

  for(auto it_e = at.epsilon.cbegin(); it_e != at.epsilon.cend(); it_e++) {
	  if(DEBUG) cout << it_e->size() << " ";
	  if(it_e->size() > 0) { return false; }
  }
  
  // trans = vector<vector<set<int>>> = [s][a]<t>
  for(auto it = at.trans.cbegin(); it != at.trans.cend(); it++) {
	  // it = itérateur sur vector<set<int>> = état de départ
	  for(auto it2 = it->cbegin(); it2 != it->cend(); it2++) {
		  // it2 = itérateur sur set<int> = symbole de transition
          // pour chaque état, il faut que chaque symbole donne exactement 1 état d'arrivée
		  if(it2->size() != 1) { return false; }
	  }
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void Fermeture(const sAutoNDE& at, etatset_t& e){
  // Cette fonction clot l'ensemble d'états E={e_0, e_1, ... ,e_n} passé en
  // paramètre avec les epsilon transitions

  //TODO définir cette fonction
}

////////////////////////////////////////////////////////////////////////////////

etatset_t Delta(const sAutoNDE& at, const etatset_t& e, symb_t c){
  //TODO sur la base de celle pour le cas sans transitions spontanées,
  // définir cette fonction en utilisant Fermeture

  return e;
}

////////////////////////////////////////////////////////////////////////////////

bool Accept(const sAutoNDE& at, string str){
  //TODO définir cette fonction

  return false;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Determinize(const sAutoNDE& at){
  sAutoNDE r;

  //TODO définir cette fonction

  return r;
}

////////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& out, const sAutoNDE& at){
  //TODO définir cette fonction


    // On affiche les nombres d'états disponible
    out << "J'ai " << at.nb_etats << " état dans mon automats." << endl;

    // On affiche les nombres de symbole
    out << "J'ai " << at.nb_symbs << " symboles." << endl;

     // On affiche le nombre d'état finaux.
    out << "J'ai " << at.nb_finaux << " états finaux disponibles." << endl;

    // On affiche l'état initial.
    out << "Mon état initial est : " << endl;
    out << at.initial << endl;
    // On affiche les états finaux possible.
    out << "Mes états finaux sont : " << endl;
    for (auto i = at->finaux.cbegin(); i != at.finaux.cend(); ++i) {
        out << ' ' << *i;
    }
    out << endl;

    // On affiche les transitions.
    // ICi où j'ai le doute dans le sens de lecture.
    for (auto it_begin = at.trans.cbegin(); it_begin != at.trans.cend(); ++it_begin) {
        // On commence par l'état de départ
        out << "transition :" << *it_begin << " -> ";
        for (auto it_lettre = it_begin->cbegin(); it_lettre != it_begin->cend(); ++it_lettre){
            // On passe maintenant à la lettre utilisé pour faire la transition.
            out << *it_lettre << " -> ";
            for (auto it_arrive = it_lettre->cbegin(); it_arrive != it_lettre->cend(); ++it_arrive) {
                // On est maintenant dans l'état d'arrivé.
                out << *it_arrive << endl;
            }
        }
    }


    if (!at.epsilon.isEmpty()) {
        for (auto it_epsilon_begin = at.epsilon.cbegin(); it_epsilon_begin != at.epsilon.cend(); ++it_epsilon_begin) {
            // On est au début
            out << "transition epsilon :" << *it_epsilon_begin << " -> " << "e " << " -> ";
            for (auto it_epsilon_end = it_epsilon_begin->cbegin(); it_epsilon_end != it_epsilon_begin->cend(); ++it_epsilon_end) {
                // On est à l'état final de la transition.
                out << *it_epsilon_end << endl;
            }
        }
    }
    return out;
}

////////////////////////////////////////////////////////////////////////////////

bool ToGraph(sAutoNDE& at, string path){
  // NE FONCTIONNE PAS

	FILE* f = fopen(path.c_str(), "w");
	if(f == NULL) { return false; }

	fprintf(f, "digraph finite_state_machine {\n");
	fprintf(f, "\trankdir=LR\n");
	fprintf(f, "\tsize=\"10,10\"\n\n");
	
	// Etats finaux
	fprintf(f, "\tnode [shape = doublecircle]; ");
	for(auto it = at.finaux.cbegin(); it != at.finaux.cend(); it++) {
		fprintf(f, "%lu ", *it);
	}
	fprintf(f, ";\n");
	
	fprintf(f, "\tnode [shape = point]; q;\n");
	fprintf(f, "\tnode [shape = circle];\n\n");
	
	// Etat initial
	fprintf(f, "\tq -> %lu;\n", at.initial);
	
	// Transitions
	int s, a;
	s = 0;
	for(auto it_s = at.trans.cbegin(); it_s != at.trans.cend(); it_s++) {
		// s = l'état source
		a = 0;
		for(auto it_a = it_s->cbegin(); it_a != it_s->cend(); it_a++) {
			// a = le symbole de transition
			for(auto it_t = it_a->cbegin(); it_t != it_a->cend(); it_t++) {
				// *it_t = l'état d'arrivée
				fprintf(f, "\t%u -> %lu [label = \"%c\"];\n", s, *it_t, a + ASCII_A);
                if(DEBUG) { printf("%u -%c-> %lu\n", s, a + ASCII_A, *it_t); }
			}
			a++;
		}
		s++;
	}
	fprintf(f, "\n");
	
	// Epsilon transitions
	s = 0;
	for(auto it_s = at.epsilon.cbegin(); it_s != at.epsilon.cend(); it_s++) {
		for(auto it_t = it_s->cbegin(); it_t != it_s->cend(); it_t++) {
			fprintf(f, "\t%u -> %lu [label = \"ε\"];\n", s, *it_t);
		}
		s++;
	}
	
	fprintf(f, "\n} \n");
	fclose(f);
	
  return true;
}


// -----------------------------------------------------------------------------
// Fonctions à compléter pour la seconde partie du projet
// -----------------------------------------------------------------------------

sAutoNDE Append(const sAutoNDE& x, const sAutoNDE& y){
  // fonction outil : on garde x, et on "ajoute" trans et epsilon de y
  // en renommant ses états, id est en décallant les indices des états de y
  // de x.nb_etats 
  assert(x.nb_symbs == y.nb_symbs);
  sAutoNDE r;

  //TODO définir cette fonction

  return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Union(const sAutoNDE& x, const sAutoNDE& y){
  assert(x.nb_symbs == y.nb_symbs);
  sAutoNDE r = Append(x, y);

  //TODO définir cette fonction

  return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Concat(const sAutoNDE& x, const sAutoNDE& y){
  assert(x.nb_symbs == y.nb_symbs);
  sAutoNDE r = Append(x, y);

  //TODO définir cette fonction

  return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Complement(const sAutoNDE& x){
  //TODO définir cette fonction

  return x;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Kleene(const sAutoNDE& x){
  //TODO définir cette fonction

  return x;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Intersection(const sAutoNDE& x, const sAutoNDE& y){
  //TODO définir cette fonction

  return x;

}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE ExpressionRationnelle2Automate(string expr){
  cout << "Construction d'un automate à partir d'une expression rationnelle\n";
  cout << "  Expression en entrée (string) : " << expr << endl;

  sExpressionRationnelle er = lit_expression_rationnelle(expr);

  cout << "  Expression en entrée (ASA)    : " << er << endl;

  sAutoNDE r;

  //TODO définir cette fonction

  return r;
}

////////////////////////////////////////////////////////////////////////////////

string Automate2ExpressionRationnelle(sAutoNDE at){
  cout << "Construction d'une expression rationnelle à partir d'un automate\n";

  string sr;

  //TODO définir cette fonction

  return sr;
}

////////////////////////////////////////////////////////////////////////////////

bool Equivalent(const sAutoNDE& a1, const sAutoNDE& a2) {

  //TODO définir cette fonction

  return false;
}

////////////////////////////////////////////////////////////////////////////////

void Help(ostream& out, char *s){
  out << "Utilisation du programme " << s << " :" << endl ;
  out << "-acc ou -accept Input Word :\n\t détermine si le mot Word est accepté par l'automate Input" << endl;
  out << "-det ou -determinize Input Output [-g] :\n\t déterminise l'automate Input, écrit le résultat dans Output" << endl;
  out << "-isdet ou -is_deterministic Input :\n\t détermine si l'automate Input est déterministe" << endl;
  out << "-aut2expr ou automate2expressionrationnelle Input :\n\t calcule l'expression rationnelle correspondant à l'automate Input et l'affiche sur la sortie standard" << endl;
  out << "-expr2aut ou expressionrationnelle2automate ExpressionRationnelle Output [-g] :\n\t calcule l'automate correspondant à ExpressionRationnelle, écrit l'automate résultant dans Output" << endl;
  out << "-equ ou -equivalent Input1 Intput2 :\n\t détermine si les deux automates Input1 et Input2 sont équivalents" << endl;
  out << "-nop ou -no_operation Input Output [-g] :\n\t ne fait rien de particulier, recopie l'entrée dans Output" << endl;
  
  out << "Exemple '" << s << " -determinize auto.txt resultat -g" << endl;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[] ){
  if(argc < 3){
    Help(cout, argv[0]);
    return EXIT_FAILURE;
  }
  
  int pos;
  int act=-1;                 // pos et act pour savoir quelle action effectuer
  int nb_ifiles = 0;          // nombre de fichiers en entrée
  int nb_ofiles = 0;          // nombre de fichiers en sortie
  string str, in1, in2, out, acc, expr;
  // chaines pour (resp.) tampon; fichier d'entrée Input1; fichier d'entrée Input2;
  // fichier de sortie et chaine dont l'acceptation est à tester 
  bool graphMode=false;     // sortie graphviz ?

  // options acceptées
  const size_t NBOPT = 8;
  string aLN[] = {"accept", "determinize", "is_terministic", "automate2expressionrationnelle", "expressionrationnelle2automate", "equivalent", "no_operation", "graph"};
  string aSN[] = {"acc", "det", "isdet", "aut2expr", "expr2aut", "equ", "nop", "g"};
  
  // on essaie de "parser" chaque option de la ligne de commande
  for(int i=1; i<argc; ++i){
    if (DEBUG) cerr << "argv[" << i << "] = '" << argv[i] << "'" << endl;
    str = argv[i];
    pos = -1;
    string* pL = find(aLN, aLN+NBOPT, str.substr(1));
    string* pS = find(aSN, aSN+NBOPT, str.substr(1));
    
    if(pL!=aLN+NBOPT)
      pos = pL - aLN;
    if(pS!=aSN+NBOPT)
      pos = pS - aSN;   
      
    if(pos != -1){
      // (pos != -1) <=> on a trouvé une option longue ou courte
      if (DEBUG) cerr << "Key found (" << pos << ") : " << str << endl;
      switch (pos) {
        case 0: //acc
          in1 = argv[++i];
          acc = argv[++i];
	  nb_ifiles = 1;
	  nb_ofiles = 0;
          break;
        case 1: //det
          in1 = argv[++i];
          out = argv[++i];
	  nb_ifiles = 1;
	  nb_ofiles = 1;
          break;
        case 2: //isdet
          in1 = argv[++i];
	  nb_ifiles = 1;
	  nb_ofiles = 0;
          break;
        case 3: //aut2expr
          in1 = argv[++i];
	  nb_ifiles = 1;
	  nb_ofiles = 0;
          break;
        case 4: //expr2aut
          expr = argv[++i];
          out = argv[++i];
	  nb_ifiles = 0;
	  nb_ofiles = 1;
          break;
        case 5: //equ
          in1 = argv[++i];
          in2 = argv[++i];
	  nb_ifiles = 2;
	  nb_ofiles = 0;
          break;
        case 6: //nop
          in1 = argv[++i];
          out = argv[++i];
	  nb_ifiles = 1;
	  nb_ofiles = 1;
          break;          
        case 7: //g
          graphMode = true;
          break;
        default:
          return EXIT_FAILURE;
        }
    }
    else{
      cerr << "Option inconnue "<< str << endl;
      return EXIT_FAILURE;
    }
    
    if(pos<7){
      if(act > -1){
        cerr << "Plusieurs actions spécififées"<< endl;
        return EXIT_FAILURE;
      }
      else
        act = pos;
    }    
  }
  
  if (act == -1){
    cerr << "Pas d'action spécififée"<< endl;
    return EXIT_FAILURE;
  }  

/* Les options sont OK, on va essayer de lire le(s) automate(s) at1 (et at2)
et effectuer l'action spécifiée. Atr stockera le résultat*/

  sAutoNDE at1, at2, atr;
  
  // lecture du des fichiers en entrée
  if ((nb_ifiles == 1 or nb_ifiles == 2) and !FromFile(at1, in1)){
    cerr << "Erreur de lecture " << in1 << endl;
    return EXIT_FAILURE;
  }  
  if (nb_ifiles ==2 and !FromFile(at2, in2)){
    cerr << "Erreur de lecture " << in2 << endl;
    return EXIT_FAILURE;
  }  
  
  switch(act) {
  case 0: //acc
    if (Accept(at1, acc)){
      cout << "'" << acc << "' est accepté : OUI\n";
    }
    else {
      cout << "'" << acc << "' est accepté : NON\n";
    }
    break;
  case 1: //det
    atr = Determinize(at1);
    break;
  case 2: //isdet
    if (EstDeterministe(at1)){
      cout << "l'automate fourni en entrée est déterministe : OUI\n";
    }
    else {
      cout << "l'automate fourni en entrée est déterministe : NON\n";
    }
    break;
  case 3: //aut2expr
    expr =  Automate2ExpressionRationnelle(at1);
    cout << "Expression rationnelle résultante :" << endl << expr << endl;
    break;
  case 4: //expr2aut
    atr =  ExpressionRationnelle2Automate(expr);
    break;
  case 5: //equ
    if (Equivalent(at1,at2)){
      cout << "les deux automates sont équivalents : OUI\n";
    }
    else {
      cout << "les deux automates sont équivalents : NON\n";
    }
    break;
  case 6: //nop
    atr = at1;
    break;
  default:
    return EXIT_FAILURE;
  }

  if (nb_ofiles == 1){
    // on affiche le résultat
    // cout << "Automate résultat :\n----------------\n";
    // cout << atr;

    // écriture dans un fichier texte
    ofstream f((out + ".txt").c_str(), ios::trunc); 
    if(f.fail())
      return EXIT_FAILURE;
    f << atr;    

    // génération d'un fichier graphviz
    if(graphMode){
      ToGraph(atr, out + ".gv");
      system(("dot -Tpng " + out + ".gv -o " + out + ".png").c_str());
    }
  }
  
  return EXIT_SUCCESS;
}



