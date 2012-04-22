/*
 * ===========================================================================================================
 * Log.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "Log.h"
#include <boost/thread.hpp>
#include "One4All.h"
#include <ostream>
#include <iostream>
#include <sys/stat.h>
static com::Mutex mutex;

using namespace std;
//------------------------------------------------------------------------------------------------------------
namespace com{
//------------------------------------------------------------------------------------------------------------
int LogFile::bffSize = 1;
//------------------------------------------------------------------------------------------------------------
LogFile::LogFile() { 
	filename = Settings::getLogFilename();
	struct stat filestatus;
	stat( filename.c_str(), &filestatus );
	if ( filestatus.st_size > SETTINGS->getMaxLogSize() ) std::remove ( filename.c_str() );
}
//------------------------------------------------------------------------------------------------------------
void LogFile::flushBff(){
}
//------------------------------------------------------------------------------------------------------------
void LogFile::writeIn(const std::string &str){
	TRY_TO_LOCK_TIMED(mutex);  //vor nebenlaeufigen zugriffen schuetzen
	static LogFile once;
	time_t t = time( 0 );
	tm *nun;
	nun = localtime(&t);
	ofstream os;
	os.open( once.filename.c_str(), ios::app );
	if ( os.fail() ) 
		return;

	os.width (1);
	os.fill ('0');
	os.setf ( ios::left, ios::adjustfield );
	os << "<" << nun->tm_hour << ":" << nun->tm_min << ":" << nun->tm_sec << "> " << str << std::endl;
	
	os.close();
}
//------------------------------------------------------------------------------------------------------------
LogFile::~LogFile() {
	TRY_TO_LOCK_TIMED(mutex);  //vor nebenlaeufigen zugriffen schuetzen
}
} // namespace com