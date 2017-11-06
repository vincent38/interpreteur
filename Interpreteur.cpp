#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <typeinfo>
//#include "SymboleValue.h"
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr), m_error(false) {
}

void Interpreteur::analyse() {
    m_arbre = programme(); // on lance l'analyse de la première règle
    if (m_error) {
        m_arbre = nullptr;
        throw SyntaxeException("Syntaxe incorrecte");
    }
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
    static char messageWhat[256];
    if (m_lecteur.getSymbole() != symboleAttendu) {
        sprintf(messageWhat,
                "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
                m_lecteur.getLigne(), m_lecteur.getColonne(),
                symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
        throw SyntaxeException(messageWhat);
    }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
    // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
    tester(symboleAttendu);
    m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
    // Lève une exception contenant le message et le symbole courant trouvé
    // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
    static char messageWhat[256];
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    testerEtAvancer("procedure");
    testerEtAvancer("principale");
    testerEtAvancer("(");
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finproc");
    tester("<FINDEFICHIER>");
    return sequence;
}

Noeud* Interpreteur::seqInst() {
    // <seqInst> ::= <inst> { <inst> }
    NoeudSeqInst* sequence = new NoeudSeqInst();
    do {
        /*try {*/
        sequence->ajoute(inst());
        /*} catch (InterpreteurException & e) {
            cout << e.what() << endl;
            int l = m_lecteur.getLigne();
            //Fast forward to next thing
            //A MODIFIER : pour l'instant on passe à la ligne suivante, ce n'est pas bon!
            while (m_lecteur.getLigne() == l) {
                m_lecteur.avancer();
            }
            cout << m_lecteur.getSymbole() << endl;
            m_error = true;
        }*/

    } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "lire" || m_lecteur.getSymbole() == "++" || m_lecteur.getSymbole() == "--" || m_lecteur.getSymbole() == "selon");
    // Tant que le symbole courant est un début possible d'instruction...
    // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
    return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    try {
        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        } else if (m_lecteur.getSymbole() == "si")
            return instSiRiche();
            // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
        else if (m_lecteur.getSymbole() == "tantque") {
            return instTantQue();
        } else if (m_lecteur.getSymbole() == "repeter") {
            return instRepeter();
        } else if (m_lecteur.getSymbole() == "pour") {
            return instPour();
        } else if (m_lecteur.getSymbole() == "ecrire") {
            return instEcrire();
        } else if (m_lecteur.getSymbole() == "lire") {
            return instLire();
        } else if (m_lecteur.getSymbole() == "++") {
            return preInc();
        } else if (m_lecteur.getSymbole() == "--") {
            return preDec();
        } else if (m_lecteur.getSymbole() == "selon") {
            return instSelon();
        } else erreur("Instruction incorrecte");
    } catch (InterpreteurException & e) {
        cout << e.what() << endl;
        m_error = true;
    }
}

Noeud* Interpreteur::affectation() {
    // <affectation> ::= <variable> = <expression> 
    tester("<VARIABLE>");
    Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
    SymboleValue* v = m_table.chercheAjoute(m_lecteur.getSymbole());
    m_lecteur.avancer();
    //Recherche post incr/decr ICI
    if (m_lecteur.getSymbole() == "++") {
        m_lecteur.avancer();
        return new NoeudPostIncrementation(v);
    } else if (m_lecteur.getSymbole() == "--") {
        m_lecteur.avancer();
        return new NoeudPostDecrementation(v);
    }
    testerEtAvancer("=");
    Noeud* exp = expression(); // On mémorise l'expression trouvée
    return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

/*Noeud* Interpreteur::expression() {
    // <expression> ::= <facteur> { <opBinaire> <facteur> }
    //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-" ||
            m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/" ||
            m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=" ||
            m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* factDroit = facteur(); // On mémorise l'opérande droit
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
    }
    return fact; // On renvoie fact qui pointe sur la racine de l'expression
}*/

Noeud* Interpreteur::expression() {
    // <expression> ::= <terme> { + <terme> | - <terme> }
    Noeud* t1 = terme();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* t2 = terme(); // On mémorise l'opérande droit
        t1 = new NoeudOperateurBinaire(operateur, t1, t2); // Et on construuit un noeud opérateur binaire
    }
    return t1; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::terme() {
    // <terme> ::= <facteur> { * <facteur> | / <facteur> }
    Noeud* relation1 = facteur();
    while (m_lecteur.getSymbole() == "*" or m_lecteur.getSymbole() == "/") {
        Symbole op = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* relation2 = facteur();
        relation1 = new NoeudOperateurBinaire(op, relation1, relation2);
    }
    return relation1;
}

Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <expBool> | non <expBool> | ( <expBool> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <expBool>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), expBool());
    } else if (m_lecteur.getSymbole() == "non") { // non <expBool>
        m_lecteur.avancer();
        fact = new NoeudOperateurBinaire(Symbole("non"), expBool(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expBool();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}

Noeud* Interpreteur::expBool() {
    // <expBool> ::= <relationEt> { ou <relationEt> }
    Noeud* relation1 = relationEt();
    while (m_lecteur.getSymbole() == "ou") {
        Symbole opEt = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* relation2 = relationEt();
        relation1 = new NoeudOperateurBinaire(opEt, relation1, relation2);
    }
    return relation1;
}

Noeud* Interpreteur::relationEt() {
    // <relationEt> ::= <relation> { et <relation> }
    Noeud* relation1 = relation();
    while (m_lecteur.getSymbole() == "et") {
        Symbole opEt = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* relation2 = relation();
        relation1 = new NoeudOperateurBinaire(opEt, relation1, relation2);
    }
    return relation1;
}

Noeud* Interpreteur::relation() {
    // <relation> ::= <expression> { <opRel> <expression> }
    // <opRel> ::= == | != | < | <= | > | >=
    Noeud* exp = expression();
    while (m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=" ) {
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* exp2 = expression();
        exp = new NoeudOperateurBinaire(operateur, exp, exp2); // Et on construuit un noeud opérateur binaire
    }
    return exp;
}

/*Noeud* Interpreteur::facteur() {
    // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
    Noeud* fact = nullptr;
    if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
        fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
        m_lecteur.avancer();
    } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
    } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
        m_lecteur.avancer();
        // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
        fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
    } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
        m_lecteur.avancer();
        fact = expression();
        testerEtAvancer(")");
    } else
        erreur("Facteur incorrect");
    return fact;
}*/

Noeud* Interpreteur::instSi() { //---------------------------------------à retirer quand siriche marchera------------
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    testerEtAvancer("si");
    testerEtAvancer("(");
    Noeud* condition = expBool(); // On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); // On mémorise la séquence d'instruction
    testerEtAvancer("finsi");
    return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
}

Noeud* Interpreteur::instTantQue() {
    // <instTantQue> ::= tantque( <expression> ) <seqInst> fintantque
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expBool(); //On mémorise la condition
    testerEtAvancer(")");
    Noeud* sequence = seqInst(); //On mémorise la séquence d'instruction
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence); //on renvoie un noeud NoeudInstTantQue
}

Noeud* Interpreteur::instRepeter() {
    // <instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst(); //On mémorise la séquence d'instruction
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expBool(); //On mémorise la condition
    testerEtAvancer(")");

    return new NoeudInstRepeter(condition, sequence); //on renvoie un noeud NoeudInstTantQue
}

Noeud* Interpreteur::instSiRiche() {
    // <instSiRiche> ::= si(<expression> )<seqInst> {sinonsi(<expression>) <seqInst> }[sinon <seqInst>]finsi
    bool testSinonSi = 1;
    bool testSinon = 1;
    vector<Noeud*> expressionSinonsi;
    vector<Noeud*> seqInstSinonsi;
    Noeud* condition1 = NULL;
    Noeud* sequence1 = NULL;
    Noeud* conditionAjout;
    Noeud* seqInstAjout;
    Noeud* sequenceSinon = NULL;

    testerEtAvancer("si");
    testerEtAvancer("(");
    condition1 = expBool();
    testerEtAvancer(")");
    sequence1 = seqInst();
    //sinonsi
    /*try{
        testerEtAvancer("sinonsi");
    } catch (SyntaxeException e){
        testSinonSi = 0;
    }
    if (testSinonSi){
        testerEtAvancer("(");
        conditionAjout = expression();
        expressionSinonsi.push_back(conditionAjout)
        testerEtAvancer(")");
        seqInstAjout = seqInst();
        seqInstSinonsi.push_back(seqInstAjout);
        
    }*/
    /*try{
        tester("sinonsi");
    } catch (SyntaxeException e){
        testSinonSi = 0;
    }*/
    /*if(m_lecteur.getSymbole() != "sinonsi"){
        testSinonSi =0;
    }*/
    while (m_lecteur.getSymbole() == "sinonsi") {
        m_lecteur.avancer();
        testerEtAvancer("(");
        conditionAjout = expBool();
        expressionSinonsi.push_back(conditionAjout);
        testerEtAvancer(")");
        seqInstAjout = seqInst();
        seqInstSinonsi.push_back(seqInstAjout);
        /*try{
            tester("sinonsi");
        } catch (SyntaxeException e){
            testSinonSi = 0;
        }*/
        /*if(m_lecteur.getSymbole() != "sinonsi"){
        testSinonSi =0;
    }*/
    }
    //sinon
    /*try{
        tester("sinon");
    } catch (SyntaxeException e){
        testSinon = 0;
    }*/
    /*if(m_lecteur.getSymbole() != "sinon"){
        testSinon = 0;
    }*/
    if (m_lecteur.getSymbole() == "sinon") {
        m_lecteur.avancer();
        sequenceSinon = seqInst();
    }
    testerEtAvancer("finsi");

    return new NoeudInstSiRiche(condition1, sequence1, expressionSinonsi, seqInstSinonsi, sequenceSinon);
}

Noeud* Interpreteur::instPour() {
    //<instPour> ::= pour ( [<affectation>] ; <expression> ; [ <affectation> ] ) <seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    //Affectation 0 ou 1
    Noeud* affectationInit = nullptr;
    Noeud* affectation2 = nullptr;
    try {
        affectationInit = affectation();
        testerEtAvancer(";");
    } catch (SyntaxeException e) {
        m_lecteur.avancer();
    }
    Noeud* condition = expBool();
    //affectation2 0 ou 1
    if (m_lecteur.getSymbole() == ";") {
        testerEtAvancer(";");
        affectation2 = affectation();
    }
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    Noeud* monPour = new NoeudInstPour(condition, sequence);
    monPour->ajoute(affectationInit);
    monPour->ajoute(affectation2);
    return monPour;
}

Noeud* Interpreteur::instEcrire() {
    //<instEcrire> ::= ecrire ( <expression> | <chaîne> {, <expression> | <chaîne> })
    Noeud* write = new NoeudInstEcrire();
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    if (m_lecteur.getSymbole() == "<CHAINE>") {
        write->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole())); // La variable est ajoutée à la table et on la mémorise
        m_lecteur.avancer();
    } else {
        write->ajoute(expression());
    }
    while (m_lecteur.getSymbole() == ",") {
        testerEtAvancer(",");
        if (m_lecteur.getSymbole() == "<CHAINE>") {
            write->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole())); // La variable est ajoutée à la table et on la mémorise
            m_lecteur.avancer();
        } else {
            write->ajoute(expression());
        }
    }
    testerEtAvancer(")");
    return write;
}

Noeud* Interpreteur::instLire() {
    bool varExist = 1;
    vector<SymboleValue*> vars;
    SymboleValue* var;

    testerEtAvancer("lire");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    //m_lecteur.getSymbole();
    var = m_table.chercheAjoute(m_lecteur.getSymbole());
    vars.push_back(var);
    m_lecteur.avancer();
    while (varExist) { //res = vars contient le nom de toute les variables concerner dans l'ordre
        if (m_lecteur.getSymbole() == ",") {
            m_lecteur.avancer();
            if (m_lecteur.getSymbole() == "<VARIABLE>") {
                var = m_table.chercheAjoute(m_lecteur.getSymbole());
                vars.push_back(var);
                m_lecteur.avancer();
            }
        } else {
            varExist = 0;
        }
    }
    testerEtAvancer(")");
    return new NoeudInstLire(vars);
}

Noeud* Interpreteur::instSelon(){            // <!> Fonction inactive, problème lors de l'analyse <!>
    // <instSeleon> ::= Selon (<variable>) { cas <entier> : <SeqInst> } [defaut : <SeqInst>] FinSelon
    Noeud* var;
    vector<Noeud*> seqInsts;
    Noeud* seqInstActuellementTraitee;
    vector<Noeud*> entiers;
    Noeud* seqParDefaut;
    
    testerEtAvancer("selon");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    var = m_table.chercheAjoute(m_lecteur.getSymbole());
    m_lecteur.avancer();
    testerEtAvancer(")");
    
    while(m_lecteur.getSymbole() == "cas"){
        m_lecteur.avancer();
        if (m_lecteur.getSymbole() != "<ENTIER>") {
            erreur("Entier attendu !");
        }
        entiers.push_back(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
        testerEtAvancer(":");
        
        seqInstActuellementTraitee = seqInst();
        seqInsts.push_back(seqInstActuellementTraitee); 
        //m_lecteur.avancer();
        testerEtAvancer("fincas");
    }
    
    if (m_lecteur.getSymbole() == "defaut"){
        m_lecteur.avancer();
        testerEtAvancer(":");
        seqParDefaut = seqInst();
    } 
    testerEtAvancer("finselon");
    
    return new NoeudInstSelon(var,entiers,seqInsts,seqParDefaut);   
}

Noeud* Interpreteur::preInc() {
    testerEtAvancer("++");
    NoeudPreIncrementation* b = new NoeudPreIncrementation(m_table.chercheAjoute(m_lecteur.getSymbole())); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
    testerEtAvancer(";");
    return b;
}

Noeud* Interpreteur::preDec() {
    testerEtAvancer("--");
    NoeudPreIncrementation* b = new NoeudPreIncrementation(m_table.chercheAjoute(m_lecteur.getSymbole())); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
    testerEtAvancer(";");
    return b;
}

void Interpreteur::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << "#include <iostream>" << endl;
    cout << "using namespace std;" << endl;
    cout << endl;
    cout << "int main() {" << endl;
    cout << endl;
    cout << setw(4) << "" << "//Variables utilisateur" << endl;
    // Début d’un programme C++
    // Ecrire en C++ la déclaration des variables présentes dans le programme... 
    // ... variables dont on retrouvera le nom en parcourant la table des symboles ! 
    // Par exemple, si le programme contient i,j,k, il faudra écrire : int i; int j; int k; ... 
    for (int s = 0; s < m_table.getTaille(); s++) {
        SymboleValue b = m_table[s];
        if (b == "<VARIABLE>") {
            cout << setw(4) << "" << "int ";
            cout << b.getChaine();
            cout << ";" << endl;
        }
    }
    cout << endl;
    cout << setw(4) << "" << "//Programme généré";
    cout << endl;
    getArbre()->traduitEnCPP(cout, indentation); // lance l'opération traduitEnCPP sur la racine
    cout << setw(4) << "";
    cout << "return 0;" << endl;
    cout << "}" << endl; // Fin d’un programme C++ 
}

int Interpreteur::getVar(string var){
    for (int s = 0; s < m_table.getTaille(); s++) {
        SymboleValue b = m_table[s];
        if (b == "<VARIABLE>" && b.getChaine() == var) {
            return b.executer();
        }
    }
    return 0;
}

void Interpreteur::setVar(string var, int val){
    for (int s = 0; s < m_table.getTaille(); s++) {
        SymboleValue b = m_table[s];
        if (b == "<VARIABLE>" && b.getChaine() == var) {
            b.setValeur(val);
        }
    }
}