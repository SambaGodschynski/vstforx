/*
 * ===========================================================================================================
 * one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <boost/xpressive/xpressive.hpp>
#include "one4All.h"
#include "OS_Specific/OS_com.h"
#include <sstream>


namespace com {
//============================================================================================================
bool isSubDirectory ( const sambag::com::Location &a,  const sambag::com::Location &b )
{
    using namespace boost::filesystem;
    if (equivalent(a, b)) {
        return false;
    }
    boost::filesystem::path path = b;
    while (!path.empty()) {
        if (equivalent(path, a)) {
            return true;
        }
        path = path.parent_path();
    }
    return false;
}
//------------------------------------------------------------------------------------------------------------
std::pair<std::string, int> extractVSTPluginFilename(const std::string &filename) {
	using namespace boost::xpressive;
	mark_tag tFilename(1), tShellid(2);
	// (.+?)@{0,1}\d+$
	cregex pat = (tFilename= -+_) >> "@" >> (tShellid= +_d) >> eol;
	cmatch what;
	if(regex_search(filename.c_str(), what, pat)) { // matches filename@number
		std::stringstream ss;
		int shellId;
		ss << what[tShellid];
		ss >> shellId;
		return std::make_pair(
			what[tFilename],
			shellId
		);
	}
	// no match: return whole string
	return std::make_pair(filename, 0);
}
//------------------------------------------------------------------------------------------------------------
std::string createVSTPluginFilename(const std::string &filename, int shellId) {
	if (shellId == 0)
		return filename;
	std::stringstream ss;
	ss<<filename<<"@"<<shellId;
	return ss.str();
}
//------------------------------------------------------------------------------------------------------------
ProcessorDescriptor extractProcessorDescriptor(const std::string &str) {
    // (\w+).(\w+).(\w+)(\( *(\d+) *, *(\d+) *\) *)*$
    
    using namespace boost::xpressive;
	mark_tag tType(1), tName(2), tConf(3), tNumI(4), tNumO(5), tFrx(6), tNameSpace(7);
	cregex pat = (tFrx=+alnum) >> "." >> (tNameSpace=+alnum) >> "." >> (tType=+alnum) >> "." >>
                 (tName=+alnum) >> *("(" >> *_s >> (tNumI= +_d) >>
                 *_s >> "," >> *_s >> (tNumO= +_d) >> *_s >> ")") >> eol;
                  
	cmatch what;
	if(regex_search(str.c_str(), what, pat)) {
        int a=-1,b=-1;
        if (what.size() >= 4) {
            std::stringstream ss;
            ss<<what[tNumI]<<" "<<what[tNumO];
            ss>>a>>b;
        }
		return ProcessorDescriptor(what[tType], what[tName], a, b);
	}
    return FRX_NULL_PROCESSOR;
}
//------------------------------------------------------------------------------------------------------------
const ProcessorDescriptor FRX_NULL_PROCESSOR = ProcessorDescriptor("","",-1,-1);
} // namespace com