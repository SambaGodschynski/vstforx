/*
 * ===========================================================================================================
 * RemoteChannelReceiver.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_REMOTECHANNEL_H
#define FORX_REMOTECHANNEL_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "processing/RemoteChannel.h"

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class RemoteChannelReceiver.
 */
class RemoteChannelReceiver:
//============================================================================================================
public ProcessAdapter, 
public Serializable
{
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<RemoteChannelReceiver> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	Frames frames;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert RemoteChannelReceiver-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object<ProcessAdapter> (*this);
	}
	//--------------------------------------------------------------------------------------------------------
	RemoteChannelReceiver (){} // wird nur von boost::serial. benutzt
protected:
	//--------------------------------------------------------------------------------------------------------
	RemoteChannelReceiver (IHostInfo *hostInfo);
	//--------------------------------------------------------------------------------------------------------
	RemoteChannelHandler rCHandler;
	//--------------------------------------------------------------------------------------------------------
	RemoteChannelHandler::Buffer *buffer;
public:
	//--------------------------------------------------------------------------------------------------------
	void setRemoteChannelHandler(const RemoteChannelHandler &rCHandler);
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initValue
	 * @return neues Volume-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new RemoteChannelReceiver(hostInfo) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * HostInfo(Samplerate/Blocksize) geaendert.
	 */
	virtual void hostInfoChanged() {
		frames.setSize( getHostInfo()->getBlockSize() );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu verarbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~RemoteChannelReceiver();
};
}// namespace processing

#endif  // FORX_REMOTECHANNEL_H


