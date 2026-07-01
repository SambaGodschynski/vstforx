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
#include "com/one4All.h"
#include "Switch.h"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/ModelFactory.hpp>

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
public com::Serializable, 
public Switch,
public VariableInputAdapter,
public com::IHasState
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
	void save ( com::oArchive &ar, const unsigned int version ) const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * Deserialisiert IntputSwich-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	void load ( com::iArchive &ar, const unsigned int version );
	//--------------------------------------------------------------------------------------------------------
	InputSwitch() {};
	//--------------------------------------------------------------------------------------------------------
	typedef std::vector<Frames*> InputMatrix;
	//--------------------------------------------------------------------------------------------------------
	/**
	 * blockt hinzufuegen von InputNode gegen processAdapter()
	 */
	com::Mutex mutex;
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
	virtual void stateChanged(State old, State _new) {
		sce::EventSender<sce::PropertyChanged>::notifyListeners(
			this, 
			sce::PropertyChanged(PROPERTY_SWITCH_STATE, SwitchState(true,old), SwitchState(true,_new))
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
	virtual void setState ( size_t state ) { Switch::setState(state); }
	//--------------------------------------------------------------------------------------------------------
	InputSwitch( frx::processing::IHostInfo::Ptr hostInfo, int initStates = 2 );
public:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initStates
     * @param not used, needed to fit ModelFactory's create function signature
	 * @return neues InputSwitch-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo, int initStates=2, int notUsed=0 ) {
		Ptr neu( new InputSwitch(hostInfo, initStates) );
		neu->self = neu;
		return neu;
	}
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
		tmpFrame.setSize( hI->getBlockSize() );
		Switch::setSampleRate( hI->getSampleRate() );
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

namespace {
    const bool INTERNAL_INSWITCH_IO_Registered =
        frx::processing::ModelFactory::instance().
            registerWithIO<InputSwitch>("internal.InputSwitch", &InputSwitch::create);
    
    const bool INTERNAL_INSWITCH_Registered =
        frx::processing::ModelFactory::instance().
		register_<InputSwitch>("internal.InputSwitch", boost::bind(&InputSwitch::create,_1,2,0));
}

}// namespace processing

#endif  // FORX_INPUTSWITCH_H


