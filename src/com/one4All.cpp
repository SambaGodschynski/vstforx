#include "one4All.h"
#include "OS_Specific/OS_com.h"


namespace com {
//============================================================================================================
bool isSubDirectory ( const Path &parent,  const Path &sub ) {
	string p = parent.string();
	string s = sub.string();
	if ( p==s ) return false;
	return s.find (p) != string::npos;
}
} // namespace com