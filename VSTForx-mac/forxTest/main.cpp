#include <iostream>
#include "tests/MyStringTest.hpp"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "com/one4All.h"
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include "com/Settings.h"
#include <stdlib.h>

//---------------------------------------------------------------------------------------
std::string getHomeDirectory() {
	return "";
}


//=============================================================================
//                       CPPUnit TestApp fuer VSTForx
//=============================================================================
int main ( const int argc, char **argv ) {
	using namespace std;
	using namespace tests;
	cout<<"* VSTForx TestApp *"<<endl;
	cout<<"testing version: " << com::Settings::versionToString() << endl;
	cout<<"==================================="<<endl;
	
	// Informiert Test-Listener ueber Testresultate
    CPPUNIT_NS :: TestResult testresult;
	
    // Listener zum Sammeln der Testergebnisse registrieren
    CPPUNIT_NS :: TestResultCollector collectedresults;
    testresult.addListener (&collectedresults);
	
    // Listener zur Ausgabe der Ergebnisse einzelner Tests
    CPPUNIT_NS :: BriefTestProgressListener progress;
    testresult.addListener (&progress);
	
    // Test-Suite ueber die Registry im Test-Runner einfuegen
    CPPUNIT_NS :: TestRunner testrunner;
    testrunner.addTest (CPPUNIT_NS :: TestFactoryRegistry :: getRegistry ().makeTest ());
    testrunner.run (testresult);
	
    // Resultate im Compiler-Format ausgeben
    CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults, std::cerr);
    compileroutputter.write ();
	
	
	// Return error code 1 if the one of test failed.
	return collectedresults.wasSuccessful() ? 0 : 1;
}