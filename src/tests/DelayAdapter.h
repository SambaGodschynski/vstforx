/*
 * ===========================================================================================================
 * DelayAdapter.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */

#ifndef DELAY_ADAPTER_H_
#define DELAY_ADAPTER_H

#include "processing/processing.h"


namespace processing {
//============================================================================================================
// DelayAdapter:
// ProcessAdapter der zu testzwecken das eingangsignal verzoegert wiedergibt.
//============================================================================================================
template < int _DELAY >
class DelayAdapter : public ProcessAdapter {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	enum { DELAY = _DELAY };
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr< DelayAdapter<_DELAY> > Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
		if (!hI) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
				"Hostinfo == NULL"
			);
		}
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		if ( Archive::is_loading::value ) {
			stream = new processing::DCStream ( hI->getBlockSize(), _DELAY );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	processing::DCStream *stream;
	//--------------------------------------------------------------------------------------------------------
	DelayAdapter() : ProcessAdapter() {}
protected:
	//--------------------------------------------------------------------------------------------------------
	DelayAdapter ( frx::processing::IHostInfo::Ptr hostInfo ) : 
		ProcessAdapter( hostInfo, 1, 1 )
	{
		setName ("DelayAdapter");
		stream = new processing::DCStream ( hostInfo->getBlockSize(), _DELAY );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void hostBaseConfigChanged() {
		stream->setSize( hostInfo.lock()->getBlockSize(), _DELAY );
	}
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo ) {
		Ptr neu( new DelayAdapter<_DELAY>( hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getProcessDelay() const { return stream->getMaxDelay(); }
	//--------------------------------------------------------------------------------------------------------
	virtual void processAdapter( Processor::Int sampleFrames ) {
		Frames *fr = getInputNode(0)->popFrame();
		stream->addFrame ( fr, sampleFrames, stream->getMaxDelay() );
		stream->flush ( sampleFrames, fr->getData() );
		outputNodes[0]->pushAndCopy( fr, sampleFrames );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual ~DelayAdapter (){
		delete stream;
		TOLOG ( "-" + getName() );
	}
};
} //namespace

#endif