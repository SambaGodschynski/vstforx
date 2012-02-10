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
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		if ( Archive::is_loading::value ) {
			stream = new processing::DCStream ( hostInfo->getBlockSize(), _DELAY );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	processing::DCStream *stream;
	//--------------------------------------------------------------------------------------------------------
	DelayAdapter() : ProcessAdapter() {}
protected:
	//--------------------------------------------------------------------------------------------------------
	DelayAdapter ( IHostInfo *hostInfo ) : 
		ProcessAdapter( hostInfo, 1, 1 )
	{
		setName ("DelayAdapter");
		stream = new processing::DCStream ( hostInfo->getBlockSize(), _DELAY );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged() {
		stream->setSize( hostInfo->getBlockSize(), _DELAY );
	}
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new DelayAdapter<_DELAY>( hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getProcessDelay() const { return stream->getMaxDelay(); }
	//--------------------------------------------------------------------------------------------------------
	virtual void _processAdapter( Processor::Int sampleFrames ) {
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