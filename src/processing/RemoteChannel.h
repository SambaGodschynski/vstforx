#ifndef REMOTE_CHANNEL_HPP_
#define REMOTE_CHANNEL_HPP_

#include "processing/Frames.h"
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <utility>
#include <map>

namespace bi = boost::interprocess;

namespace processing {
//=============================================================================
struct RemoteChannel {
//=============================================================================
	std::string name;
	std::string bufferId;
	RemoteChannel(const std::string &name="unnamed") : name(name) {}
	struct Buffer;
};
//=============================================================================
struct RemoteChannel::Buffer {
//=============================================================================
	typedef bi::allocator<float, bi::managed_shared_memory::segment_manager>  
		Allocator;
	Buffer(const Allocator &alloc) : data(alloc) {}
	typedef bi::vector<float, Allocator> _Buffer;
	_Buffer data;
	_Buffer::value_type & operator[](size_t i) {return data[i];}
	const _Buffer::value_type & operator[](size_t i) const {return data[i];}
};
//=============================================================================
// RemoteChannel Map 
// channel name => ( channel obj., reference counter )
//=============================================================================
typedef std::string RCMKeyType;
typedef std::pair<RemoteChannel, int> RCMappedType;
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
	void createChannelBuffer(RemoteChannel &channel);
	int & getNbReferences(const RemoteChannel &channel);
	RCMValueType create(const std::string &name);
public:
	/**
	 * @param channel
	 * @return channel buffer related to RemoteChannel
	 */
	RemoteChannel::Buffer & getChannelBuffer(const RemoteChannel &channel);
	/**
	 * Decreases related reference counter.
	 * Channel will be destroyed when reference counter == 0
	 * @param channel
	 */
	void releaseChannel(const RemoteChannel &channel);
	/**
	 * Decreases related channel reference counter.
	 * Channel will be destroyed when reference counter == 0
	 * @param channel
	 */
	void releaseChannelBuffer(const RemoteChannel &channel);
	/**
	 * @return ChannelManager
	 */
	static RemoteChannelManager * instance();
	/**
	 * Creates a RemoteChannel object and a related buffer.
	 * @param name
	 * @return RemoteChannel handler
	 */
	RemoteChannel createRemoteChannel(const std::string &name);
	/**
	 * @return all registered channels.
	 * TODO: return only handler intead whole map.
	 */
	const RegisteredChannels & getRegisteredChannels() const;
	RegisteredChannels & getRegisteredChannels();
};

inline RemoteChannelManager * getRemoteChannelManager() {
	return RemoteChannelManager::instance();
}

} // namespace

#endif