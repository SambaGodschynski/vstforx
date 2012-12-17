#include <iostream>
#include "tests/MyStringTest.hpp"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "windows.h"
#include "com/one4All.h"
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include "com/Settings.h"
#include <stdlib.h>
#include <crtdbg.h>

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")

//---------------------------------------------------------------------------------------
std::string getHomeDirectory() {
	using namespace com;
	std::string home_dir;
	const size_t N = 512; 
	char _d[N];
	DWORD r = GetModuleFileName ( NULL, &_d[0], N );
	com::Filename f( _d  );
	if ( is_regular_file(f) ) home_dir = f.remove_filename().string();
	else home_dir = f.string();
	return home_dir;
}


//=============================================================================
//                       CPPUnit TestApp fuer VSTForx
//=============================================================================
int main ( const int argc, char **argv ) {

	::com::initSettings(getHomeDirectory());

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
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