/*
 * RemoteChannelManager.hpp
 *
 *  Created on: Sat Sep 14 09:23:31 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_REMOTECHANNELMANAGER_H
#define SAMBAG_REMOTECHANNELMANAGER_H

#include <loki/Singleton.h>
#include "Stream.hpp"
#include <time.h>
#include <sambag/com/Interprocess.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/foreach.hpp>

namespace frx { namespace processing { namespace interprocess {
typedef sambag::com::interprocess::String::Class IPString;
namespace bi = boost::interprocess;
namespace si = sambag::com::interprocess;
//=============================================================================
/** 
  * @class RemoteChannelManager<Singleton>.
  *
  * Manages between RemoteChannels/Receiver and it's related RCHandler.
  */
class RemoteChannelManager {
//=============================================================================
friend struct Loki::CreateUsingNew<RemoteChannelManager>;
public:
    //-------------------------------------------------------------------------
    typedef std::string RCId;
    //-------------------------------------------------------------------------
    typedef boost::tuple<std::string> RCData;
private:
    //-------------------------------------------------------------------------
    typedef si::String::Class SHM_String;
    //-------------------------------------------------------------------------
    typedef SHM_String SHM_RCId;
    //-------------------------------------------------------------------------
    // name, streamId
    typedef boost::tuple<SHM_String> SHM_RCData;
    //-------------------------------------------------------------------------
    typedef si::Map<SHM_RCId, SHM_RCData> RemoteChannels;
    //-------------------------------------------------------------------------
    time_t *changed;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::interprocess_mutex Mutex;
    //-------------------------------------------------------------------------
    Mutex *mutex;
    //-------------------------------------------------------------------------
    RemoteChannels::Class *channels;
    //-------------------------------------------------------------------------
    si::SharedMemoryHolder shmh;
    RemoteChannelManager() : changed(NULL), mutex(NULL), channels(NULL) {}
protected:
    //-------------------------------------------------------------------------
    void initManager();
public:
    //-------------------------------------------------------------------------
    std::string getStreamId(const RCData &data) const;
	//-------------------------------------------------------------------------
	static RemoteChannelManager & instance();
    //-------------------------------------------------------------------------
    /**
     * task: determine when something was changed.
     * Since the traditional event approach is a bit to expensive (we have
     * to check frequently whether something was changed in the interprocess
     * memory) we use a "inter process" timestamp.
     * Every instance which is interrested, has to check this value.
     *
     * @return the recent timepoint when RemoteChannel was added or removed.
     */
    time_t getLastChangedTime() const {
        if (!changed) {
            return 0;
        }
        return *changed;
    }
    //-------------------------------------------------------------------------
    // Receiver stuff
    //-------------------------------------------------------------------------
    size_t getNumChannels() const;
    //-------------------------------------------------------------------------
    template <class Container>
    void getChannels(Container &out) const;
    //-------------------------------------------------------------------------
    RCData getChannelData(const RCId &id) const;
    //-------------------------------------------------------------------------
    Stream::Ptr getStream(const RCId &rc);
    //-------------------------------------------------------------------------
    // Sender stuff
    //-------------------------------------------------------------------------
    void addChannel(const RCId &hnd, const RCData &data);
    //-------------------------------------------------------------------------
    void removeChannel(const RCId &hnd);
    //-------------------------------------------------------------------------
    std::string getUniqueName() const;
    //-------------------------------------------------------------------------
    void removeAllChannels();
}; // RemoteChannelManager
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class Container>
void RemoteChannelManager::getChannels(Container &out) const {
    BOOST_FOREACH(const RemoteChannels::Class::value_type &x, *channels)
    {
        out.push_back( std::string(x.first.c_str()) );
    }
}

}}} // namespace(s)

#endif /* SAMBAG_REMOTECHANNELMANAGER_H */
