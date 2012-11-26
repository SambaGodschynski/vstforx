/*
 * TooltipTexts.hpp
 *
 *  Created on: Mon Nov 26 14:16:18 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_TOOLTIPTEXTS_H
#define SAMBAG_TOOLTIPTEXTS_H

#include <boost/shared_ptr.hpp>
#include <loki/LokiTypeinfo.h>
#include <string>
#include <map>

namespace frx { namespace gui {
namespace components { namespace ui { 
//=============================================================================
/** 
  * @class TooltipTexts.
  */
class TooltipTexts {
//=============================================================================
protected:
	//-------------------------------------------------------------------------
	typedef std::map<Loki::TypeInfo, std::string> TypeMap;
	//-------------------------------------------------------------------------
	static TypeMap typeMap;
	//-------------------------------------------------------------------------
	static void initMap();
	//-------------------------------------------------------------------------
	static const std::string & _getTooltipText(const Loki::TypeInfo &typeinfo);
	//-------------------------------------------------------------------------
public:
	template <class T>
	static const std::string & getText(const T &obj) {
		return _getTooltipText( Loki::TypeInfo(typeid(obj)) );
	}
}; // TooltipTexts
}}}} // namespace(s)

#endif /* SAMBAG_TOOLTIPTEXTS_H */
