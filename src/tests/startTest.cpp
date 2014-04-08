
#ifdef FRX_OS_WINDOWS
	#define WIN32ONLY(x) x
	#include <crtdbg.h>
	void* hInstance=NULL;
#else
	#define WIN32ONLY(x)
#endif

#include <iostream>
#include "tests/MyStringTest.hpp"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/ui/text/TestRunner.h>
#include "com/one4All.h"
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include "com/Settings.h"
#include <stdlib.h>
#include <OS_Specific/OS_com.h>
#include <sambag/com/UnitEx.hpp>

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")

//=============================================================================
//                       CPPUnit TestApp fuer VSTForx
//=============================================================================
int main ( const int argc, char **argv ) {

	::com::initSettings(".");
    
    sambag::com::unitex::processArguments(argc, argv);
    
	WIN32ONLY(
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); //VS memory tracking
	)
	using namespace std;
	using namespace tests;
	cout<<"* VSTForx TestApp *"<<endl;
	cout<<"testing version: " << SETTINGS.versionToString() << endl;
	cout<<"==================================="<<endl;
    if (sambag::com::unitex::ignoreKnownIssues) {
        cout<<"! ignoring known issues !"<<endl;
    }
	
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

    // important stuff happens next
   std::ofstream xmlFileOut("vstforx_testresult.xml");
   CPPUNIT_NS::XmlOutputter xmlOut(&collectedresults, xmlFileOut);
   xmlOut.write();

	return 0;
}
