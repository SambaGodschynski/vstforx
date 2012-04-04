#ifndef REMOTE_CHANNEL_HPP_
#define REMOTE_CHANNEL_HPP_

#include "processing/Frames.h"
#include <string>
//#include <vector>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

using namespace boost::interprocess;


typedef allocator<float, 
	managed_shared_memory::segment_manager
>  BufferAllocator;


namespace processing {

//=============================================================================
struct RemoteChannel {
//=============================================================================
	std::string name;
	std::string bufferId;
	RemoteChannel(const std::string &name) : name(name) {}
	typedef vector<
		float, 
		BufferAllocator
	> Buffer;
};

typedef allocator<RemoteChannel, 
	managed_shared_memory::segment_manager
>  ChannelAllocator;

typedef vector<
	RemoteChannel, 
	ChannelAllocator
> RegisteredChannels;

//=============================================================================
class RemoteChannelManager {
//=============================================================================
private:
	RemoteChannelManager();
	void createChannelBuffer(RemoteChannel &channel);
public:
	RemoteChannel::Buffer & getChannelBuffer(const RemoteChannel &channel);
	static RemoteChannelManager * instance();
	RemoteChannel * createRemoteChannel(const std::string &name);
	void removeRemoteChannel(const std::string &name);
	const RegisteredChannels & getRegisteredChannels(); 
};

} // namespace

#endif