#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

//class SymboleValue;

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
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

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
  //while (m_condition->executer()) m_sequence->executer();
    do{
        m_sequence->executer();
    }  
    while (!m_condition->executer());
  return 0; // La valeur renvoyée ne représente rien !
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
    if(m_condition->executer()){
        m_sequence->executer();
        passageDansLeIf = 1;
    } else if (1) {
        for (auto condi : m_conditionSinonSi){
            if(condi->executer()){
                m_sequenceSinonSi[i]->executer();
                break;
            }
            i++;
        }
    } if(!passageDansLeIf && i == m_conditionSinonSi.size()) {
        if(m_sequenceSinon != NULL){
            m_sequenceSinon->executer();
        }   
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
        for (; m_condition->executer(); ) {
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