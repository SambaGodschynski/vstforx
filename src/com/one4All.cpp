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
#include <boost/tuple/tuple_comparison.hpp>

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
//============================================================================================================
// class Descriptor
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
Descriptor::Descriptor(const std::string &str) :
     Data( FRX_NULL_PROCESSOR.data() )
{
    if (str.length()==0) {
        return;
    }
    // frx.(\w+).(\w+)(\( *(\d+) *, *(\d+) *\)*|\( *'(.*?)' *\))*$
    using namespace boost::xpressive;
	mark_tag tType(1),
             tName(2),
             tConf(3),
             tNumI(4),
             tNumO(5),
             tFrx(6),
             tNameSpace(7),
             tDetails(8);
	cregex pat = "frx." >> (tNameSpace=+alnum) >> "." >> (tType=+alnum) >> "." >>
                 (tName=+alnum) >> *("(" >> *_s >> (tNumI= +_d) >>
                 *_s >> "," >> *_s >> (tNumO= +_d) >> *_s >> ")" |
                 ("(" >> *_s >> "'" >> (tDetails=-*_) >> "'" >> *_s >> ")") ) >>
                 eol;
                  
	cmatch what;
	if(!regex_search(str.c_str(), what, pat)) {
        data( FRX_NULL_PROCESSOR.data() );
        return;
    }
    namespace_(what[tNameSpace]);
    type(what[tType]);
    name(what[tName]);
    if (what.size() >= 5) {
        std::stringstream ss;
        ss<<what[tNumI]<<" "<<what[tNumO];
        ss>>boost::get<3>(*this)>>boost::get<4>(*this);
    }
    if (what.size() >= 8) {
        details(what[tDetails]);
    }
}
//------------------------------------------------------------------------------------------------------------
std::string Descriptor::toString() const {
    std::stringstream ss;
    ss<<*this;
    return ss.str();
}
//------------------------------------------------------------------------------------------------------------
bool Descriptor::operator==(const Descriptor &descr) const {
    return this->data() == descr.data();
}
//------------------------------------------------------------------------------------------------------------
bool Descriptor::operator!=(const Descriptor &descr) const {
    return !(*this == descr);
}
//------------------------------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream &os, const Descriptor &pd) {
    if ( pd == FRX_NULL_PROCESSOR ) {
        return os;
    }
    os<<"frx."<<pd.namespace_()<<"."<<pd.type()<<"."<<pd.name();
    if (pd.details().length() > 0) {
        os<<"('"<<pd.details()<<"')";
        return os;
    }
    if (pd.numInputs()>=0 || pd.numOutputs()>=0) {
        os<<"("<<pd.numInputs()<<", "<<pd.numOutputs()<<")";
    }
    return os;
}
//------------------------------------------------------------------------------------------------------------
const Descriptor FRX_NULL_PROCESSOR = Descriptor("","","",-1,-1,"");
} // namespace com



