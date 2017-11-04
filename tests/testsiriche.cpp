/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.<
 */

/*
 * File:   testsiriche.cpp
 * Author: aubriotv
 *
 * Created on Nov 3, 2017, 2:45:08 PM
 */

#include "testsiriche.h"
#include "../Interpreteur.h"
#include "../Exceptions.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testsiriche);

testsiriche::testsiriche() {

}

testsiriche::~testsiriche() {
}

void testsiriche::setUp() {
}

void testsiriche::tearDown() {
}

void testsiriche::testMethod() {
    //Mise en place analyse fichier 1
    ifstream fichier("testsiriche.txt");
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    if (interpreteur.getArbre()!=nullptr) interpreteur.getArbre()->executer();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test1 differs from expected value!", 1, interpreteur.getVar("test1"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test2 differs from expected value!", 1, interpreteur.getVar("test2"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("test3 differs from expected value!", 1, interpreteur.getVar("test3"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("autre differs from expected value!", 1, interpreteur.getVar("autre"));
}

void testsiriche::testFailedMethod() {
    //Mise en place analyse fichier 1
    ifstream fichier("testsirichesyntaxefausse.txt");
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    CPPUNIT_ASSERT_MESSAGE("tree is not empty!", interpreteur.getArbre()==nullptr);
}

