/*
 * ===========================================================================================================
 * OutputSwitch.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_OUTPUTSWITCH_H
#define FORX_OUTPUTSWITCH_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include "Switch.h"
#include "com/One4All.h"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>

namespace processing {
using namespace parameter;
//============================================================================================================
/**
 * @class OutputSwitch :
 * Schaltet Eingangs-Samplemenge auf N Ausgaenge
 */
class OutputSwitch : 
public ProcessAdapter, 
public com::Serializable, 
public Switch,
public VariableOutputAdapter,
public com::IHasState
{
//============================================================================================================
friend class boost::serialization::access;
BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	//--------------------------------------------------------------------------------------------------------
	typedef boost::shared_ptr<OutputSwitch> Ptr;
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Serialisiert OutputSwich-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void save ( com::oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert OutputSwich-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( com::iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	OutputSwitch() {};
	//--------------------------------------------------------------------------------------------------------
	typedef std::vector<Frames*> OutputMatrix;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockt hinzufuegen von OutputNode gegen processAdapter()
	 */
	com::Mutex mutex;
	//--------------------------------------------------------------------------------------------------------
	OutputMatrix outpMatrix;
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param iFrame Eingangs-Frames-Objekt
	 * @param fr Frames-OutputMatrix
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	inline void _processFrames ( Frames *iFrame, OutputMatrix &fr, Processor::Int numSamples );
private:
protected:
	//--------------------------------------------------------------------------------------------------------
	virtual void stateChanged(State old, State _new) {
		sce::EventSender<sce::PropertyChanged>::notifyListeners(
			this, 
			sce::PropertyChanged(PROPERTY_SWITCH_STATE, SwitchState(false,old), SwitchState(false,_new))
		);
	}
	//--------------------------------------------------------------------------------------------------------
	Parameter::Ptr selector;
	//--------------------------------------------------------------------------------------------------------
	std::vector<Parameter::Ptr> parameterMap;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * setzt State
	 * @param state
	 */
	virtual void setState ( Switch::State state ) { Switch::setState(state); }
	//--------------------------------------------------------------------------------------------------------
	OutputSwitch( frx::processing::IHostInfo::Ptr hostInfo, int initStates = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initStates
	 * @return neues OutputSwitch-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo, int initStates = 2 ) {
		Ptr neu( new OutputSwitch(hostInfo, initStates) );
		neu->self = neu;
		return neu;
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
	 * Verarbeitet Samplemenge des Eingangsknoten und fuegt Ergebniss Ausgangsknoten hinzu.
	 * @param numSamples Anzahl der zu bearbeitenden Samples
	 */
	virtual void processAdapter( Processor::Int numSamples );
	//--------------------------------------------------------------------------------------------------------
	virtual ~OutputSwitch();
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return aktuellen State
	 */
	Switch::State getState() const { return aNode->isActive() ? Switch::getState() : UINT_MAX; }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * HostInfo geandert. Beeinflusst Switch::Fader
	 */
	virtual void hostBaseConfigChanged() {
		frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
		if (!hI) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
				"Hostinfo == NULL"
			);
		}
		Switch::setSampleRate( hI->getSampleRate() );
	}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param index
	 * @return liefert Parameter zu index. Wirft: out_of_range
	 */
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const {return parameterMap.at(index);}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return Anzahl aller OutputSwitch Parameter
	 */
	virtual size_t getNumParameter () const { return parameterMap.size(); }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Fuegt OutputNode hinzu
	 * @return neues OutputNode-Objekt
	 */
	virtual ProcessorNode::Ptr addOutputNode();
};
}// namespace processing

#endif  // FORX_OUTPUTSWITCH_H


