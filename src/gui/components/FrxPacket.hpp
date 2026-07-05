/*
 * FrxPacket.hpp
 *
 *  Created on: Tue Jun 24 10:10:15 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPACKET_H
#define SAMBAG_FRXPACKET_H

#include <memory>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include "FrxNode.hpp"
#include <sambag/com/events/PropertyChanged.hpp>
#include <gui/HandyNamespaces.hpp>
#include "FrxSelection.hpp"
#include <list>
#include "FrxCircuidView.hpp"
#include <gui/components/FrxFlag.hpp>

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
	typedef std::shared_ptr<FrxPacket> Ptr;
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
    void updateComponentLocation(FrxComponentPtr c);
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
    const Container & getPacketContainer() const {
        return container;
    }
	//-------------------------------------------------------------------------
	virtual ~FrxPacket();
    //-------------------------------------------------------------------------
    /**
     * @brief Add element to packet.
     */
    void add(FrxComponentPtr c);
    //-------------------------------------------------------------------------
    /**
     * @brief Packs content.
     */
    void pack();
    //-------------------------------------------------------------------------
    /**
     * @brief unpack all containing elements
     */
    void unpack();
	//-------------------------------------------------------------------------
	static Ptr create();
	//-------------------------------------------------------------------------
	/**
     * @brief packs content and add packet to view
     */
    template <class STL>
    static Ptr create(FrxCircuidViewPtr view, const STL &c);
    //-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const;
}; // FrxPacket

///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
    template <typename T>
    std::shared_ptr<T> __getPtr(std::shared_ptr<T> x) {
        return x;
    }
    template <typename T>
    std::shared_ptr<T> __getPtr(std::weak_ptr<T> x) {
        return x.lock();
    }
} // namespace
template <class STL>
FrxPacket::Ptr FrxPacket::create(FrxCircuidViewPtr view, const STL &c) {
    FrxPacket::Ptr packet = FrxPacket::create();
    // add elements to packet
    BOOST_FOREACH(typename STL::value_type x, c) {
        FrxComponent::Ptr comp =
            std::dynamic_pointer_cast<FrxComponent>(
                __getPtr(x)
            );
        if (!comp) {
            continue;
        }
        packet->add(comp);
    }
    packet->pack();
    // add packet to view
    view->add(packet, FrxCircuidView::Z_ProcessorNodes);
    FrxFlag::Ptr flag = FrxFlag::create();
    flag->setTarget(packet);
    view->add(flag, FrxCircuidView::Z_Flags, true);
    // update name
    std::stringstream ss;
    ss<<"Packet ["<<packet->getPacketContainer().size()<<" items]";
    packet->setName(ss.str());
    
    return packet;
}
}}}
#endif /* SAMBAG_FRXPACKET_H */
