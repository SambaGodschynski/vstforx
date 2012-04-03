#ifndef REMOTE_CHANNEL_HPP_
#define REMOTE_CHANNEL_HPP_

#include "processing/Frames.h"
#include <string>
#include <vector>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

namespace boost { namespace interprocess {
	typedef allocator<int, managed_shared_memory::segment_manager>  ShmemAllocator;
}}

namespace processing {

//=============================================================================
struct RemoteChannel {
//=============================================================================
	std::string name;
	RemoteChannel(const std::string &name) : name(name) {}
};

typedef std::vector<
	RemoteChannel, 
	boost::interprocess::ShmemAllocator
> RegisteredChannels;

//=============================================================================
class RemoteChannelManager {
//=============================================================================
private:
	RemoteChannelManager();
public:
	static RemoteChannelManager * instance();
	RemoteChannel * createRemoteChannel(const std::string &name);
	void removeRemoteChannel(const std::string &name);
	const RegisteredChannels & getRegisteredChannels(); 
};

} // namespace

#endif