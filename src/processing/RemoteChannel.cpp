#include "RemoteChannel.h"


namespace processing {
using namespace boost::interprocess;

#define REMOTE_CHANNEL "VSTForx RemoteChannel"
#define CHANNEL_REGISTER "VSTForx ChannelRegister"
#define CHANNEL_BUFFER "Channel Buffer"

namespace {
	managed_shared_memory segment;
	RegisteredChannels * channels;
}

//-----------------------------------------------------------------------------
void initSharedMemory(int tries = 0) {
	try {
		segment = 
			managed_shared_memory(open_or_create, REMOTE_CHANNEL, 65536);
	} catch (...) {
		if (tries>0)
			throw;
		shared_memory_object::remove(REMOTE_CHANNEL);
		initSharedMemory(tries+1);
		return;
	}
	//Initialize shared memory STL-compatible allocator
	const ShmemAllocator alloc_inst (segment.get_segment_manager());
	//Construct a vector in shared memory with argument alloc_inst
	channels = 
		segment.find_or_construct<RegisteredChannels>(CHANNEL_REGISTER)(alloc_inst);
}

//=============================================================================
// RemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
void RemoteChannelManager::createChannelBuffer(RemoteChannel &channel) {
	const ShmemAllocator alloc_inst (segment.get_segment_manager());
	channel.bufferId = channel.name + " " + CHANNEL_BUFFER;
	segment.find_or_construct<RemoteChannel::Buffer>
		(channel.bufferId.c_str())(alloc_inst);
}
//-----------------------------------------------------------------------------
RemoteChannel::Buffer & 
RemoteChannelManager::getChannelBuffer(const RemoteChannel &channel) 
{
	return *segment.find<RemoteChannel::Buffer>
		(channel.bufferId.c_str()).first;
}
//-----------------------------------------------------------------------------
RemoteChannelManager::RemoteChannelManager() {
	initSharedMemory();
}
//-----------------------------------------------------------------------------
RemoteChannelManager * RemoteChannelManager::instance() {
	static RemoteChannelManager mngr;
	return &mngr;
}
//-----------------------------------------------------------------------------
RemoteChannel * 
RemoteChannelManager::createRemoteChannel(const std::string &name)
{
	channels->push_back(RemoteChannel(name));
	createChannelBuffer(channels->back());
	return &channels->back();
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::removeRemoteChannel(const std::string &name) {
	RegisteredChannels::iterator it = channels->begin();
	for(; it!=channels->end(); ++it) {
		if(it->name==name) {
			segment.destroy<RemoteChannel::Buffer>(it->bufferId.c_str());
			channels->erase(it);
			break;
		}
	}
	if (channels->size() == 0) {
		shared_memory_object::remove(REMOTE_CHANNEL);
	}
}
//-----------------------------------------------------------------------------
const RegisteredChannels & RemoteChannelManager::getRegisteredChannels() {
	return *channels;
}

} // namespace
