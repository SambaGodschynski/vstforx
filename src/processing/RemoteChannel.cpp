#include "RemoteChannel.h"


namespace processing {
using namespace boost::interprocess;

#define REMOTE_CHANNEL "VSTForx RemoteChannel"
#define CHANNEL_REGISTER "VSTForx ChannelRegister"
#define CHANNEL_BUFFER "Channel Buffer"

namespace {
	managed_shared_memory segment;
}

//=============================================================================
// RemoteChannelManager
//=============================================================================
//-----------------------------------------------------------------------------
void RemoteChannelManager::initSharedMemory(int tries) {
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
	ChannelAllocator alloc_inst (segment.get_segment_manager());
	//Construct a vector in shared memory with argument alloc_inst
	channels = 
		segment.find_or_construct<RegisteredChannels>
		(CHANNEL_REGISTER)(std::less<std::string>(), alloc_inst);
}
//-----------------------------------------------------------------------------
RegisteredChannels * RemoteChannelManager::channels = NULL;
//-----------------------------------------------------------------------------
RCMValueType RemoteChannelManager::create(const std::string &name) {
	RCMappedType v(RemoteChannel(name), 1);
	return RCMValueType(name, v);
}
//-----------------------------------------------------------------------------
int & RemoteChannelManager::getNbReferences(const RemoteChannel &channel) {
	RegisteredChannels::iterator it = channels->find(channel.name);
	if (it==channels->end()) {
		// occurs when all references gone
		static int dummy = -1;
		return dummy;
	}
	return (it->second.second);
}

//-----------------------------------------------------------------------------
void RemoteChannelManager::createChannelBuffer(RemoteChannel &channel) {
	RemoteChannel::Buffer::Allocator alloc_inst (segment.get_segment_manager());
	channel.bufferId = channel.name + " " + CHANNEL_BUFFER;
	segment.find_or_construct<RemoteChannel::Buffer>
		(channel.bufferId.c_str())(alloc_inst);
}
//-----------------------------------------------------------------------------
RemoteChannel::Buffer & 
RemoteChannelManager::getChannelBuffer(const RemoteChannel &channel) 
{
	int &refs = getNbReferences(channel.name);
	refs++;
	return *segment.find<RemoteChannel::Buffer>
		(channel.bufferId.c_str()).first;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::releaseChannel(const RemoteChannel &channel) {
	int &refs = getNbReferences(channel.name);
	if (--refs!= 0) 
		return;
	RegisteredChannels::iterator it = channels->find(channel.name);
	segment.destroy<RemoteChannel::Buffer>(it->second.first.bufferId.c_str());
	channels->erase(it);

	if (channels->size() == 0) { // remove shared memory when vector is empty
		shared_memory_object::remove(REMOTE_CHANNEL);
	}
	
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::releaseChannelBuffer(const RemoteChannel &channel) {
	releaseChannel(channel);	
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
RemoteChannel 
RemoteChannelManager::createRemoteChannel(const std::string &name)
{
	using namespace boost::interprocess;
	channels->insert(create(name));
	RemoteChannel &neu = (*channels)[name].first;
	createChannelBuffer(neu);
	return neu;
}
} // namespace
