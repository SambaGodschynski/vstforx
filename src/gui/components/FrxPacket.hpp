/*
 * FrxPacket.hpp
 *
 *  Created on: Tue Jun 24 10:10:15 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPACKET_H
#define SAMBAG_FRXPACKET_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include "FrxNode.hpp"
#include <sambag/com/events/PropertyChanged.hpp>
#include <gui/HandyNamespaces.hpp>
#include "FrxSelection.hpp"
#include <list>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxPacket.
  */
class FrxPacket : public FrxNode {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNode Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxPacket> Ptr;
    //-------------------------------------------------------------------------
    typedef std::pair<FrxComponentPtr, sd::Point2D> Content;
    //-------------------------------------------------------------------------
    typedef std::list<Content> Container;
protected:
    //-------------------------------------------------------------------------
    virtual void postConstructor();
	//-------------------------------------------------------------------------
	FrxPacket() {}
    //-------------------------------------------------------------------------
    void onPropertyChanged(const sce::PropertyChanged &ev);
    //-------------------------------------------------------------------------
    void onBoundChanged();
    //-------------------------------------------------------------------------
    void updateComponentLocation(sdc::AComponentPtr c);
private:
    //-------------------------------------------------------------------------
    Container container;
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) { 
		ar & boost::serialization::base_object<Super>(*this);
        ar & container;
    }
public:
	//-------------------------------------------------------------------------
	virtual ~FrxPacket();
    //-------------------------------------------------------------------------
    /**
     * @brief Add element to packet.
     */
    void add(FrxComponentPtr c);
    //-------------------------------------------------------------------------
    /**
     * @brief unpack all containing elements
     */
    void unpack();
	//-------------------------------------------------------------------------
	static Ptr create();
    //-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
}; // FrxPacket
}}}
#endif /* SAMBAG_FRXPACKET_H */
