#include <iostream>
using namespace std;
#include "Interpreteur.h"
#include "Exceptions.h"

int main(int argc, char* argv[]) {
  string nomFich;
  if (argc != 2) {
    cout << "Usage : " << argv[0] << " nom_fichier_source" << endl << endl;
    cout << "Entrez le nom du fichier que voulez-vous interpréter : ";
    getline(cin, nomFich);
  } else
    nomFich = argv[1];
  ifstream fichier(nomFich.c_str());
  try {
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    if (interpreteur.getArbre()!=nullptr) {
        // Si pas d'exception levée, l'analyse syntaxique a réussi
        cout << endl << "================ Syntaxe Correcte" << endl;
        // On affiche le contenu de la table des symboles avant d'exécuter le programme
        cout << endl << "================ Table des symboles avant exécution : " << interpreteur.getTable();
        cout << endl << "================ Execution de l'arbre" << endl;
        // On exécute le programme si l'arbre n'est pas vide
        cout << "arbre : " << interpreteur.getArbre() << endl;
        interpreteur.getArbre()->executer();
    } else {
        throw "Erreurs de syntaxe détectées - Impossible de poursuivre l'exécution et la compilation";
    }
    // Et on vérifie qu'il a fonctionné en regardant comment il a modifié la table des symboles
    cout << endl << "================ Table des symboles apres exécution : " << interpreteur.getTable();
    
    fichier.close();
    
    string nomCPP;                              /////////partie traduction en c++
    cout << "nom du fichier en c++: ";
    cin >> nomCPP;
    nomCPP = nomCPP+".cpp";
    ofstream cout(nomCPP);
    interpreteur.traduitEnCPP(cout, 1);
    
    
  } catch (const char* e) {
    cout << e << endl;
  }
  
  
  
  return 0;
}
