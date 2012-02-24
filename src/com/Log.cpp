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
LogFile::Ptr LogFile::once;
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
	list<MyString>::iterator it = buffer.begin();
	ofstream f;
	f.open( filename.c_str(), ios::app );
	if ( f.fail() ) return;
	while ( it != buffer.end() ){
		f << *it << endl;
		++it;
	}
	f.close();
	buffer.clear();
}
//------------------------------------------------------------------------------------------------------------
void LogFile::writeIn(const std::string &str){
	TRY_TO_LOCK_TIMED(mutex);  //vor nebenlaeufigen zugriffen schuetzen
	if ( !once ) once = LogFile::Ptr ( new LogFile() );
	time_t t = time( 0 );
	tm *nun;
	nun = localtime(&t);
	ostringstream os;
	os.width (1);
	os.fill ('0');
	os.setf ( ios::left, ios::adjustfield );
	os << "<" << nun->tm_hour << ":" << nun->tm_min << ":" << nun->tm_sec << "> " << str;
	// wurde LogFile schon released ist bffSize == -1
	once->buffer.push_back ( os.str() );
	if ( once->buffer.size() == once->bffSize ) once->flushBff();
}
//------------------------------------------------------------------------------------------------------------
LogFile::~LogFile() {
	TRY_TO_LOCK_TIMED(mutex);  //vor nebenlaeufigen zugriffen schuetzen
}
} // namespace com