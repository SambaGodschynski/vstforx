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
#include <boost/shared_ptr.hpp>
#include <processing/IHostInfo.h>
#include <processing/PlugInfo.h>
#include <sambag/com/ArithmeticWrapper.hpp>

namespace frx { namespace processing { namespace interprocess {
class BridgeSessionClient;
typedef boost::shared_ptr<BridgeSessionClient> BridgeSessionClientPtr;
class PluginSessionClient;
typedef boost::shared_ptr<PluginSessionClient> PluginSessionClientPtr;
//=============================================================================
/** 
  * @class BridgeSessionManager.
  * @note no shm in here. Handles initiating the BridgeSession
  * and is an adapter for its operations.
  */
class BridgeSessionManager {
//=============================================================================
friend struct Loki::CreateUsingNew<BridgeSessionManager>;
public:
protected:
    //-------------------------------------------------------------------------
    BridgeSessionManager();
private:
    //-------------------------------------------------------------------------
    BridgeSessionClientPtr ___bridge_;
    //-------------------------------------------------------------------------
    std::string path;
    //-------------------------------------------------------------------------
    void startBridge();
    //-------------------------------------------------------------------------
    /**
     * @note not public because bridge closes automtically and to make managing
     * a bit easyier we want be the only instance who have a bridge session ptr.
     */
    BridgeSessionClientPtr getBridgeClient();
    //-------------------------------------------------------------------------
    void onHostClosing();
    //-------------------------------------------------------------------------
    void onHostClosingAsync();
public:
    //-------------------------------------------------------------------------
    sambag::com::ArithmeticWrapper<bool, false>
        __BridgeSessionManager_private_isBridge;
    //-------------------------------------------------------------------------
    /**
     * @return true when current application is the bridge itself
     */
    bool isBridge() {
        return __BridgeSessionManager_private_isBridge;
    }
	//-------------------------------------------------------------------------
	static BridgeSessionManager & instance();
    //-------------------------------------------------------------------------
    void setBridgePath(const std::string &path);
    //-------------------------------------------------------------------------
    const std::string & getBridgePath() const {
        return path;
    }
    //-------------------------------------------------------------------------
    std::string getBridgeSessionId() const;
    //-------------------------------------------------------------------------
    bool isBridgeSessionEstabished() const;
    //-------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////
    //-------------------------------------------------------------------------
    PluginSessionClientPtr createPluginSession(IHostInfo::Ptr hI,
        const ::processing::PluginInfo &pI);
    //-------------------------------------------------------------------------
    PluginSessionClientPtr createPluginSession(const std::string &path,
        float sampleRate, int blockSize);
    //-------------------------------------------------------------------------
    void closePluginSession(PluginSessionClientPtr session);
}; // BridgeSessionManager
}}} // namespace(s)

#endif /* SAMBAG_BRIDGESESSIONMANAGER_H */
