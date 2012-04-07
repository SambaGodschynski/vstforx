#ifndef REMOTE_CHANNEL_HPP_
#define REMOTE_CHANNEL_HPP_

#include "processing/Frames.h"
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/sync/interprocess_upgradable_mutex.hpp>
#include <boost/foreach.hpp>
#include <utility>
#include <map>

namespace bi = boost::interprocess;

namespace processing {
//=============================================================================
struct RemoteChannelHandler {
//=============================================================================
	std::string name;
	std::string bufferId;
	RemoteChannelHandler(const std::string &name="unnamed") : name(name) {}
	struct Buffer;
};
//=============================================================================
struct RemoteChannelHandler::Buffer {
//=============================================================================
	typedef bi::allocator<float, bi::managed_shared_memory::segment_manager>  
		Allocator;
        Buffer(const Allocator &alloc) : data(alloc) {}
	typedef bi::vector<float, Allocator> _Buffer;
	_Buffer data;
	mutable bi::interprocess_upgradable_mutex mutex;
	_Buffer::value_type & operator[](size_t i) {return data[i];}
	const _Buffer::value_type & operator[](size_t i) const {return data[i];}
	void write(float **in, size_t numSamples);
	void read(float **out, size_t numSamples) const;
};
//=============================================================================
// RemoteChannelHandler Map 
// channel name => ( channel obj., reference counter )
//=============================================================================
typedef std::string RCMKeyType;
typedef std::pair<RemoteChannelHandler, int> RCMappedType;
typedef std::pair<const RCMKeyType, RCMappedType> RCMValueType;

} // namespace procesing

namespace boost { namespace interprocess {
typedef allocator<processing::RCMValueType, 
	managed_shared_memory::segment_manager
>  ChannelAllocator;
}} //namespaces

namespace processing {

typedef bi::map<
	RCMKeyType,
	RCMappedType,
	std::less<RCMKeyType>,
	bi::ChannelAllocator
> RegisteredChannels;

//=============================================================================
class RemoteChannelManager {
//=============================================================================
private:
	RemoteChannelManager();
	void createChannelBuffer(RemoteChannelHandler &channel);
	int & getNbReferences(const RemoteChannelHandler &channel);
	RCMValueType create(const std::string &name);
	static RegisteredChannels *channels;
	void initSharedMemory(int tries = 0);
public:
	/**
	 * @param channel
	 * @return channel buffer related to RemoteChannelHandler
	 */
	RemoteChannelHandler::Buffer & getChannelBuffer(const RemoteChannelHandler &channel);
	/**
	 * Decreases related reference counter.
	 * Channel will be destroyed when reference counter == 0
	 * @param channel
	 */
	void releaseChannel(const RemoteChannelHandler &channel);
	/**
	 * Decreases related channel reference counter.
	 * Channel will be destroyed when reference counter == 0
	 * @param channel
	 */
	void releaseChannelBuffer(const RemoteChannelHandler &channel);
	/**
	 * @return ChannelManager
	 */
	static RemoteChannelManager * instance();
	/**
	 * Creates a RemoteChannelHandler object and a related buffer.
	 * @param name
	 * @return RemoteChannelHandler handler
	 */
	RemoteChannelHandler createRemoteChannel(const std::string &name);
	/**
	 * @return all registered channels.
	 */
	template <class ChannelContainer>
	void getRegisteredChannels(ChannelContainer &outContainer) {
		typedef RegisteredChannels::value_type V;
		BOOST_FOREACH(const V &v, *channels) {
			outContainer.push_back(v.second.first);
		}
	}
};

inline RemoteChannelManager * getRemoteChannelManager() {
	return RemoteChannelManager::instance();
}

} // namespace

#endif
