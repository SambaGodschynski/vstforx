/*
 * ===========================================================================================================
 * RemoteChannelReceiver.cpp
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#include "RemoteChannelReceiver.h"


namespace processing{
//============================================================================================================
// RemoteChannelReceiver:
//============================================================================================================
//------------------------------------------------------------------------------------------------------------
RemoteChannelReceiver::RemoteChannelReceiver (IHostInfo *hostInfo) : 
	ProcessAdapter( hostInfo, 0, 1 ),
	buffer(NULL)
{
	setName ("RemoteChannelReceiver");
	getOutputNode(0)->setName ("RemoteChannelReceiver output");
	frames.setSize( getHostInfo()->getBlockSize() );
	frames.setZero( getHostInfo()->getBlockSize() );
	TOLOG ( "+" + getName() );
}
//------------------------------------------------------------------------------------------------------------
void RemoteChannelReceiver::processAdapter(Processor::Int numSamples) {
	if (buffer)
		buffer->read(frames.getData(), numSamples);

	getOutputNode(0)->pushAndCopy(&frames, numSamples);
}
//------------------------------------------------------------------------------------------------------------
void RemoteChannelReceiver::setRemoteChannelHandler(const RemoteChannelHandler &_rCHandler) {
	rCHandler = _rCHandler;
	buffer = 
		&getRemoteChannelManager()->getChannelBuffer(rCHandler);
}
//--------------------------------------------------------------------------------------------------------
RemoteChannelReceiver::~RemoteChannelReceiver() {
	TOLOG ( "-" + getName() );
	buffer = NULL;
	getRemoteChannelManager()->releaseChannelBuffer(rCHandler);
	getRemoteChannelManager()->releaseChannel(rCHandler);
}
}// namespace processing
