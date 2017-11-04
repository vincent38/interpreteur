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


CPPUNIT_TEST_SUITE_REGISTRATION(testsiriche);

testsiriche::testsiriche() {
    //Mise en place analyse fichier 1
    Interpreteur interpreteur("testsiriche.txt");
    interpreteur.analyse();
    if (interpreteur.getArbre()!=nullptr) interpreteur.getArbre()->executer();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("OK");
}

testsiriche::~testsiriche() {
}

void testsiriche::setUp() {
}

void testsiriche::tearDown() {
}

void testsiriche::testMethod() {
    CPPUNIT_ASSERT(true);
}

void testsiriche::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}

