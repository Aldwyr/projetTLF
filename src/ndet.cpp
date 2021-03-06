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
            getline(myfile,line);
        } while (line.empty() || line[0]=='#');
        iss.clear();
        iss.str(line);
        if((iss >> at.initial).fail())
            return -1;

        // les autres lignes donnent les états finaux
        for(size_t i = 0; i < at.nb_finaux; i++){
            do{
                getline(myfile,line);
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
            getline(myfile,line);
            if (line.empty() || line[0]=='#')
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

    // trans = vector<vector<set<int>>> = [s][a]<t
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

    unsigned long size = e.size();

    for(auto it_e = e.begin(); it_e != e.end(); it_e++) {
        // *it_e = un etat (size_t)
        //cout << *it_e << " ";
        etatset_t transitions = at.epsilon[*it_e]; // les etats accessibles depuis un état donné par epsilon transition
        //cout << "transitions trouvées ";
        e.insert(transitions.begin(), transitions.end()); // on ajoute tous ces états à l'ensemble E
        //cout << "transitions insérées\n";
    }

    if(size != e.size()) {
        // il y a de nouveaux éléments, on doit effectuer leur fermeture transitive également
        Fermeture(at, e);
    }

}

////////////////////////////////////////////////////////////////////////////////

etatset_t Delta(const sAutoNDE& at, const etatset_t& e, symb_t c){

    etatset_t delta; // variable de retour
    etatset_t e_copy(e); // copie pour calculer la fermeture transitive

    // on calcule tous les états accessibles sans lire un symbole dans un premier temps
    Fermeture(at, e_copy);

    for(auto it_e = e_copy.begin(); it_e != e_copy.end(); it_e++) {
        // pour chacun des états, on ajoute à delta les états accessibles en lisant le symbole c
        etatset_t transitions = at.trans[*it_e][c - ASCII_A];
        delta.insert(transitions.begin(), transitions.end());
    }

    // on calcule les états accessibles après avoir lu ce symbole par epsilon transition
    Fermeture(at, delta);

    return delta;
}

////////////////////////////////////////////////////////////////////////////////

bool Accept(const sAutoNDE& at, string str){
    etatset_t initial; // pour stocker l'état initial.
    etatset_t deltaReturn; // pour récupérer la valeur de retour de Delta

    initial.insert(at.initial);
    deltaReturn = Delta(at, initial, str[0]);

    for (int i = 1; i < (int)str.size(); i++) {
        deltaReturn = Delta(at, deltaReturn, str[i]);
        if (deltaReturn.empty())
            return false;
    }

    for (auto i = deltaReturn.cbegin(); i != deltaReturn.cend(); i++) {
        for (auto x = at.finaux.cbegin(); x != at.finaux.cend(); x++) {
            if (*i == *x)
                return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Determinize(const sAutoNDE& at){
    sAutoNDE r;

    map_t etats; // stocke les états du nouvel automate
    // clé = l'ensemble des états, valeur = le nombre associé à cet ensemble (l'état)
    // l'opérateur [] insère automatique la clé et met la valeur à 0

    // première étape : déterminer les epsilon transitions de chaque état
    etatset_t* epsilon = new etatset_t[at.nb_etats];
    for(etat_t i = 0; i < at.nb_etats; i++) {
        etatset_t f;
        f.insert(i);
        Fermeture(at, f);
        epsilon[i] = f;
    }

    // on ajoute le premier etatset à la map : ce sera le premier état du nouvel automate, de valeur 0
    etats[epsilon[at.initial]] = 0;

    // deuxième étape : on part du nouvel état initial, et on cherche ses transitions.
    // pour chaque ensemble d'états trouvé, s'il n'est pas déjà présent dans l'automate on l'ajoute à la queue
    queue<etatset_t> queue;
    queue.push(epsilon[at.initial]);
    etat_t next_value = 1;
    unsigned int i = 0;
    while(!queue.empty()) {
        // on travaille sur un nouvel état pas encore ajouté dans l'automate (mais présent dans le map)
        r.trans.emplace_back(); // on peut accéder à r.trans[i]
        etatset_t e = queue.front();
        assert(i == etats[e]);
        for(symb_t c = 0; c < at.nb_symbs; c++) {
            // on calcule les transitions pour chaque symbole
            etatset_t delta = Delta(at, e, c + ASCII_A);
            etat_t value = etats[delta]; // le numéro du prochain état (insère l'état si besoin et renvoie 0 dans ce cas)
            if(value == 0 && !(delta == epsilon[at.initial])) { // set implémente operator==
                // si delta n'est pas l'état initial et que sa valeur est 0, alors c'est la première fois qu'on
                // trouve cet ensemble, on lui donne un numéro et on l'ajoute à la queue
                value = etats[delta] = next_value;
                next_value++;
                queue.push(delta);
            }
            r.trans[i].emplace_back(); // on peut accéder à r.trans[i][c]
            r.trans[i][c].insert(value);
        }
        i++;
        queue.pop();
    }

    // troisième étape : on renseigne les informations de l'automate
    r.nb_etats = etats.size();
    r.nb_finaux = 0;
    r.nb_symbs = at.nb_symbs;
    r.initial = 0;
    for(auto it = etats.cbegin(); it != etats.cend(); it++) {
        // it = <etatset_t, etat_t>
        // it->first = etatset_t
        // it->second = etat_t
        // on pourrait utiliser set_intersection ici
        for(auto it_f = at.finaux.cbegin(); it_f != at.finaux.cend(); it_f++) {
            if(it->first.find(*it_f) != it->first.end()) {
                // l'étatset contient au moins un état final
                r.finaux.insert(it->second);
                r.nb_finaux++;
                break;
            }
        }
    }

	delete[] epsilon;

    return r;
}

////////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& out, const sAutoNDE& at){
#if 0
    // On affiche les nombres d'états disponible
    out << "J'ai " << at.nb_etats << " états dans mon automate." << endl;

    // On affiche les nombres de symbole
    out << "J'ai " << at.nb_symbs << " symboles." << endl;

    // On affiche le nombre d'état finaux.
    out << "J'ai " << at.nb_finaux << " états finaux disponibles." << endl;

    // On affiche l'état initial.
    out << "Mon état initial est : " << endl;
    out << at.initial << endl;
    // On affiche les états finaux possible.
    out << "Mes états finaux sont : " << endl;
    for (auto i = at.finaux.cbegin(); i != at.finaux.cend(); ++i) {
        out << ' ' << *i;
    }
    out << endl;

    // On affiche les transitions.
    int s = 0;
    char a;
    for (auto it_begin = at.trans.cbegin(); it_begin != at.trans.cend(); ++it_begin) {
        // On commence par l'état de départ
        out << "transition :" << s << " -> ";
        a = ASCII_A;
        for (auto it_lettre = it_begin->cbegin(); it_lettre != it_begin->cend(); ++it_lettre){
            // On passe maintenant à la lettre utilisé pour faire la transition.
            out << a  << " -> ";
            for (auto it_arrive = it_lettre->cbegin(); it_arrive != it_lettre->cend(); ++it_arrive) {
                // On est maintenant dans l'état d'arrivé.
                out << *it_arrive << endl;
            }
            a++;
        }
        s++;
    }

    s = 0;
    for (auto it_epsilon_begin = at.epsilon.cbegin(); it_epsilon_begin != at.epsilon.cend(); ++it_epsilon_begin) {
        // On est au début
        out << "transition epsilon :" << s << " -> " << "ε " << " -> ";
        for (auto it_epsilon_end = it_epsilon_begin->cbegin(); it_epsilon_end != it_epsilon_begin->cend(); ++it_epsilon_end) {
            // On est à l'état final de la transition.
            out << *it_epsilon_end << endl;
        }
    }
#endif

	// on affiche <nb_etats> <nb_symbs> <nb_finaux>
	out << at.nb_etats << " " << at.nb_symbs << " " << at.nb_finaux << endl;
	out << at.initial << endl;
	for(auto it_f = at.finaux.cbegin(); it_f != at.finaux.cend(); it_f++) {
		out << *it_f << endl;
		// on affiche les états finaux
	}

	// on affiche (un par ligne) <état i> <symbole c> <état j> tel que (i,c,j) appartient à at.trans ou (i,j) à at.epsilon
	// avec c un caractère > nb_symbs
	char epsilon = ASCII_A + at.nb_symbs;
	for(unsigned int i = 0; i < at.nb_etats; i++) {
		for(unsigned int c = 0; c < at.nb_symbs; c++) {
			char symb = c + ASCII_A;
			for(auto it_j = at.trans[i][c].cbegin(); it_j != at.trans[i][c].cend(); it_j++) {
				out << i << " " << symb << " " << *it_j << endl;
			}
		}
		for(auto it_j = at.epsilon[i].cbegin(); it_j != at.epsilon[i].cend(); it_j++) {
			out << i << " " << epsilon << " " << *it_j << endl;
		}
	}

    return out;
}

////////////////////////////////////////////////////////////////////////////////

bool ToGraph(sAutoNDE& at, string path){

    FILE* f = fopen(path.c_str(), "w");
    if(f == NULL) { return false; }

    fprintf(f, "digraph finite_state_machine {\n");
    fprintf(f, "\trankdir=LR;\n");
    fprintf(f, "\tsize=\"10,10\"\n\n");

    // Etats finaux
    fprintf(f, "\tnode [shape = doublecircle]; ");
    for(auto it = at.finaux.cbegin(); it != at.finaux.cend(); it++) {
        fprintf(f, "%lu ", *it);
    }
    fprintf(f, ";\n");

    fprintf(f, "\tnode [shape = point ]; q;\n");
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

    fprintf(f, "\n}\n");
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

    r.nb_symbs = x.nb_symbs;
    r.epsilon.resize(x.nb_etats + y.nb_etats);
    r.trans.resize(x.nb_etats + y.nb_etats);
    // on resize à l'avance
    for(size_t i = 0; i < x.nb_etats + y.nb_etats; i++) {
        r.trans[i].resize(r.nb_symbs);
    }

    etatset_t etats;
    // on ajoute les états de x
    for(size_t i = 0; i < x.nb_etats; i++) {
        for(unsigned char c = 0; c < r.nb_symbs; c++) {
            etats = x.trans[i][c];
            r.trans[i][c].insert(etats.begin(), etats.end());
        }
        etats = x.epsilon[i];
        r.epsilon[i].insert(etats.cbegin(), etats.cend());
    }

    // on ajoute les états de y
    for(size_t i = 0; i < y.nb_etats; i++) {
        for(unsigned char c = 0; c < r.nb_symbs; c++) {
            etats = y.trans[i][c];
            for(auto it_e = etats.cbegin(); it_e != etats.cend(); it_e++) {
                r.trans[i+x.nb_etats][c].insert(*it_e + x.nb_etats);
            }
        }
        etats = y.epsilon[i];
        for(auto it_e = etats.cbegin(); it_e != etats.cend(); it_e++) {
            r.epsilon[i+x.nb_etats].insert(*it_e + x.nb_etats);
        }
    }

    return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Union(const sAutoNDE& x, const sAutoNDE& y){
    assert(x.nb_symbs == y.nb_symbs);
    sAutoNDE r = Append(x, y);

    etatset_t etats;
    vector<etatset_t> transitions;
    transitions.resize(r.nb_symbs);
    r.trans.push_back(transitions); // on crée un nouvel état sans transitions

    etats.emplace(x.initial);
    etats.emplace(y.initial + x.nb_etats);
    r.epsilon.push_back(etats); // on ajoute des epsilon transitions du nouvel état
    // vers les précédents initiaux

    r.initial = x.nb_etats + y.nb_etats; // l'état initial est ce nouvel état
    r.nb_etats = x.nb_etats + y.nb_etats +1;

    for(auto it = x.finaux.cbegin(); it != x.finaux.cend(); it++) {
        r.finaux.emplace(*it);
    }
    for(auto it = y.finaux.cbegin(); it != y.finaux.cend(); it++) {
        r.finaux.emplace(*it + x.nb_etats);
    }
    r.nb_finaux = x.nb_finaux + y.nb_finaux;

    return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Concat(const sAutoNDE& x, const sAutoNDE& y){
    assert(x.nb_symbs == y.nb_symbs);
    sAutoNDE r = Append(x, y);

    r.nb_etats = x.nb_etats + y.nb_etats;
    for(auto it = x.finaux.cbegin(); it != x.finaux.cend(); it++) {
        // on ajoute une epsilon transition vers l'initial de y
        r.epsilon[*it].insert(x.nb_etats + y.initial);
    }

    r.nb_finaux = 0;
    for(auto it = y.finaux.cbegin(); it != y.finaux.cend(); it++) {
        // les états finaux de r sont les états finaux de y
        r.finaux.insert(x.nb_etats + *it);
        r.nb_finaux++;
    }

    r.initial = x.initial;

    return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Complement(const sAutoNDE& x){

    // il faut que l'automate x soit déterministe
    assert(EstDeterministe(x));
    sAutoNDE r;
    r.nb_symbs = x.nb_symbs;
    r.nb_etats = x.nb_etats;
    r.nb_finaux = x.nb_etats - x.nb_finaux;
    r.initial = x.initial;
    r.trans = x.trans;
    r.epsilon = x.epsilon;

    for(unsigned int i = 0; i < r.nb_etats; i++) {
        std::set<etat_t>::iterator it = r.finaux.find(i);
        if(it == r.finaux.end()) {
            // l'état numéro i n'est pas dans la liste des finaux de x
            r.finaux.insert(i);
        }
    }

    return r;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Kleene(const sAutoNDE& x){
    //TODO définir cette fonction
    sAutoNDE res;

    res.initial = x.nb_etats;
    res.nb_etats = x.nb_etats + 1;
    res.nb_finaux = x.nb_finaux + 1;
    res.nb_symbs = x.nb_symbs;


    // On rajoute les transitions normal.
    for (auto it_x = x.trans.begin(); it_x != x.trans.end(); ++it_x) {
        res.trans.push_back(*it_x);
    }
    res.trans.resize(res.nb_etats);
    res.trans[res.initial].resize(res.nb_symbs);
    // On récupérer les transition epsilon de l'ancien automate.
    res.epsilon = x.epsilon;
    // On agrandit de 1 pour que l'état final puisse pointer vers l'état initial de x;
    res.epsilon.resize(x.nb_etats + 1);
    // On rajoute les état finaux de x et on donne une transition epsilon vers le nouvel état final de res
    for (auto it_x = x.finaux.begin(); it_x != x.finaux.end(); ++it_x) {
        res.finaux.insert(*it_x);
        res.epsilon[*it_x].insert(res.initial);
    }
    res.finaux.insert(x.nb_etats);

    res.epsilon[res.initial].insert(x.initial);  // on fait pointer l'état initial du nouvelle automate vers l'ancien état initial.

    return res;
}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE Intersection(const sAutoNDE& x, const sAutoNDE& y){
    //TODO définir cette fonction

    return x;

}

////////////////////////////////////////////////////////////////////////////////

sAutoNDE expr2Aut(sExpressionRationnelle er) {
    sAutoNDE r;
    switch(er->op) {
        case o_variable: {
            char c = er->nom->at(0);
            r.trans.resize(2);
            r.nb_etats = 2;
            r.nb_symbs = 4; // on précise a, b, c, d par défaut pour que les concat et union se passent bien
            r.trans[0].resize(r.nb_symbs);
            r.trans[1].resize(r.nb_symbs);
            r.epsilon.resize(2);
            r.initial = 0;
            r.finaux.insert(1);
            r.nb_finaux = 1;

            if (c < 'e') {
                // il s'agit d'une transition normale
                r.trans[0][c - ASCII_A].insert(1);
            } else {
                // il s'agit d'une epsilon transition
                assert(c == 'e');
                r.epsilon[0].insert(1);
            }
            break;
        }
        case o_ou: {
            r = Union(expr2Aut(er->arg1), expr2Aut(er->arg2));
            break;
        }
        case o_concat: {
            sAutoNDE r1 = expr2Aut(er->arg1);
            sAutoNDE r2 = expr2Aut(er->arg2);
            r = Concat(r1,r2);
            break;
        }
        case o_etoile: {
            r = Kleene(expr2Aut(er->arg));
            break;
        }
        default: {
            assert(false);
            // erreur
        }
    }
    return r;
}

sAutoNDE ExpressionRationnelle2Automate(string expr){
    cout << "Construction d'un automate à partir d'une expression rationnelle\n";
    cout << "  Expression en entrée (string) : " << expr << endl;

    sExpressionRationnelle er = lit_expression_rationnelle(expr);

    cout << "  Expression en entrée (ASA)    : " << er << endl;

    sAutoNDE r;

    r = expr2Aut(er);

    return r;
}

////////////////////////////////////////////////////////////////////////////////

string Automate2ExpressionRationnelle(sAutoNDE at){
    cout << "Construction d'une expression rationnelle à partir d'un automate\n";

    string sr;

	sAutoNDE at2;
	at2.nb_symbs = at.nb_symbs;
	at2.nb_etats = at.nb_etats + 2;
	at2.initial = 0;
	at2.epsilon.resize(at2.nb_etats);
	at2.trans.resize(at2.nb_etats);
	for(unsigned int i = 0; i < at2.nb_etats; i++) { at2.trans.at(i).resize(at2.nb_symbs); }

	for(unsigned int i = 0; i < at.nb_etats; i++) {
		for(unsigned char c = 0; c < at.nb_symbs; c++) {
			for(auto it_e = at.trans.at(i).at(c).cbegin(); it_e != at.trans.at(i).at(c).cend(); it_e++) {
				etat_t e = *it_e + 1;
				at2.trans.at(i + 1).at(c).insert(e);
			}
		}
		for(auto it_e = at.epsilon.at(i).cbegin(); it_e != at.epsilon.at(i).cend(); it_e++) {
			etat_t e = *it_e + 1;
			at2.epsilon.at(i + 1).insert(e);
		}
	}
	for(auto it_e = at.finaux.cbegin(); it_e != at.finaux.cend(); it_e++) {
		etat_t e = *it_e + 1;
		at2.epsilon.at(e).insert(at2.nb_etats - 1);
	}
	at2.finaux.insert(at2.nb_etats - 1);
	at2.nb_finaux = 1;
	at2.epsilon.at(0).insert(at.initial + 1);

    // Initialisation du tableau de string.
	string*** R = new string**[at2.nb_etats];
	for(unsigned i = 0; i < at2.nb_etats; i++) { // initialisation à une chaîne vide pour tester plus loin si
		R[i] = new string*[at2.nb_etats]; // on a déjà ajouté un élément ou non
		for(unsigned j = 0; j < at2.nb_etats; j++) {
			R[i][j] = new string[at2.nb_etats-1];
			for(unsigned k = 0; k < at2.nb_etats - 1; k++) {
				R[i][j][k] = "";
			}
		}
	}

    // début du parcours en cherchant les transition direct. dans l'automate.
	for(unsigned int i = 0; i < at2.nb_etats; i++) {
		for(unsigned int c = 0; c < at2.nb_symbs; c++) {
            // On rentre dans la boucle for uniquement si un it_j est trouvé.
			for(auto it_j = at2.trans[i][c].cbegin(); it_j != at2.trans[i][c].cend(); it_j++) {
				char symb = c + ASCII_A;
				string s = string(1, symb);
				if(R[i][*it_j][0] != "") {
					// on a déjà une ou plusieurs transitions, on ajoute celle-ci
					R[i][*it_j][0] += " | " + s;
				} else {
					R[i][*it_j][0] = s;
				}
			}
		}
		for(auto it_j = at2.epsilon[i].cbegin(); it_j != at2.epsilon[i].cend(); it_j++) {
			if(R[i][*it_j][0] != "") {
				// on a déjà une ou plusieurs transitions, on ajoute celle-ci
				R[i][*it_j][0] += " | e";
			} else {
				R[i][*it_j][0] = "e";
			}
		}
	}

    for(unsigned int k = 1; k < at2.nb_etats - 1; k++) {
        for(unsigned int i = 0; i < at2.nb_etats; i++) {
            for(unsigned int j = 0; j < at2.nb_etats; j++) {
                // rappel : R(i,j,k) = R(i,j,k-1) U R(i,k,k-1)R(k,k,k-1)* R(k,j,k-1)

	            string s1 = R[i][j][k-1]; // = D
	            string s2 = R[i][k][k-1]; // = A
	            string s3 = R[k][k][k-1]; // = C
	            string s4 = R[k][j][k-1]; // = B
	            string s5 = ""; // = A.C*.B

	            if(!(s2 == "" || s4 == "")) {
		            // il existe un chemin de i à k et de k à j (A et B), potentiellement epsilon
		            if(s2 != "e") {
			            // on ne veut pas du e de tête car inutile (si A = C = B = e, alors on ajoute le e de fin)
			            if(s2.find('|') != string::npos) {
				            // s2 contient une union, on ajoute des parenthèses pour garantir la priorité
				            s5 += "(" + s2 + ")";
			            } else {
				            s5 += s2;
			            }
		            }
	                // on ajoute potentiellement C
		            if(!(s3 == "" || s3 == "e")) {
			            if(s5 != "") { s5 += "."; } // concaténation si s5 non vide, càd si s2 != e
			            if(s3.length() > 1) {
				            // on entoure de parenthèses
				            s5 += "(" + s3 + ")*";
			            } else {
				            s5 += s3 + "*";
			            }
		            }
		            if(s4 != "e" || s5 == "") {
			            // soit s5 est vide (càd A = C = e), soit B != e, on ajoute B
			            if(s5 != "") { s5 += "."; } // concaténation si s5 non vide, càd si s2 != e et/ou s3 != e
			            if(s4.find('|') != string::npos) {
				            // s4 contient une union, on ajoute des parenthèses pour garantir la priorité
				            s5 += "(" + s4 + ")";
			            } else {
				            s5 += s4;
			            }
		            }
	            }

	            // on compare s1 et s5 et on met la bonne formule dans R[i][j][k]
	            if(s1 == "") {
		            // rien dans s1, donc on ne met que s5
		            R[i][j][k] = s5;
	            } else if(s5 == "" || s1 == s5) {
		            // rien dans s5 ou s5 égal à s1, on ne met que s1
		            R[i][j][k] = s1;
	            } else {
		            if(s5.find('|') != string::npos) {
			            // s5 contient une union, on l'encadre avec des parenthèses
			            R[i][j][k] = s1 + " | (" + s5 + ")";
		            } else {
			            R[i][j][k] = s1 + " | " + s5;
		            }
	            }
	            //R[i][j][k] = R[i][j][k-1] + " | " + R[i][k][k-1] + " (" + R[k][k][k-1]+ ")* "+
	            // R[k][j][k-1];R[i][j][k] = R[i][j][k-1] + " | " + R[i][k][k-1] + " (" + R[k][k][k-1]+ ")* "+ R[k][j][k-1];

	            //std::cout << R[i][j][k] << std::endl;
            }
        }
    }
	sr =  string(R[0][at2.nb_etats-1][at2.nb_etats-2]); // copie
	for(unsigned int i = 0; i < at2.nb_etats; i++) {
		for(unsigned int j = 0; j < at2.nb_etats; j++) {
			delete[] R[i][j];
		}
		delete[] R[i];
	}
    //std::cout << sr << std::endl;
    return sr;
}

////////////////////////////////////////////////////////////////////////////////

bool Equivalent(const sAutoNDE& a1, const sAutoNDE& a2) {

    if(a1.nb_symbs != a2.nb_symbs) { return false; }

    const size_t LAST = ASCII_A + a1.nb_symbs - 1;
    size_t word_max_size = max(a1.nb_etats, a2.nb_etats);
    for(size_t i = 1; i <= word_max_size; i++) {
        // i = la taille du mot
        char* word = (char*) malloc(i+1);
        for(size_t j = 0; j < i; j++) { word[j] = ASCII_A; }
        word[i] = '\0';
        int index; // index est la position de la lettre à modifier
        do {
            index = i-1;

            // on change la dernière lettre
            for(unsigned char c = ASCII_A; c <= LAST; c++) {
                word[i-1] = c;
                //std::cout << string(word) << std::endl; // on vient de générer un nouveau mot
                if(Accept(a1, string(word)) != Accept(a2, string(word))) { free(word); return false; }
            }

            while(index >= 0 && word[index] == (int) LAST) {
                // la lettre est la dernière de l'alphabet, on la remet au début 'a' et on passe à la lettre d'avant
                word[index] = ASCII_A;
                index--;
            }
            if(index >= 0) {
                word[index]++;
                // index est la dernière lettre n'étant pas au bout de sa boucle, on l'incrémente
                // et on recommence le traitement
            }

        } while (index >= 0);

        free(word);
    }

    return true;
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



