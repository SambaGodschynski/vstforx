/*
 * BridgeSessionManager.hpp
 *
 *  Created on: Mon Dec  2 10:48:48 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_BRIDGESESSIONMANAGER_H
#define SAMBAG_BRIDGESESSIONMANAGER_H

#include <loki/Singleton.h>
#include <string>
#include "SessionManager.hpp"

namespace frx { namespace processing { namespace interprocess {
//=============================================================================
/** 
  * @class BridgeSessionManager.
  */
class BridgeSessionManager : public SessionManager {
//=============================================================================
friend struct Loki::CreateUsingNew<BridgeSessionManager>;
public:
protected:
    //-------------------------------------------------------------------------
    BridgeSessionManager();
private:
    //-------------------------------------------------------------------------
    std::string path;
public:
	//-------------------------------------------------------------------------
	static BridgeSessionManager & instance();
    //-------------------------------------------------------------------------
    void setHostPath(const std::string &path);
    //-------------------------------------------------------------------------
    const std::string & getHostPath() const;
    //-------------------------------------------------------------------------
    
}; // BridgeSessionManager
}}} // namespace(s)

#endif /* SAMBAG_BRIDGESESSIONMANAGER_H */
