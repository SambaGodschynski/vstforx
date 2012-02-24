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

#include "AudioEffectX.h"
#include "processing/IHostInfo.h"

namespace processing {

struct DummyFX : public AudioEffectX, public IHostInfo {
	float sampleRate;
	int blockSize;
	VstTimeInfo timeInfo;
	DummyFX ( audioMasterCallback audioMaster, 
		      const VstTimeInfo & timeInfo = VstTimeInfo() 
			 ) : AudioEffectX ( audioMaster, 0, 0 ), timeInfo(timeInfo), blockSize(0), sampleRate(0) {}
	void processReplacing( float **, float **, VstInt32 ) {}
	virtual VstTimeInfo * getTimeInfo( int filter = 0 ) { return &timeInfo; }
	virtual float getSampleRate() const { return sampleRate; }
	virtual int getBlockSize() const { return blockSize; }
	virtual void setSampleRate( float sampleRate ) { DummyFX::sampleRate = sampleRate; }
	virtual void setBlockSize( int blockSize ) { DummyFX::blockSize = blockSize; }
	virtual VstTimeInfo * getVstTimeInfo ( VstInt32 filter ) { return getTimeInfo(filter); }
	virtual AudioEffectX * getAudioEffectX() { return this; }
	virtual AudioMasterCallback getAudioMasterCallback() { return audioMaster; }
	virtual ~DummyFX(){}
};

} // namespace
#endif 
