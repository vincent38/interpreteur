#include <stdlib.h>
#include <typeinfo>
#include <iostream>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////


NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
        m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
    if (instruction != nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream& cout, unsigned int indentation) const{
    for (auto inst : m_instructions){           //répeter pour toute les instructions
        cout << setw(4*indentation);            // écrie l'instruction avec une indentation de 4*indentation espaces
        inst->traduitEnCPP(cout,indentation);   //                          //
        cout << endl;
    }
}
////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
    int valeur = m_expression->executer(); // On exécute (évalue) l'expression
    ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "";
    m_variable->traduitEnCPP(cout, indentation);
    cout << " = ";
    m_expression->traduitEnCPP(cout, 0);
    cout << ";";
}

void NoeudAffectation::traduitEnCPPNoComma(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "";
    m_variable->traduitEnCPP(cout, indentation);
    cout << " = ";
    m_expression->traduitEnCPP(cout, 0);
}
    

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    // Et on combine les deux opérandes en fonctions de l'opérateur
    if (this->m_operateur == "+") valeur = (og + od);
    else if (this->m_operateur == "-") valeur = (og - od);
    else if (this->m_operateur == "*") valeur = (og * od);
    else if (this->m_operateur == "==") valeur = (og == od);
    else if (this->m_operateur == "!=") valeur = (og != od);
    else if (this->m_operateur == "<") valeur = (og < od);
    else if (this->m_operateur == ">") valeur = (og > od);
    else if (this->m_operateur == "<=") valeur = (og <= od);
    else if (this->m_operateur == ">=") valeur = (og >= od);
    else if (this->m_operateur == "et") valeur = (og && od);
    else if (this->m_operateur == "ou") valeur = (og || od);
    else if (this->m_operateur == "non") valeur = (!og);
    else if (this->m_operateur == "/") {
        if (od == 0) throw DivParZeroException();
        valeur = og / od;
    }
    return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "";
    m_operandeGauche->traduitEnCPP(cout, 0);
    cout << " " << m_operateur.getChaine() << " ";
    m_operandeDroit->traduitEnCPP(cout, 0);
}
    

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
    if (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnCPP(ostream & cout,unsigned int indentation)const{
    cout << setw(4*indentation) << "" << "if (";      // Ecrit "if (" avec un décalage de 4*indentation espaces 
    m_condition->traduitEnCPP(cout,0);                // Traduit la condition en C++ sans décalage 
    cout << ") {" << endl;                            // Ecrit ") {" et passe à la ligne 
    m_sequence->traduitEnCPP(cout, indentation+1);    // Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne 
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while (m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "" << "while (";   // Ecrit "while (" avec un décalage de 4*indentation espaces // les indentations ne se font pas correctement et nous n'avons aucune idée d'où cela peut venir
    m_condition->traduitEnCPP(cout,0);                // Traduit la condition en C++ sans décalage 
    cout << ") {" << endl;                            // Ecrit ") {" et passe à la ligne 
    m_sequence->traduitEnCPP(cout, indentation+1);    // Traduit en C++ la séquence avec indentation augmentée 
    cout << setw(4*indentation) << "" << "}" << endl; // Ecrit "}" avec l'indentation initiale et passe à la ligne 
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
    //while (m_condition->executer()) m_sequence->executer();
    do {
        m_sequence->executer();
    } while (!m_condition->executer());
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "" << "do {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << "" << "} while(";
    m_condition->traduitEnCPP(cout, 0);
    cout << ");" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(Noeud* condition, Noeud* sequence, vector<Noeud*> conditionSinonSi,
        vector<Noeud*> sequenceSinonSi, Noeud* sequenceSinon)
: m_condition(condition), m_sequence(sequence), m_conditionSinonSi(conditionSinonSi),
m_sequenceSinonSi(sequenceSinonSi), m_sequenceSinon(sequenceSinon) {
}

int NoeudInstSiRiche::executer() {
    //while (m_condition->executer()) m_sequence->executer();
    int i = 0;
    bool passageDansLeIf = 0;
    if (m_condition->executer()) {
        m_sequence->executer();
        passageDansLeIf = 1;
    } else if (1) {
        for (auto condi : m_conditionSinonSi) {
            if (condi->executer()) {
                m_sequenceSinonSi[i]->executer();
                break;
            }
            i++;
        }
    }
    if (!passageDansLeIf && i == m_conditionSinonSi.size()) {
        if (m_sequenceSinon != NULL) {
            m_sequenceSinon->executer();
        }
    }
}

void NoeudInstSiRiche::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "" << "if (";
    m_condition->traduitEnCPP(cout, 0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout, indentation+1);
    cout << setw(4*indentation) << "" << "}";
    if(int imax=m_conditionSinonSi.size() != 0){
        int i = 0;
        while(i < imax){
            cout << " else if (";
            m_conditionSinonSi[i]->traduitEnCPP(cout, 0);
            cout << ") {" << endl;
            m_sequenceSinonSi[i]->traduitEnCPP(cout, indentation+1);
            cout << setw(4*indentation) << "" << "}";
            i++;
        }
    }
    if(m_sequenceSinon != NULL){
        cout << " else {" << endl;
        m_sequenceSinon->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}" << endl;
    }
    
}

// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
    status = 0;
}

void NoeudInstPour::ajoute(Noeud* instruction) {
    if (status == 0) {
        m_init = instruction;
        status = 1;
    } else {
        m_incr = instruction;
        status = 0;
    }
}

int NoeudInstPour::executer() {
    //while (m_condition->executer()) m_sequence->executer();
    if (m_init == nullptr and m_incr == nullptr) {
        for (; m_condition->executer();) {
            m_sequence->executer();
        }
    } else if (m_init == nullptr) {
        for (; m_condition->executer(); m_incr->executer()) {
            m_sequence->executer();
        }
    } else if (m_incr == nullptr) {
        for (m_init->executer(); m_condition->executer();) {
            m_sequence->executer();
        }
    } else {
        for (m_init->executer(); m_condition->executer(); m_incr->executer()) {
            m_sequence->executer();
        }
    }

  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstPour::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    if (m_init == nullptr and m_incr == nullptr) {
        cout << setw(4*indentation) << "" << "for (";
        cout << "; ";
        m_condition->traduitEnCPP(cout,0);
        cout << "; ";
        cout << ") {" << endl;
        m_sequence->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}" << endl;
    } else if (m_init == nullptr) {
        cout << setw(4*indentation) << "" << "for (";
        cout << "; ";
        m_condition->traduitEnCPP(cout,0);
        cout << "; ";
        m_incr->traduitEnCPPNoComma(cout,0);
        cout << ") {" << endl;
        m_sequence->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}" << endl;
    } else if (m_incr == nullptr) {
        cout << setw(4*indentation) << "" << "for (";
        m_init->traduitEnCPP(cout, 0);
        m_condition->traduitEnCPP(cout,0);
        cout << "; ";
        cout << ") {" << endl;
        m_sequence->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}" << endl;
    } else {
        cout << setw(4*indentation) << "" << "for (";
        m_init->traduitEnCPP(cout, 0);
        m_condition->traduitEnCPP(cout,0);
        cout << "; ";
        m_incr->traduitEnCPPNoComma(cout,0);
        cout << ") {" << endl;
        m_sequence->traduitEnCPP(cout, indentation+1);
        cout << setw(4*indentation) << "" << "}" << endl;
    }
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<SymboleValue*> vars) : m_vars(vars){}

int NoeudInstLire::executer() {
  
    int a = 0;
    for(auto v : m_vars){
        cin >> a;
        v->setValeur(a);
    }
    
  return 0; // La valeur renvoyée ne représente rien !
}
void NoeudInstLire::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    
    int i = 0;

    cout << setw(4*indentation) << "" << "cin" ;
    while (i < m_vars.size()){
        cout << " >> ";
        m_vars[i]->traduitEnCPP(cout,0);
        i++;
    }
    cout << ";" << endl;
    
}

// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire() {

}

void NoeudInstEcrire::ajoute(Noeud* instruction) {
    m_toPrint.push_back(instruction);
}

int NoeudInstEcrire::executer() {
    for (auto i : m_toPrint) {
        if (typeid (*i) == typeid (SymboleValue) && *((SymboleValue*) i) == "<CHAINE>") {
            //Chaîne de caractères
            cout << ((SymboleValue*) i)->getChaine();
        } else {
            //Autre
            cout << i->executer();
        }
    }
    cout << endl;
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    int i = 0;
    
    cout << setw(4*indentation) << "" << "cout" ;
    while (i < m_toPrint.size()){
        cout << " << ";
        m_toPrint[i]->traduitEnCPP(cout, 0);
        i++;
    }
    cout << " << endl;" << endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudPreIncrementation
////////////////////////////////////////////////////////////////////////////////

NoeudPreIncrementation::NoeudPreIncrementation(SymboleValue* variable)
: m_variable(variable) {
}

int NoeudPreIncrementation::executer() {
    int buff = m_variable->executer();
    m_variable->setValeur(++buff);
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudPreIncrementation::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "" << "++";
    m_variable->traduitEnCPP(cout, indentation);
    cout << ";";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudPreDecrementation
////////////////////////////////////////////////////////////////////////////////

NoeudPreDecrementation::NoeudPreDecrementation(SymboleValue* variable)
: m_variable(variable) {
}

int NoeudPreDecrementation::executer() {
    int buff = m_variable->executer();
    m_variable->setValeur(--buff);
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudPreDecrementation::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "" << "--";
    m_variable->traduitEnCPP(cout, indentation);
    cout << ";";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudPostIncrementation
////////////////////////////////////////////////////////////////////////////////

NoeudPostIncrementation::NoeudPostIncrementation(SymboleValue* variable)
: m_variable(variable) {
}

int NoeudPostIncrementation::executer() {
    int buff = m_variable->executer();
    m_variable->setValeur(buff++);
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudPostIncrementation::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "";
    m_variable->traduitEnCPP(cout, indentation);
    cout << "++" << ";";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudPostDecrementation
////////////////////////////////////////////////////////////////////////////////

NoeudPostDecrementation::NoeudPostDecrementation(SymboleValue* variable)
: m_variable(variable) {
}

int NoeudPostDecrementation::executer() {
    int buff = m_variable->executer();
    m_variable->setValeur(buff--);
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudPostDecrementation::traduitEnCPP(ostream & cout, unsigned int indentation) const{
    cout << setw(4*indentation) << "";
    m_variable->traduitEnCPP(cout, indentation);
    cout << "--" << ";";
}