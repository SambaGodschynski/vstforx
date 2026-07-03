/*
 * ============================================================================
 * Volume.h
 *      Author: Johannes Unger
 * ============================================================================
 */
#ifndef FORX_DC_TESTER_HPP
#define FORX_DC_TESTER_HPP

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/SerializationFwd.h"
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/ModelFactory.hpp>

namespace processing {
using namespace parameter;
//=============================================================================
/**
 * @class DCTester.
 * creates a delay between input and output for testing purpose
 */
class DCTester :
//=============================================================================
public ProcessAdapter, 
public HasParameter, 
public com::Serializable
{
friend class boost::serialization::access;
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<DCTester> Ptr;
private:
    //-------------------------------------------------------------------------
    DCStream stream;
    //-------------------------------------------------------------------------
    void setupStream(frx::processing::IHostInfo::Ptr hI);
	//-------------------------------------------------------------------------
	/**
	 * Initalisiert Listener.
	 */
	void initListener() {
		delay->addValueChangedListener (
			boost::bind(&DCTester::valueChanged, this, _1, _2)
		);
	}
	//-------------------------------------------------------------------------
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version ) {
		ar & boost::serialization::base_object< ProcessAdapter > ( *this );
		ar & delay;
		if ( Archive::is_loading::value ) {
			frx::processing::IHostInfo::Ptr hI = hostInfo.lock();
			if (!hI) {
				SAMBAG_THROW(sambag::com::exceptions::IllegalStateException,
					"Hostinfo == NULL"
				);
			}
            setupStream(hI);
			initListener();
		}
	}
	//-------------------------------------------------------------------------
	DCTester (){} // wird nur von boost::serial. benutzt
protected:
	//-------------------------------------------------------------------------
	DCTester ( frx::processing::IHostInfo::Ptr hostInfo ) :
		ProcessAdapter( hostInfo, 1, 1 ) 
	{
		setName ("DC_Tester");
		delay = Parameter::create();
		delay->setName ("Delay");
		getInputNode(0)->setName ("DCTester Input Node");
		getOutputNode(0)->setName ("DCTester Output Node");
        setupStream(hostInfo);
	}
	//-------------------------------------------------------------------------
	Parameter::Ptr delay;
public:
    //-------------------------------------------------------------------------
    virtual size_t getProcessDelay() const;
	//-------------------------------------------------------------------------
	/**
	 * @param hostInfo
	 * @param initValue
	 * @return neues Volume-Objekt
	 */
	static Ptr create( frx::processing::IHostInfo::Ptr hostInfo ) {
		Ptr neu( new DCTester(hostInfo) );
		neu->self = neu;
		neu->initListener();
		return neu;
	}
	//-------------------------------------------------------------------------
	virtual void valueChanged ( void *src, const float &value );
	//-------------------------------------------------------------------------
	/**
	 * HostInfo(Samplerate/Blocksize) geaendert.
	 */
	virtual void hostBaseConfigChanged();
	//-------------------------------------------------------------------------
	virtual void processAdapter( Processor::Int numSamples );
	//-------------------------------------------------------------------------
	virtual Parameter::Ptr getParameter ( size_t index = 0 ) const { return delay; }
	//-------------------------------------------------------------------------
	/**
	 *
	 * @return 1
	 */
	virtual size_t getNumParameter () const { return 1; }
	//-------------------------------------------------------------------------
	virtual ~DCTester () {}
};


FRX_MODELFACTORY_REGISTER(private_int, DCTester);
namespace { const bool DCTester_Registered =   
    frx::processing::ModelFactory::instance().register_<DCTester>( 
        "internal-private.DCTester", boost::bind(&DCTester::create, _1)
    );
}

}// namespace processing

#endif  // FORX_DC_TESTER_HPP


