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

//=============================================================================
// Buffer Allocator
//=============================================================================
namespace boost { namespace interprocess {
	typedef allocator<float, 
		managed_shared_memory::segment_manager
	>  BufferAllocator;
}} // namespaces

namespace processing {
//=============================================================================
struct RemoteChannel {
//=============================================================================
	std::string name;
	std::string bufferId;
	RemoteChannel(const std::string &name="unnamed") : name(name) {}
	typedef bi::vector<
		float, 
		bi::BufferAllocator
	> Buffer;
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
	RemoteChannel::Buffer & getChannelBuffer(const RemoteChannel &channel);
	void releaseChannel(const RemoteChannel &channel);
	void releaseChannelBuffer(const RemoteChannel &channel);
	static RemoteChannelManager * instance();
	RemoteChannel createRemoteChannel(const std::string &name);
	const RegisteredChannels & getRegisteredChannels() const;
	RegisteredChannels & getRegisteredChannels();
};

inline RemoteChannelManager * getRemoteChannelManager() {
	return RemoteChannelManager::instance();
}

} // namespace

#endif