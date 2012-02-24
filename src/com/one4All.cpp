/*
 * ===========================================================================================================
 * one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include "one4All.h"
#include "OS_Specific/OS_com.h"


namespace com {
//============================================================================================================
bool isSubDirectory ( const sambag::com::Location &parent,  const sambag::com::Location &sub ) {
	string p = parent.string();
	string s = sub.string();
	if ( p==s ) return false;
	return s.find (p) != string::npos;
}
} // namespace com