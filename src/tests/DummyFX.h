/*
 * ===========================================================================================================
 * DummyFX.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */


#ifndef DUMMY_FX_H
#define DUMMY_FX_H

/*
		test effect instance
*/

#include "audioeffectx.h"
#include "processing/IHostInfo.h"
#include <boost/shared_ptr.hpp>
#include <com/Serialization.h>


namespace processing {

struct DummyFX : public AudioEffectX, public frx::processing::IHostInfo {
public:
	DummyFX() : AudioEffectX ( NULL, 0, 0 ) {}
	DummyFX ( audioMasterCallback audioMaster, 
		      const frx::processing::TimeInfo & timeInfo = frx::processing::TimeInfo() 
	) : AudioEffectX ( audioMaster, 0, 0 ), timeInfo(timeInfo), blockSize(0), sampleRate(0) {}
public:
	typedef boost::shared_ptr<DummyFX> Ptr;
	float sampleRate;
	int blockSize;
	frx::processing::TimeInfo timeInfo;
	void processReplacing( float **, float **, VstInt32 ) {}
	virtual frx::processing::TimeInfo * getHostTimeInfo( int filter = 0 ) { return &timeInfo; }
	virtual float getSampleRate() const { return sampleRate; }
	virtual int getBlockSize() const { return blockSize; }
	virtual void setSampleRate( float sampleRate ) { DummyFX::sampleRate = sampleRate; }
	virtual void setBlockSize( int blockSize ) { DummyFX::blockSize = blockSize; }
	virtual bool ioChanged() {return true;}
	virtual void * getEffectPtr() {
		return this;
	}
    MasterType getMasterType() const {
        return VST2X;
    }
	virtual void * getMasterCallback() {
		return (void*)audioMaster;
	}
	virtual ~DummyFX(){}
	virtual HostIOChangedConnection 
	addHostChangedListener(const HostIOChangedFunction &f) {
		return HostIOChangedConnection();
	}
	virtual HostIOChangedConnection 
	addTrackedHostChangedListener(const HostIOChangedFunction &f, AnyWPtr wptr) {
		return HostIOChangedConnection();
	}
	static Ptr create( audioMasterCallback audioMaster, 
	     const frx::processing::TimeInfo & timeInfo = frx::processing::TimeInfo() )
	{
		return Ptr(new DummyFX(audioMaster, timeInfo));
	}
	template< typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<frx::processing::IHostInfo> ( *this );
		ar & sampleRate;
		ar & blockSize;
	}
    virtual frx::scripts::PluginScriptCtrlPtr getScriptController() const {
        return frx::scripts::PluginScriptCtrlPtr();
    }
};

} // namespace
#endif 
