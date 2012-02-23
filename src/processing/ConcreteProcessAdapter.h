/*
 * ===========================================================================================================
 * ConcreteProcessAdapter.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef CONCRETE_PROCESS_ADAPTER
#define CONCRETE_PROCESS_ADAPTER

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/one4All.h"
#include "dspTools.h"
#include "com/Serialization.h"
#include "MidiEventProcessor.h"

//============================================================================================================
//	Vorwaertz Deklarationen
//============================================================================================================
namespace processing {
using namespace std;
using namespace com;
using namespace parameter;
class Volume;
//============================================================================================================
//Volume2Parameter
//Tranformiert Signal Lautstaerke in Parameter wert.
//============================================================================================================
class PeakTracker;
class Pan;
class OutputStep;
class ValueTranslator;
class MidiProcessor;
}// namespace processing

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class Volume.
 * Multipliziert Eingangs-Samplemenge mit Multiplikator.
 */
class Volume :
//============================================================================================================
public ProcessAdapter, 
public HasParameter, 
public Serializable,
public events::ValueChangedListener<float>
{
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Volume> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert Volume-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		ar & volume;
		if ( Archive::is_loading::value ) {
			volume->addValueChangedListener (this);
			fader.setDuration( getFaderDuration( hostInfo->getSampleRate() ) );  
			fader.setValue ( *volume );
		}
	}
	//--------------------------------------------------------------------------------------------------------
	Volume (){} // wird nur von boost::serial. benutzt
	//--------------------------------------------------------------------------------------------------------
	FadeValue fader;
protected:
	//--------------------------------------------------------------------------------------------------------
	static float getFaderDuration( float sampleRate ) {
		float one_ms = sampleRate / 1000.0;
		return one_ms*5.0f;
	}
	//--------------------------------------------------------------------------------------------------------
	Volume ( IHostInfo *hostInfo, float initValue ) : 
		ProcessAdapter( hostInfo, 1, 1 ), fader( initValue ) 
	{
		setName ("Volume");
		volume = Parameter::create();
		volume->setName ("Volume");
		volume->addValueChangedListener (this);
		*volume = initValue;
		getInputNode(0)->setName ("Volume Input Node");
		getOutputNode(0)->setName ("Volume Output Node");
		fader.setDuration( getFaderDuration( hostInfo->getSampleRate() ) );  
		TOLOG ( "+" + getName() );
	}
	//--------------------------------------------------------------------------------------------------------
	//Lautstaerke-wert
	Parameter::Ptr volume;
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initValue
	 * @return neues Volume-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo, float initValue = 1.0f ) {
		Ptr neu( new Volume(hostInfo, initValue ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Volume-Parameter geaendert.
	 * @param src
	 * @param value
	 */
	virtual void valueChanged ( void *src, const float &value ) { fader = value; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * HostInfo(Samplerate/Blocksize) geaendert.
	 */
	virtual void hostInfoChanged() {
		fader.setDuration( getFaderDuration( hostInfo->getSampleRate() ) );  
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu verarbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param index
	 * @return ausshcliesslich Volumeparameter, da einzger Parameter.
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const { return volume; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Volume (){
		TOLOG ( "-" + getName() );
	}
};
//============================================================================================================
//	Klasse Pan:
//============================================================================================================
class Pan : public ProcessAdapter, public Serializable, public HasParameter  {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<Pan> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		ar & pan;
	}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr pan;
	//--------------------------------------------------------------------------------------------------------
	Pan() {}
protected:
	//--------------------------------------------------------------------------------------------------------
	Pan ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new Pan(hostInfo) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
    //ruft die processReplacing Methode des zugeordneten Processor Objekt auf.
    virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const { return pan; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Pan() {
		TOLOG ( "-" + getName() );
	}

};
//============================================================================================================
// Klasse Switch:
// verwaltet N FadeValue und dazugehoerige Parameter
//============================================================================================================
class Switch : public HasParameter {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef vector<FadeValue> FadeValues;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Parameter::Ptr> Parameters;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & numStates;
		ar & nFader;
		ar & nDurationIN;
		ar & nDurationOUT;
		ar & nCurveTypeIN;
		ar & nCurveTypeOUT;
		ar & parameterMap;
		ar & state;
		if ( Archive::is_loading::value ) { // rebind func listener
			Parameter::ParameterListenerFunction dI = boost::bind( 
				&Switch::durationINChanged, this, _1, _2 
			);
			Parameter::ParameterListenerFunction dO = boost::bind( 
				&Switch::durationOUTChanged, this, _1, _2 
			);
			Parameter::ParameterListenerFunction cT = boost::bind( 
				&Switch::curveTypeChanged, this, _1, _2 
			);

			for ( int i=0; i<numStates; ++i ) {
				nDurationIN[i]->addValueChangedListenerF ( dI );
				nDurationOUT[i]->addValueChangedListenerF ( dO );
				nCurveTypeIN[i]->addValueChangedListenerF ( cT );
				nCurveTypeOUT[i]->addValueChangedListenerF ( cT );
			}
		}
	}
	//--------------------------------------------------------------------------------------------------------
	size_t numStates;
	//--------------------------------------------------------------------------------------------------------
	enum { NUM_P = 4 };
	//--------------------------------------------------------------------------------------------------------
	size_t state;
	//--------------------------------------------------------------------------------------------------------
	void durationINChanged ( void *src, const float &v );
	//--------------------------------------------------------------------------------------------------------
	void durationOUTChanged ( void *src, const float &v );
	//--------------------------------------------------------------------------------------------------------
	void curveTypeChanged ( void *src, const float &v );
	//--------------------------------------------------------------------------------------------------------
	float one_ms_in_samples;
	//--------------------------------------------------------------------------------------------------------
	FadeValues nFader;
	//--------------------------------------------------------------------------------------------------------
	Parameters nDurationIN, nDurationOUT, nCurveTypeIN, nCurveTypeOUT, parameterMap; 
	//--------------------------------------------------------------------------------------------------------
	void _addState();
public:
	//--------------------------------------------------------------------------------------------------------
	void setSampleRate ( float sampleRate ) {
		one_ms_in_samples = sampleRate / 1000.0f;
	}
	//--------------------------------------------------------------------------------------------------------
	Switch( size_t numStates = 0, float sampleRate = 0.0f );
	//--------------------------------------------------------------------------------------------------------
	void setState ( size_t x ) {
		setFaderValue(state, 0.0f);
		state = x % numStates; 
		setFaderValue(state, 1.0f);
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void addState();
	//--------------------------------------------------------------------------------------------------------
	void nextState() { setState( state + 1 ); }
	//--------------------------------------------------------------------------------------------------------
	void prevState() { setState( state - 1 ); }
	//--------------------------------------------------------------------------------------------------------
	size_t getState() const { return state; }
	//--------------------------------------------------------------------------------------------------------
	size_t getNumStates() const { return numStates; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~Switch();
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr ) const {  return parameterMap.at(nr); }
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr getDurationParameterIN ( size_t nr ) const {  return nDurationIN.at(nr); }
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr getDurationParameterOUT ( size_t nr ) const {  return nDurationOUT.at(nr); }
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr getCurveTypeParameterIN ( size_t nr ) const {  return nCurveTypeIN.at(nr); }
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr getCurveTypeParameterOUT ( size_t nr ) const {  return nCurveTypeOUT.at(nr); }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return NUM_P*numStates; }
	//--------------------------------------------------------------------------------------------------------
	const FadeValue::T & getFaderValueAndIncT ( size_t n ) { return nFader[n].getValue(); } 
	//--------------------------------------------------------------------------------------------------------
	void setFaderValue ( size_t n, const FadeValue::T &v ) { 
		assert ( one_ms_in_samples != 0.0f );
		bool out = v<nFader[n];
		float d = out ? *nDurationOUT[n] : *nDurationIN[n]; // fadein : fadeout
		int t = mapInteger ( ( out ? *nCurveTypeOUT[n] : *nCurveTypeIN[n] ), FadeValue::NUM_FADE_TYPES );
		nFader[n].setDuration ( one_ms_in_samples * 1000.0f * d); 
		nFader[n].setType ( (FadeValue::FadeType) t );
		nFader[n].setValue(v);
	} 
	//--------------------------------------------------------------------------------------------------------
	FadeValue * getFader( size_t n ) { return &nFader[n]; } 
};
//============================================================================================================
//	Klasse OutputSwitch :
//	Schaltet mehrere Ausgaenge
//============================================================================================================
class OutputSwitch : 
public ProcessAdapter, 
public Serializable, 
public Switch,
public VariableOutputAdapter,
public ValueChangedListener<float>,
public IHasState
{
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<OutputSwitch> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	OutputSwitch() {};
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> OutputMatrix;
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	OutputMatrix outpMatrix;
	//--------------------------------------------------------------------------------------------------------
	inline void _processFrames ( Frames *iFrame, OutputMatrix &fr, Processor::Int numSamples );
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr selector;
	//--------------------------------------------------------------------------------------------------------
	vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	virtual void setState ( size_t state ) { Switch::setState(state); }
	//--------------------------------------------------------------------------------------------------------
	OutputSwitch( IHostInfo *hostInfo, int initStates = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo, int initStates = 2 ) {
		Ptr neu( new OutputSwitch(hostInfo, initStates) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
    virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~OutputSwitch();
	//--------------------------------------------------------------------------------------------------------
	size_t getState() const { return aNode->isActive() ? Switch::getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged() {
		Switch::setSampleRate( hostInfo->getSampleRate() );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const {return parameterMap.at(nr);}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ProcessorNode::Ptr addOutputNode();
};
//============================================================================================================
//	Klasse InputSwitch :
//	Schaltet mehrere Eingaenge
//============================================================================================================
class InputSwitch : 
public ProcessAdapter, 
public Serializable, 
public Switch,
public VariableInputAdapter,
public ValueChangedListener<float>,
public IHasState
{
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<InputSwitch> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	InputSwitch() {};
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> InputMatrix;
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	InputMatrix inputMatrix;
	//--------------------------------------------------------------------------------------------------------
	Frames tmpFrame;
	//--------------------------------------------------------------------------------------------------------
	inline void _processFrames ( InputMatrix &fr, Processor::Int numSamples );
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr selector;
	//--------------------------------------------------------------------------------------------------------
	vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	virtual void setState ( size_t state ) { Switch::setState(state); }
	//--------------------------------------------------------------------------------------------------------
	InputSwitch( IHostInfo *hostInfo, int initStates = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo, int initStates = 2 ) {
		Ptr neu( new InputSwitch(hostInfo, initStates) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged() {
		tmpFrame.setSize( hostInfo->getBlockSize() );
		Switch::setSampleRate( hostInfo->getSampleRate() );
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
    virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~InputSwitch();
	//--------------------------------------------------------------------------------------------------------
	size_t getState() const { return aNode->isActive() ? Switch::getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const {return parameterMap.at(nr);}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ProcessorNode::Ptr addInputNode();
};
//============================================================================================================
// Klasse Step:
// Verwaltet Step zustaende und zugehoerige Multiplikations Faktoren.
//============================================================================================================
class Step : public Switch {
friend class boost::serialization::access;
public:
	//----------------------------------------------------------------------------------------------------
	typedef FadeValue FactorType;
private:
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object<Switch> ( *this );
		ar & currTranslator;
		ar & steps;
		ar & duration;
		ar & nDuration;
		//:::::::::::::::IF_LOADING::::::::::::::::::::::::::::::::
		if ( !Archive::is_loading::value ) return;
		Parameter::ParameterListenerFunction f = boost::bind( 
			&Step::durationParameterChanged, this, _1, _2 
		);
		for (int i=0; i<steps; ++i) {
			nDuration[i]->addValueChangedListenerF ( f );
		}
	}
	//----------------------------------------------------------------------------------------------------
	Step() {}
	//----------------------------------------------------------------------------------------------------
	ValueTranslator *currTranslator;
	//----------------------------------------------------------------------------------------------------
	int steps;
	//----------------------------------------------------------------------------------------------------
	int duration;
	//----------------------------------------------------------------------------------------------------
	vector<Parameter::Ptr> nDuration;
	//----------------------------------------------------------------------------------------------------
	void durationParameterChanged ( void *src, const float &v );
public:
	//----------------------------------------------------------------------------------------------------
	void setDuration();
	//--------------------------------------------------------------------------------------------------------
	int skimDuration() { return duration--; }
	//----------------------------------------------------------------------------------------------------
	void setValueTranslator ( ValueTranslator *tr ){ currTranslator = tr; }
	//----------------------------------------------------------------------------------------------------
	void resetLabel();
	//----------------------------------------------------------------------------------------------------
	virtual void addState();
	//----------------------------------------------------------------------------------------------------
	ValueTranslator * getValueTranslator (){ return currTranslator; }
	//----------------------------------------------------------------------------------------------------
	Parameter::Ptr getParameter ( size_t index ){ return nDuration[index]; }
	//----------------------------------------------------------------------------------------------------
	void setOffset ( int off )  { duration+=off; }
	//----------------------------------------------------------------------------------------------------
	void setSampleRate( float sampleRate ) {
		Switch::setSampleRate( sampleRate );
	}
	//----------------------------------------------------------------------------------------------------
	Step( ValueTranslator*, size_t initSteps = 2, float sampleRate = 0.0f );
	//----------------------------------------------------------------------------------------------------
	size_t getState() const { return Switch::getState(); }
	//----------------------------------------------------------------------------------------------------
	void reset(){
		setFaderValue(getState(), 0.0f);
		setState (0);
		setDuration();
		setFaderValue(getState(), 1.0f);
	}
	//----------------------------------------------------------------------------------------------------
	void nextStep(){ setState ( ( getState() + 1 ) % getNumSteps() ); }
	//----------------------------------------------------------------------------------------------------
	void setNumSteps( int n ) { steps = n; }
	//----------------------------------------------------------------------------------------------------
	int getNumSteps() { return steps; }
	//----------------------------------------------------------------------------------------------------
	~Step();
};

//============================================================================================================
//	Schnittstelle ValueTranslator:
//	Transformiert einen eingabewert im bereich 0..1 in eine Zeitangabe in Samples.
//============================================================================================================
class ValueTranslator {
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){}
protected:
public:
	//--------------------------------------------------------------------------------------------------------
	// liefert Parameter wer als String zb. 500 oder 1/4 , ...
	virtual MyString  translateAsString ( float v ) = 0; 
	//--------------------------------------------------------------------------------------------------------
	virtual int translate( float v ) = 0; 
	//--------------------------------------------------------------------------------------------------------
	virtual ~ValueTranslator(){}
};
//============================================================================================================
//	TimeProcessor
//  Schaltintervalle sind zeitgesteuert von 1 ms bis x ms.  
//============================================================================================================
template < int MIN_IN_MSEC >
class FixTimeValue : public ValueTranslator {
friend class boost::serialization::access;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ValueTranslator > ( *this );
	}
private:
	//--------------------------------------------------------------------------------------------------------
	float oneMsInSamples;
	//--------------------------------------------------------------------------------------------------------
	float minInSampl;
	//--------------------------------------------------------------------------------------------------------
	float fak;
public:
	//--------------------------------------------------------------------------------------------------------
	FixTimeValue( float sampleRate ) : 
	  oneMsInSamples ( sampleRate/1000.0f ),
	  minInSampl ( oneMsInSamples * MIN_IN_MSEC ),
	  fak(99.0f) {}
	//--------------------------------------------------------------------------------------------------------
	void hostInfoChanged( float sampleRate ) {
		oneMsInSamples = sampleRate/1000.0f; 
		minInSampl = oneMsInSamples * MIN_IN_MSEC;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual MyString  translateAsString ( float v ) {
		assert ( oneMsInSamples != 0.0f );
		float y = fak * v + 1.0f; // abbildung 0.0..1.0 => 1.0 100.0
		return MyString(y*minInSampl/oneMsInSamples) + " ms";
	}
	//--------------------------------------------------------------------------------------------------------
	virtual int translate ( float v ){
		return (int)( (fak * v + 1.0f) * minInSampl);
	}
};

//============================================================================================================
//	TimeProcessor
//  Schaltintervalle sind zeitgesteuert von 1 ms bis x ms.  
//============================================================================================================
class SyncTranslator : public ValueTranslator {
friend class boost::serialization::access;
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ValueTranslator > ( *this );
		ar & hostInfo;
	}
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator() {}
private:
	//--------------------------------------------------------------------------------------------------------
	float oneMsInSamples;
	//--------------------------------------------------------------------------------------------------------
	IHostInfo * hostInfo;
public:
	//--------------------------------------------------------------------------------------------------------
	void hostInfoChanged() {
		oneMsInSamples = hostInfo->getSampleRate()/1000.0f; 
	}
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator( IHostInfo * hostInfo ): 
	hostInfo(hostInfo), 
	oneMsInSamples ( hostInfo->getSampleRate()/1000.0f ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual MyString  translateAsString ( float v ) {
		int n = mapInteger ( v, musicalValues::NUM_STDNOTES );
		return musicalValues::noteLengthTable[n].str;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual int translate ( float v ){
		int n = mapInteger ( v, musicalValues::NUM_STDNOTES );
		VstTimeInfo *inf = hostInfo->getVstTimeInfo( kVstTempoValid );
		return note2Sample ( musicalValues::noteLengthTable[n].val, inf->tempo, inf->sampleRate );
	}
};
//============================================================================================================
//	Klasse OutputStep:
//	Hatt mehrere Ausgaenge. Zuordung des Input-Signals ist zustands abhaengig.
//============================================================================================================
class OutputStep: 
public ProcessAdapter, 
public HasParameter, 
public Serializable, 
public VariableOutputAdapter,
public IHasState
{
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<OutputStep> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	OutputStep() : fixTimeValue(0.0f) {};
private:
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	ClockEdge transport;
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr type;
	//--------------------------------------------------------------------------------------------------------
	void init();
	//--------------------------------------------------------------------------------------------------------
	inline void reset();
	//--------------------------------------------------------------------------------------------------------
	void typeChanged ( void *src, const float& v );
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> OutputMatrix;
	//--------------------------------------------------------------------------------------------------------
	OutputMatrix outpMatrix;
	//--------------------------------------------------------------------------------------------------------
	// Dupliziert iFrame nach *fr[numSteps] * step_Faktor
	inline void processFrames ( Frames *iFrame, OutputMatrix &fr, Processor::Int numSamples );
	//----------------------------------------------------------------------------------------------------
	// skims one sample of step duration samples and switches to next state if duration samples == 0
	void skimStepDuration() {
		if ( cStep->skimDuration() <= 0 ) {
			cStep->nextStep();
			cStep->setDuration();
		}
	}
protected:
	//--------------------------------------------------------------------------------------------------------
	// anzahl der Steps == Ausgaenge == anz. der ProcessAdapterNode
	int steps;
	//--------------------------------------------------------------------------------------------------------
	vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	Step *cStep;
	//--------------------------------------------------------------------------------------------------------
	FixTimeValue<10> fixTimeValue;
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator *sync;
	//--------------------------------------------------------------------------------------------------------
	virtual void setState ( size_t ignore ) {} 
	//--------------------------------------------------------------------------------------------------------
	OutputStep( IHostInfo *hostInfo, int initSteps = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo, int initSteps = 2 ) {
		Ptr neu( new OutputStep(hostInfo, initSteps) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged() {
		fixTimeValue.hostInfoChanged( hostInfo->getSampleRate() );
		if (sync) sync->hostInfoChanged();
		cStep->setSampleRate ( hostInfo->getSampleRate() );
	}
	//--------------------------------------------------------------------------------------------------------
    //ruft die processReplacing Methode des zugeordneten Processor Objekt auf.
    virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~OutputStep();
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const {return parameterMap.at(nr);}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getState() const { return aNode->isActive() ? cStep->getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	ProcessorNode::Ptr addOutputNode();
};
//============================================================================================================
//	Klasse InputStep:
//	Hatt mehrere Ausgaenge. Zordung des Input-Signals ist zustands abhaengig.
//============================================================================================================
class InputStep: 
public ProcessAdapter, 
public HasParameter, 
public Serializable, 
public VariableInputAdapter,
public IHasState
{
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<InputStep> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	InputStep() : fixTimeValue(0.0f) {}
private:
	//--------------------------------------------------------------------------------------------------------
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	ClockEdge transport;
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr type;
	//--------------------------------------------------------------------------------------------------------
	void init();
	//--------------------------------------------------------------------------------------------------------
	Frames tmpFrame;
	//--------------------------------------------------------------------------------------------------------
	inline void reset();
	//--------------------------------------------------------------------------------------------------------
	void typeChanged ( void *src, const float& v );
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> InputMatrix;
	//--------------------------------------------------------------------------------------------------------
	InputMatrix inputMatrix;
	//--------------------------------------------------------------------------------------------------------
	// Dupliziert iFrame nach *fr[numSteps] * step_Faktor
	inline void processFrames ( InputMatrix &fr, Processor::Int numSamples );
protected:
	//--------------------------------------------------------------------------------------------------------
	// anzahl der Steps == Ausgaenge == anz. der ProcessAdapterNode
	int steps;
	//--------------------------------------------------------------------------------------------------------
	vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	Step *cStep;
	//--------------------------------------------------------------------------------------------------------
	FixTimeValue<10> fixTimeValue;
	//--------------------------------------------------------------------------------------------------------
	SyncTranslator *sync;
	//--------------------------------------------------------------------------------------------------------
	virtual void setState ( size_t ignore ) {} 
	//--------------------------------------------------------------------------------------------------------
	InputStep( IHostInfo *hostInfo, int initSteps = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo, int initSteps = 2 ) {
		Ptr neu( new InputStep(hostInfo, initSteps) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged() {
		fixTimeValue.hostInfoChanged( hostInfo->getSampleRate() );
		tmpFrame.setSize( hostInfo->getBlockSize() );
		cStep->setSampleRate ( hostInfo->getSampleRate() );
		if (sync) sync->hostInfoChanged();
	}
	//--------------------------------------------------------------------------------------------------------
    //ruft die processReplacing Methode des zugeordneten Processor Objekt auf.
    virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~InputStep();
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const {return parameterMap.at(nr);}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getState() const { return aNode->isActive() ? cStep->getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	ProcessorNode::Ptr addInputNode();
};
//============================================================================================================
//PeakTracker
//Tranformiert Signal Lautstaerke in Parameter wert.
//Sammelt N viele werte und bildet daraus Mittelwert. Ist dieser ermittelt wird er an out uebergeben.
//============================================================================================================
class PeakTracker : public ProcessAdapter, public HasParameter, public Serializable {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<PeakTracker> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ProcessAdapter > ( *this );
		ar & offset;
		ar & out;
	}
	//--------------------------------------------------------------------------------------------------------
	PeakTracker (){}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr offset;
protected:
	//--------------------------------------------------------------------------------------------------------
	PeakTracker ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new PeakTracker(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	//Lautstaerke-wert
	Parameter::Ptr out;
	//--------------------------------------------------------------------------------------------------------
	//ruft die processReplacing Methode des zugeordneten 
	//Processor Objekt auf.
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const { return offset; }
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr getOutParameter() { return out; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return 1; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~PeakTracker (){ 
		TOLOG ( "-" + getName() );
	}
};

//============================================================================================================
// ADSRTrigger
// Triggert Eingangs Signal als gate fuer adsr. 
//============================================================================================================
class ADSRTrigger : public ProcessAdapter, public HasParameter, public Serializable {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<ADSRTrigger> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ){
		ar & boost::serialization::base_object < ProcessAdapter > ( *this );
		ar & out;
		ar & adsr;
	}
	//--------------------------------------------------------------------------------------------------------
	ADSRTrigger (){}
	//--------------------------------------------------------------------------------------------------------
	ADSR *adsr;
protected:
	//--------------------------------------------------------------------------------------------------------
	ADSRTrigger ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new ADSRTrigger(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	//Lautstaerke-wert
	Parameter::Ptr out;
	//--------------------------------------------------------------------------------------------------------
	virtual void hostInfoChanged() { 
		adsr->hostInfoChanged();
	}
	//--------------------------------------------------------------------------------------------------------
	//ruft die processReplacing Methode des zugeordneten 
	//Processor Objekt auf.
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const { 
		if ( nr > adsr->getNumParameter() ) return Parameter::Ptr();
		return adsr->getParameter(nr); 
	}
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return adsr->getNumParameter(); }
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr getOutParameter() { return out; }
	//--------------------------------------------------------------------------------------------------------
	virtual ~ADSRTrigger (){ 
		delete adsr; 
		TOLOG ( "+" + getName() );
	}
};
//============================================================================================================
// MidiProcessor:
// One MidiProcessor per channel.
//============================================================================================================
class MidiProcessor : 
public ProcessAdapter, 
public HasParameter, 
public Serializable,
public MidiEventProcessor,
public events::ValueChangedListener<float>
{
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<MidiProcessor> Ptr;
	//--------------------------------------------------------------------------------------------------------
	typedef unsigned char Byte;
private:
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr channelSelector;
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Parameter::Ptr> Parameters;
	//--------------------------------------------------------------------------------------------------------
	enum NoCCParameterIndex { PITCH_BEND, NUM_NO_CC_PARAMETER };
	//--------------------------------------------------------------------------------------------------------
	enum { NUM_CC = 128, NUM_OUT_PARAM = NUM_CC + NUM_NO_CC_PARAMETER };
	//--------------------------------------------------------------------------------------------------------
	Parameters midiParameters;
	//--------------------------------------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object < ProcessAdapter > ( *this );
		ar & midiParameters; 
	}
	//--------------------------------------------------------------------------------------------------------
	void initParameter( size_t index, const std::string &name ) {
		midiParameters[index] = Parameter::create( index );
		midiParameters[index]->setName ( name );
	}
	//--------------------------------------------------------------------------------------------------------
	MidiProcessor (){} // wird nur von boost::serial. benutzt
protected:
	//--------------------------------------------------------------------------------------------------------
	MidiProcessor ( IHostInfo *hostInfo );
public:
	//--------------------------------------------------------------------------------------------------------
	virtual void processMidiEvents( VstEvents * events );
	//--------------------------------------------------------------------------------------------------------
	static Ptr create( IHostInfo *hostInfo ) {
		Ptr neu( new MidiProcessor(hostInfo ) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const float &value ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getMidiChannelParameter() { return channelSelector; }
	//--------------------------------------------------------------------------------------------------------
	virtual void processAdapter( Processor::Int numSamples ) {}
	//--------------------------------------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const { return midiParameters[nr]; }
	//--------------------------------------------------------------------------------------------------------
	virtual size_t getNumParameter () const { return midiParameters.size(); }
	//--------------------------------------------------------------------------------------------------------
	virtual ~MidiProcessor (){
		TOLOG ( "-" + getName() );
	}
};
}// namespace processing

#endif


