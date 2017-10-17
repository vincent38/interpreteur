#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable

  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)

  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction tantque"
//  et ses 2 fils : la condition du tantque et la séquence d'instruction associée
  public:
    NoeudInstTantQue(Noeud* condition, Noeud* sequence);
     // Construit une "instruction tantque" avec sa condition et sa séquence d'instruction
   ~NoeudInstTantQue() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction tantque : tantque condition vraie on exécute la séquence

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud {
// Classe pour représenter un noeud "instruction repeter"
//  et ses 2 fils : la condition du repeter et la séquence d'instruction associée
  public:
    NoeudInstRepeter(Noeud* condition, Noeud* sequence);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstRepeter() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction repeter : repeter la séquence jusqu'a ce que la condition soit vraie 

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSiRiche : public Noeud {
// Classe pour représenter un noeud "instruction repeter"
//  et ses 2 fils : la condition du repeter et la séquence d'instruction associée
  public:
    NoeudInstSiRiche(Noeud* condition, Noeud* sequence, vector<Noeud*> conditionSinonSi,
            vector<Noeud*> sequenceSinonSi, Noeud* sequenceSinon);
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstSiRiche() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction repeter : repeter la séquence jusqu'a ce que la condition soit vraie 
    
  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
    vector<Noeud*> m_conditionSinonSi;
    vector<Noeud*> m_sequenceSinonSi;
    Noeud*  m_sequenceSinon;
};

class NoeudInstPour : public Noeud {
// Classe pour représenter un noeud "instruction pour"
//  et ses 2 à 4 fils : la condition du pour et la séquence d'instruction associée
  public:
    NoeudInstPour(Noeud* condition, Noeud* sequence);
     // Construit une "instruction pour" avec sa condition et sa séquence d'instruction
   ~NoeudInstPour() {} // A cause du destructeur virtuel de la classe Noeud
    int executer() override;  // Exécute l'instruction pour : répéter la séquence jusqu'a ce que la condition soit vraie 
    void ajoute(Noeud* instruction);  // Ajoute une initialisation

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
    Noeud*  m_init;
    Noeud*  m_incr;
    int     status;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud {
// Classe pour représenter un noeud "instruction écrire"
  public:
    NoeudInstEcrire();
     // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction
   ~NoeudInstEcrire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction ecrire : ecrit la phrase au tableau
    void ajoute(Noeud* instruction);  // Ajoute un élément à écrire
    
  private:
    vector<Noeud*> m_toPrint;
};

#endif /* ARBREABSTRAIT_H */
