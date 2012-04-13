/*
 * ===========================================================================================================
 * InputSwitch.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_INPUTSWITCH_H
#define FORX_INPUTSWITCH_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "com/One4All.h"
#include "Switch.h"

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class InputSwitch.
 * Schaltet N Eingangs-Samplemengen auf 1 Ausgang
 */
class InputSwitch :
//============================================================================================================
public ProcessAdapter, 
public Serializable, 
public Switch,
public VariableInputAdapter,
public IHasState
{
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<InputSwitch> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert IntputSwich-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert IntputSwich-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	InputSwitch() {};
	//--------------------------------------------------------------------------------------------------------
	typedef vector<Frames*> InputMatrix;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockt hinzufuegen von InputNode gegen processAdapter()
	 */
	Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	InputMatrix inputMatrix;
	//--------------------------------------------------------------------------------------------------------
	Frames tmpFrame;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param iFrame Eingangs-Frames-Objekt
	 * @param fr Frames-OutputMatrix
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	inline void _processFrames ( InputMatrix &fr, Processor::Int numSamples );
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr selector;
	//--------------------------------------------------------------------------------------------------------
	vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt State
	 * @param state
	 */
	virtual void setState ( size_t state ) { Switch::setState(state); }
	//--------------------------------------------------------------------------------------------------------
	InputSwitch( IHostInfo *hostInfo, int initStates = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initStates
	 * @return neues InputSwitch-Objekt
	 */
	static Ptr create( IHostInfo *hostInfo, int initStates = 2 ) {
		Ptr neu( new InputSwitch(hostInfo, initStates) );
		neu->self = neu;
		return neu;
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * HostInfo geandert. Beeinflusst Switch::Fader
	 */
	virtual void hostInfoChanged() {
		tmpFrame.setSize( hostInfo->getBlockSize() );
		Switch::setSampleRate( hostInfo->getSampleRate() );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Selector-Parameter geandert
	 * @param src
	 * @param val neuer Wert
	 */
	virtual void valueChanged ( void *src, const float &val );
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Verarbeitet Samplemenge der Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~InputSwitch();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuellen State
	 */
	size_t getState() const { return aNode->isActive() ? Switch::getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: std::out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t nr = 0 ) const {return parameterMap.at(nr);}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller InputSwitch Parameter
	 */
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt InputNode hinzu
	 * @return neues InputNode-Objekt
	 */
	virtual ProcessorNode::Ptr addInputNode();
};
}// namespace processing

#endif  // FORX_INPUTSWITCH_H


