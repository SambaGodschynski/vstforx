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
#include <sambag/com/PlacementAlloc.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/foreach.hpp>
#include <processing/FrxAsyncDSPTimer.hpp>
#include <sambag/com/SharedMemory.hpp> 

namespace frx { namespace processing { namespace interprocess {
namespace bi = boost::interprocess;
namespace si = sambag::com::interprocess;
using ::sambag::com::interprocess::Integer;
using ::sambag::com::interprocess::UInteger;
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
    /**
     * tuple(streamId)
     */
    typedef boost::tuple<std::string> RCData;
private:
    //-------------------------------------------------------------------------
    //since boost managed memory has a 32/64 communictaion bug, we
    //need to use a workaround.
    //typedef si::String::Class SHM_String;
    //-------------------------------------------------------------------------
    //typedef SHM_String SHM_RCId;
    //-------------------------------------------------------------------------
    //typedef boost::tuple<SHM_String> SHM_RCData;
    //-------------------------------------------------------------------------
    //typedef si::Map<SHM_RCId, SHM_RCData> RemoteChannels;
    struct RemoteChannels;
    //-------------------------------------------------------------------------
    UInteger *changed;
    //-------------------------------------------------------------------------
    UInteger *totmann_time;
    //-------------------------------------------------------------------------
    UInteger *references;
    //-------------------------------------------------------------------------
    typedef boost::interprocess::interprocess_mutex Mutex;
    //-------------------------------------------------------------------------
    Mutex *mutex;
    //-------------------------------------------------------------------------
    //RemoteChannels::Class *channels;
    RemoteChannels *channels;
    //-------------------------------------------------------------------------
    //typedef si::SharedMemoryHolder SHMH;
    //typedef boost::shared_ptr<SHMH> SHMHPtr;
    //SHMHPtr shmh;
	typedef boost::shared_ptr<si::SharedMemoryObject> SharedMemoryObjectPtr;
    SharedMemoryObjectPtr shm;
	typedef boost::shared_ptr<si::MappedRegion> MappedRegionPtr;
    MappedRegionPtr mapped_region;
    //-------------------------------------------------------------------------
    RemoteChannelManager();
    //-------------------------------------------------------------------------
    FrxAsyncDSPTimer::Ptr totmannTimer;
    //-------------------------------------------------------------------------
    struct Dummy {};
    typedef boost::shared_ptr<Dummy> TrackingDummyPtr;
    TrackingDummyPtr trackingDummy;
    //-------------------------------------------------------------------------
    bool isStreamValid(const std::string &streamId) const;
protected:
    //-------------------------------------------------------------------------
    void doTotmann();
    //-------------------------------------------------------------------------
    bool isTotmann();
    //-------------------------------------------------------------------------
    void initManager(int tries=0);
    //-------------------------------------------------------------------------
    void destroyShm();
public:
    //-------------------------------------------------------------------------
    /**
     * @return the number of running RemoteChannelManager instances.
     */
    UInteger getNumReferences() const {
        return *references;
    }
    //-------------------------------------------------------------------------
    std::string getStreamId(const RCData &data) const;
	//-------------------------------------------------------------------------
	virtual ~RemoteChannelManager();
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
    UInteger getLastChangedTime() const {
        if (!changed) {
            return 0;
        }
        return *changed;
    }
    //-------------------------------------------------------------------------
    // Receiver stuff
    //-------------------------------------------------------------------------
    UInteger getNumChannels() const;
    //-------------------------------------------------------------------------
    //template <class Container>
    //void getChannels(Container &out) const;
    void getChannels(std::vector<RCId> &out);
    void getChannels(std::list<RCId> &out);
    //-------------------------------------------------------------------------
    RCData getChannelData(const RCId &id) const;
    //-------------------------------------------------------------------------
    Stream::Ptr getStream(const RCId &rc);
    //-------------------------------------------------------------------------
    // Sender stuff
    //-------------------------------------------------------------------------
    void addChannel(const RCId &hnd, const RCData &data);
    //-------------------------------------------------------------------------
    RCId addChannel(const RCData &data);
    //-------------------------------------------------------------------------
    void removeChannel(const RCId &hnd);
    //-------------------------------------------------------------------------
    std::string createUniqueName() const;
    //-------------------------------------------------------------------------
    void removeAllChannels();
    //-------------------------------------------------------------------------
    std::string getName(const RCId &id);
}; // RemoteChannelManager
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
/*template <class Container>
void RemoteChannelManager::getChannels(Container &out) const {
    BOOST_FOREACH(const RemoteChannels::Class::value_type &x, *channels)
    {
        out.push_back( std::string(x.first.c_str()) );
    }
}*/
}}} // namespace(s)

#endif /* SAMBAG_REMOTECHANNELMANAGER_H */