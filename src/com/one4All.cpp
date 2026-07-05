/*
 * ===========================================================================================================
 * one4All.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#include <tuple>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_primitives.hpp>
#include "one4All.h"
#include "OS_Specific/OS_com.h"
#include <sstream>
#include <boost/tuple/tuple_comparison.hpp>

namespace com {
//============================================================================================================
bool isSubDirectory ( const sambag::com::Location &a,  const sambag::com::Location &b )
{
    using namespace std::filesystem;
    if (equivalent(a, b)) {
        return false;
    }
    path p = b;
    while (!p.empty()) {
        if (equivalent(p, a)) {
            return true;
        }
        p = p.parent_path();
    }
    return false;
}
//------------------------------------------------------------------------------------------------------------
std::pair<std::string, std::string> extractVSTPluginFilename(const std::string &filename) {
	using namespace boost::xpressive;
	mark_tag tFilename(1), tShellid(2);
	// (.+?)@{0,1}\d+$
	cregex pat = (tFilename= -+_) >> "@" >> (tShellid= +set[range('a','z')|range('A','Z')|range('0','9')|'-']) >> eol;
	cmatch what;
	if(regex_search(filename.c_str(), what, pat)) { // matches filename@number
		return std::make_pair(
			what[tFilename],
			what[tShellid]
		);
	}
	// no match: return whole string
	return std::make_pair(filename, "");
}
//------------------------------------------------------------------------------------------------------------
std::string createVSTPluginFilename(const std::string &filename, const std::string &shellId) {
	if (shellId == "")
		return filename;
	std::stringstream ss;
	ss<<filename<<"@"<<shellId;
	return ss.str();
}
//------------------------------------------------------------------------------------------------------------
std::string getFileNameFromPath( const std::string &fileName ) {
	std::filesystem::path p(fileName);
	return std::string( p.stem().string() );
}
//============================================================================================================
// class IdParser
//============================================================================================================
//------------------------------------------------------------------------------------------------------------

#define FRX_IDPARSER_NAME set[range('a','z')|range('A','Z')|range('0','9')|'-']

IdParser::IdParser(const std::string &str) :
     Data( FRX_NULL_ID.data() )
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
    
    cregex pat = "frx." >> (tNameSpace=+FRX_IDPARSER_NAME) >> "." >> (tType=+FRX_IDPARSER_NAME) >> "." >>
                 (tName=+FRX_IDPARSER_NAME) >> *("(" >> *_s >> (tNumI= +_d) >>
                 *_s >> "," >> *_s >> (tNumO= +_d) >> *_s >> ")" |
                 ("(" >> *_s >> "'" >> (tDetails=-*_) >> "'" >> *_s >> ")") ) >>
                 eol;
                  
	cmatch what;
	if(!regex_search(str.c_str(), what, pat)) {
        data( FRX_NULL_ID.data() );
        return;
    }
    namespace_(what[tNameSpace]);
    type(what[tType]);
    name(what[tName]);
    if (what.size() >= 5) {
        std::stringstream ss;
        ss<<what[tNumI]<<" "<<what[tNumO];
        ss>>std::get<3>(*this)>>std::get<4>(*this);
    }
    if (what.size() >= 8) {
        details(what[tDetails]);
    }
}
//------------------------------------------------------------------------------------------------------------
std::string IdParser::toString() const {
    std::stringstream ss;
    ss<<*this;
    return ss.str();
}
//------------------------------------------------------------------------------------------------------------
bool IdParser::operator==(const IdParser &descr) const {
    return this->data() == descr.data();
}
//------------------------------------------------------------------------------------------------------------
bool IdParser::operator!=(const IdParser &descr) const {
    return !(*this == descr);
}
//------------------------------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream &os, const IdParser &pd) {
    if ( pd == FRX_NULL_ID ) {
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
const IdParser FRX_NULL_ID = IdParser("","","",-1,-1,"");
} // namespace com



