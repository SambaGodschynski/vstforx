/*
 * ===========================================================================================================
 * Log.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef LOGGING_H
#define LOGGING_H

#include <list>
#include <string>
#include "MyString.h"
#include "boost/shared_ptr.hpp"
#include <fstream>
#include <time.h>
#include "Settings.h"

using namespace std;

#define LOGFILE getLogFileName()


// fuegt string x ins log datei ein. -- NUR DEBUG -- 
/*#ifdef NDEBUG                                     // TODO: fuer beta release auskommentieren
#define TOLOG(X)
#define LOG_ASSERT(X)
#define RETURN_NAME(x) return(x)
#else*/
#define TOLOG(X) ::com::LogFile::writeIn ( (X) )
#define LOG_ASSERT(X) if (!(X)) TOLOG("!ASSERTION ERROR in File:" + MyString (__FILE__) + " ln:" + MyString (__LINE__) );
#define RETURN_NAME(x) return (x) //+ "<" + MyString( getOID().name ) + ">"
//#endif

namespace com {
//============================================================================================================
// class LogFile.
// replace with serious log framework
//============================================================================================================
class LogFile {
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<LogFile> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	list<com::MyString> buffer;
	//--------------------------------------------------------------------------------------------------------
	string filename;
	//--------------------------------------------------------------------------------------------------------
	static int bffSize;
	//--------------------------------------------------------------------------------------------------------
	static Ptr once;
	//--------------------------------------------------------------------------------------------------------
	LogFile ();
public:
	//--------------------------------------------------------------------------------------------------------
	static void writeIn ( const string &str );
	//--------------------------------------------------------------------------------------------------------
	void flushBff();
	//--------------------------------------------------------------------------------------------------------
	/*static void setBufferSize ( int size ){
		bffSize = size;
	}*/
	//--------------------------------------------------------------------------------------------------------
	virtual ~LogFile();
};
//============================================================================================================
// class MethodMessage.
// Schreibt in Log: start X ( konstr. )
//                  end X ( destr. )
//============================================================================================================
template < class T >
class MethodMessage {
	//--------------------------------------------------------------------------------------------------------
	string methodName;
	//--------------------------------------------------------------------------------------------------------
	string className;
public:
	//--------------------------------------------------------------------------------------------------------
	MethodMessage ( const string &methodName ) : methodName(methodName) {
		className = string( typeid(T).name() );
		TOLOG ( "start<" + className + "> " + methodName ); 
	}
	//--------------------------------------------------------------------------------------------------------
	// ermittelt klassenname in anhand des src ptr. ( stichwort vererbung )
	MethodMessage ( const string &methodName, const T & src ) : methodName(methodName) {
		className = string( typeid(src).name() );
		TOLOG ( "start<" + className + "> " + methodName ); 
	}
	//--------------------------------------------------------------------------------------------------------
	~MethodMessage () {
		TOLOG ( "end<" + className + "> " + methodName ); 
	}
};
} //namespace com

#endif



