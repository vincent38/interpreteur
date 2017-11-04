/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   testsiriche.h
 * Author: aubriotv
 *
 * Created on Nov 3, 2017, 2:45:07 PM
 */

#ifndef TESTSIRICHE_H
#define TESTSIRICHE_H

#include <cppunit/extensions/HelperMacros.h>

class testsiriche : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testsiriche);

    CPPUNIT_TEST(testMethod);
    CPPUNIT_TEST(testFailedMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    testsiriche();
    virtual ~testsiriche();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif /* TESTSIRICHE_H */

