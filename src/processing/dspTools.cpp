#include "dspTools.h"
#include <boost/assign/list_inserter.hpp>

#define N(x) 1.0/(double)(x) 
#define NP(x) N(x)+N(x)/2.0
#define NT(x) N((x/2))/3.0

//------------------------------------------------------------------------------------------------------------
namespace com {
	namespace musicalValues {
		//----------------------------------------------------------------------------------------------------
		const NoteLength noteLengthTable[] = {
			{ "1/128" , N(128) },
			{ "1/128T", NT(128) }, 
			{ "1/128D", NP(128) }, 
			{ "1/64" , N(64) },
			{ "1/64T", NT(64) }, 
			{ "1/64D", NP(64) }, 
			{ "1/32" , N(32) },
			{ "1/32T", NT(32) }, 
			{ "1/32D", NP(32) }, 
			{ "1/16" , N(16) },
			{ "1/16T", NT(16) }, 
			{ "1/16D", NP(16) }, 
			{ "1/8" , N(8) },
			{ "1/8T", NT(8) }, 
			{ "1/8D", NP(8) }, 
			{ "1/4" , N(4) },
			{ "1/4T", NT(4) }, 
			{ "1/4D", NP(4) }, 
			{ "1/2" , N(2) },
			{ "1/2T", NT(2) }, 
			{ "1/2D", NP(2) }, 
			{ "1/1" , 1.0 },
		};
		//------------------------------------------------------------------------------------------------
		const int NUM_STDNOTES = sizeof( noteLengthTable ) / sizeof( noteLengthTable[0] );
		//------------------------------------------------------------------------------------------------
		typedef std::map<size_t, std::string > CCNamesMap;
		//------------------------------------------------------------------------------------------------
		CCNamesMap initCCNames();
		//------------------------------------------------------------------------------------------------
		const CCNamesMap ccNamesMap = initCCNames();
	} // namespace musicalValues

//========================================================================================================
// Klasse ADSR :
// Attack, Decay, Sustain, Release bestehend aus FadeValue
//========================================================================================================
//--------------------------------------------------------------------------------------------------------
const float ADSR::HOLD_FAK = 5.0f; 
//--------------------------------------------------------------------------------------------------------
ADSR::ADSR(IHostInfo *hostInfo, float maxD ) : 
hostInfo(hostInfo), 
in(0),
state(R),
holdSamples(0),
maxDurationInSec ( maxD ),
blockSize( hostInfo->getBlockSize() ),
sampleRate ( hostInfo->getSampleRate() ),
attackVal(0.0f)
{
	static const char bff[][15] = { {"attack"},{"decay"},{"sustain"},{"release"} };
	Parameter::ParameterListenerFunction lC=boost::bind( &ADSR::levelChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction dC=boost::bind( &ADSR::durationChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction cT=boost::bind( &ADSR::curveTypeChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction mC=boost::bind( &ADSR::modeChanged, this, _1, _2 );
	
	// mode.
	mode = Parameter::create();
	mode->setName ("mode");
	mode->addValueChangedListenerF ( mC );
	parameterMap.push_back ( mode );
	
	for ( int i=A; i<NUM_STATES; ++i ){
		curveType[i] = Parameter::create();
		curveType[i]->setIndex (i);
		curveType[i]->setName (MyString(bff[i]) + " slope type"); 
		curveType[i]->addValueChangedListenerF (cT); 
		curveType[i]->setValue(0.0f); 
		duration[i] = Parameter::create();
		duration[i]->setIndex (i);
		duration[i]->addValueChangedListenerF(dC);
		duration[i]->setName (MyString(bff[i]) + " duration"); 
		duration[i]->setLabel(" s"); 
		duration[i]->setDisplay("?");
		duration[i]->setValue (1.0f / maxD); 
		level[i] = Parameter::create();
		level[i]->setIndex (i);
		level[i]->setValue (1.0f - (i*1.0f/4.0f) ); 
		level[i]->setName (MyString(bff[i]) + " level"); 
		level[i]->addValueChangedListenerF (lC);
		parameterMap.push_back ( level[i] );
		parameterMap.push_back ( duration[i] );
		parameterMap.push_back ( curveType[i] );
	}
	// trigger tresh.
	threshold = Parameter::create();
	*threshold = 0.001f;
	threshold->setName ("trigger_threshold");
	parameterMap.push_back ( threshold );
	
	// trigger hold
	Parameter::ParameterListenerFunction hC=boost::bind( &ADSR::holdChanged, this, _1, _2 );
	hold = Parameter::create();
	*hold = 0.1f;
	hold->addValueChangedListenerF(hC);
	hold->setName ("trigger_hold");
	hold->setLabel("s");
	parameterMap.push_back ( hold );
	level[R]->setValue (0.0f); 
}
//--------------------------------------------------------------------------------------------------------
void ADSR::save ( oArchive &ar, const unsigned int version ) const {
	ar << hostInfo;
	ar << state;
	for ( int i=0; i<NUM_STATES; ++i ){
		ar << duration[i];
		ar << level[i];
		ar << curveType[i];
	}
	ar << mode;
	ar << threshold;
	ar << fader;
	ar << parameterMap;
	ar << in;
	ar << value;
	ar << maxDurationInSec;
	ar << trigger;
	ar << fadeFinished;
	ar << hold;
	ar << holdSamples;
	ar << attackVal;
	
}
//--------------------------------------------------------------------------------------------------------
void ADSR::load ( iArchive &ar, const unsigned int version ) {
	Parameter::ParameterListenerFunction lC=boost::bind( &ADSR::levelChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction dC=boost::bind( &ADSR::durationChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction cT=boost::bind( &ADSR::curveTypeChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction hC=boost::bind( &ADSR::holdChanged, this, _1, _2 );
	Parameter::ParameterListenerFunction mC=boost::bind( &ADSR::modeChanged, this, _1, _2 );
	ar >> hostInfo;
	blockSize = hostInfo->getBlockSize();
	ar >> state;
	for ( int i=0; i<NUM_STATES; ++i ){
		ar >> duration[i];
		ar >> level[i];
		ar >> curveType[i];
		duration[i]->addValueChangedListenerF ( dC );
		level[i]->addValueChangedListenerF ( lC );
		curveType[i]->addValueChangedListenerF ( cT );
	}
	ar >> mode;
	mode->addValueChangedListenerF ( mC );
	ar >> threshold;
	ar >> fader;
	ar >> parameterMap;
	ar >> in;
	ar >> value;
	ar >> maxDurationInSec;
	ar >> trigger;
	ar >> fadeFinished;
	ar >> hold;
	hold->addValueChangedListenerF ( hC );
	ar >> holdSamples;
	ar >> attackVal;
}
//--------------------------------------------------------------------------------------------------------
void ADSR::curveTypeChanged(void *src, const float &v){
	Parameter *p = (Parameter*) src;
	int type = mapInteger ( v, FADER_TYPES );
	fader.setType ( (FadeValue::FadeType)type );
	p->setDisplay ("type " + MyString(type+1) );
}
//--------------------------------------------------------------------------------------------------------
void ADSR::levelChanged(void *src, const float &v ){
	Parameter *p = (Parameter*) src;
	if ( state == p->getIndex() ) fader.resetEndValue ( v );
}
//--------------------------------------------------------------------------------------------------------
void ADSR::modeChanged(void *src, const float &v ) {
	mode->setLabel (isModeAbsolute() ? "absolute" : "relative" );
}
//--------------------------------------------------------------------------------------------------------
void ADSR::holdChanged(void *src, const float &v ){
	hold->setDisplay ( MyString( v*HOLD_FAK ) );
}
//--------------------------------------------------------------------------------------------------------
void ADSR::durationChanged(void *src, const float &v ){
	Parameter *p = (Parameter*) src;
	p->setDisplay( v * maxDurationInSec * sampleRate / sampleRate   );  
}
//--------------------------------------------------------------------------------------------------------
ADSR::~ADSR() {
}

namespace musicalValues {
//--------------------------------------------------------------------------------------------------------
CCNamesMap initCCNames() {
	using namespace boost::assign;
	CCNamesMap ccs;
	insert( ccs )
		(0  , "Bank select")
		(1  , "Modulation")
		(2  , "Breath Controller")
		(4  , "Foot Controller")
		(5  , "Portamento Time")
		(6  , "Data Byte")
		(7  , "Main volume")
		(8  , "Balance")
		(10 , "Panorama")
		(11 , "Expression")
		(12 , "Effect Control 1")
		(13 , "Effect Control 2")
		(64 , "Hold 1")
		(65 , "Portamento")
		(66 , "Sostenuto")
		(67 , "Soft Pedal")
		(68 , "Legato Footswitch")
		(69 , "Hold 2")
		(70 , "Sound Variation")
		(71 , "Harmonic Content")
		(72 , "Release Time")
		(73 , "Attack Time")
		(74 , "Brightness")
		(84 , "Portamento Control")
		(91 , "Effects 1 Depth")
		(92 , "Effects 2 Depth")
		(93 , "Effects 3 Depth")
		(94 , "Effects 4 Depth")
		(95 , "Effects 5 Depth")
		(120, "All sounds off")
		(121, "Controller Reset")
		(122, "Local Control on/off")
		(123, "all notes off")
		(124, "omni off")
		(125, "omni on")
		(126, "mono on / poly off")
		(127, "poly on / mono off");
	return ccs;
}
//------------------------------------------------------------------------------------------------
std::string getCCName ( size_t index ) {
	CCNamesMap::const_iterator it = ccNamesMap.find(index);
	if ( it==ccNamesMap.end() ) return "unnamed";
	return it->second;
}



} // namespace musicalValues
} // namespace com