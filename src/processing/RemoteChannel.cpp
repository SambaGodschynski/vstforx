#include "RemoteChannel.h"


namespace processing {
using namespace boost::interprocess;

#define REMOTE_CHANNEL "VSTForx RemoteChannelHandler"
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
		// open shared memory
		segment = 
			managed_shared_memory(open_or_create, REMOTE_CHANNEL, 65536);
	} catch (...) {
		if (tries>0)
			throw;
		shared_memory_object::remove(REMOTE_CHANNEL);
		initSharedMemory(tries+1);
		return;
	}
	ChannelAllocator alloc_inst (segment.get_segment_manager());
	// create or find registered channels
	channels = 
		segment.find_or_construct<RegisteredChannels>
			(CHANNEL_REGISTER)(std::less<std::string>(), alloc_inst);
}
//-----------------------------------------------------------------------------
RegisteredChannels * RemoteChannelManager::channels = NULL;
//-----------------------------------------------------------------------------
RCMValueType RemoteChannelManager::create(const std::string &name) {
	RCMappedType v(RemoteChannelHandler(name), 1);
	return RCMValueType(name, v);
}
//-----------------------------------------------------------------------------
int & RemoteChannelManager::getNbReferences(const RemoteChannelHandler &channel) {
	// used when all references gone
	static int dummy = -1;
	if (!channels) 
		return dummy;
	RegisteredChannels::iterator it = channels->find(channel.name);
	if (it==channels->end()) 
		return dummy;
	
	return (it->second.second);
}

//-----------------------------------------------------------------------------
void RemoteChannelManager::createChannelBuffer(RemoteChannelHandler &channel) {
	RemoteChannelHandler::Buffer::Allocator alloc_inst (segment.get_segment_manager());
	channel.bufferId = channel.name + " " + CHANNEL_BUFFER;
	segment.find_or_construct<RemoteChannelHandler::Buffer>
		(channel.bufferId.c_str())(alloc_inst);
}
//-----------------------------------------------------------------------------
RemoteChannelHandler::Buffer & 
RemoteChannelManager::getChannelBuffer(const RemoteChannelHandler &channel) 
{
	int &refs = getNbReferences(channel.name);
	refs++;
	return *segment.find<RemoteChannelHandler::Buffer>
		(channel.bufferId.c_str()).first;
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::releaseChannel(const RemoteChannelHandler &channel) {
	// decrease nbReferences for channel
	int &refs = getNbReferences(channel.name);
	if (--refs!= 0) 
		return;
	// release channel
	RegisteredChannels::iterator it = channels->find(channel.name);
	segment.destroy<RemoteChannelHandler::Buffer>(it->second.first.bufferId.c_str());
	channels->erase(it);

	if (channels->size() == 0) { // remove shared memory when vector is empty
		shared_memory_object::remove(REMOTE_CHANNEL);
		channels = NULL;
	}
	
}
//-----------------------------------------------------------------------------
void RemoteChannelManager::releaseChannelBuffer(const RemoteChannelHandler &channel) {
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
RemoteChannelHandler 
RemoteChannelManager::createRemoteChannel(const std::string &name)
{
	using namespace boost::interprocess;
	if (!channels)
		initSharedMemory();
	channels->insert(create(name));
	RemoteChannelHandler &neu = (*channels)[name].first;
	createChannelBuffer(neu);
	return neu;
}
//=============================================================================
// struct RemoteChannelHandler::Buffer
//=============================================================================
//-----------------------------------------------------------------------------
void RemoteChannelHandler::Buffer::write(float **in, size_t numSamples) {
	mutex.lock();
	for (size_t i=0; i<numSamples; ++i) {
		(*this)[i] = in[0][i];
	}
	mutex.unlock();
}
//----------------------------------------------------------------------------- 
void RemoteChannelHandler::Buffer::read(float **out, size_t numSamples) const {
	mutex.lock_sharable();
	for (int i=0; i<numSamples; ++i) {
		out[0][i]   = (*this)[i];
		out[1][i]   = (*this)[i];
	}
	mutex.unlock_sharable();
}

} // namespace
