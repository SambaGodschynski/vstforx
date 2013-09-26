/*
 * RemoteChReceiver.hpp
 *
 *  Created on: Tue Sep 17 21:10:30 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_REMOTECHRECEIVER_H
#define SAMBAG_REMOTECHRECEIVER_H

#include <boost/shared_ptr.hpp>
#include "RemoteChannelManager.hpp"
#include "Stream.hpp"
#include "processing/processing.h"
#include <com/Serialization.h>
#include <processing/parameter/parameter.h>
#include "Stream.hpp"

namespace frx { namespace processing { namespace interprocess {
namespace pr = ::processing;
namespace prp = ::processing::parameter;
//=============================================================================
/** 
  * @class RemoteChReceiver.
  */
class RemoteChReceiver :
    public pr::ProcessAdapter,
    public prp::HasParameter,
    public ::com::Serializable
{
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<RemoteChReceiver> Ptr;
private:
    //-------------------------------------------------------------------------
    size_t blocksRead;
    //-------------------------------------------------------------------------
    ::processing::Frames frames;
    //-------------------------------------------------------------------------
    ::processing::DCStream dcStream;
    //-------------------------------------------------------------------------
    Stream::Ptr ipStream;
	//-------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void initListener(){}
	//-------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Volume-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object< pr::ProcessAdapter > ( *this );
		if ( Archive::is_loading::value ) {
			frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
			if (!hI) {
				SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
					"Hostinfo == NULL"
				);
			}
			initListener();
		}
	}
	//-------------------------------------------------------------------------
	RemoteChReceiver() : blocksRead(Stream::UndefinedNumBlocks) {}
protected:
	//-------------------------------------------------------------------------
	RemoteChReceiver(frx::processing::IHostInfo::Ptr hostInfo,
                     const std::string &rcId,
                     size_t numOutputs);
public:
	//-------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @return neues Volume-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo,
                       const std::string &rcId);
    //--------------------------------------------------------------------------
	/**
	 * HostInfo(Samplerate/Blocksize) geaendert.
	 */
	virtual void hostBaseConfigChanged() {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu verarbeitenden Samples
	 */
	virtual void processAdapter( pr::Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param index
	 * @return parameter to index
	 */
	virtual prp::Parameter::Ptr getParameter ( size_t index = 0 ) const { return prp::Parameter::Ptr(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 0; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~RemoteChReceiver() {}
};
}}} // namespace(s)

#endif /* SAMBAG_REMOTECHRECEIVER_H */
