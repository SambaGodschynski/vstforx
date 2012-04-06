#ifndef REMOTE_CHANNEL_HPP_
#define REMOTE_CHANNEL_HPP_

#include "processing/Frames.h"
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/foreach.hpp>
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
        Buffer(const Allocator &alloc) : data(alloc), wrote(0) {}
	typedef bi::vector<float, Allocator> _Buffer;
	_Buffer data;
	_Buffer::value_type & operator[](size_t i) {return data[i%data.size()];}
	const _Buffer::value_type & operator[](size_t i) const {return data[i%data.size()];}
	int wrote;
	void write(float **in, size_t numSamples);
	void read(float **out, size_t numSamples) const;
	static const int preBuffer = 1024;
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
	static RegisteredChannels *channels;
	void initSharedMemory(int tries = 0);
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
