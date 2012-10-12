/*
 * FrxSelection.hpp
 *
 *  Created on: Tue Aug 28 10:30:08 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXSELECTION_H
#define SAMBAG_FRXSELECTION_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/AContainer.hpp>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <list>

namespace frx { namespace gui { namespace components {
namespace sdc = sambag::disco::components;
namespace sdcu = sdc::ui;
class FrxCircuidView;
//=============================================================================
/** 
  * @class FrxSelection.
  * A selection for several objects.
  */
class FrxSelection : public sdc::AComponent {
//=============================================================================
friend class FrxCircuidView;
public:
	//-------------------------------------------------------------------------
	typedef sdc::AComponent Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxSelection> Ptr;
	//-------------------------------------------------------------------------
	typedef std::list<sdc::AComponent::WPtr> ContentContainer;
protected:
	//-------------------------------------------------------------------------
	FrxSelection(){ 
		setName("FrxSelection");
		setVisible(false); 
	}
	//-------------------------------------------------------------------------
	ContentContainer content;
	//-------------------------------------------------------------------------
	void extendBounds(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxSelection)
private:
	///////////////////////////////////////////////////////////////////////////
	// Archive:
	//-------------------------------------------------------------------------
	friend class boost::serialization::access;
	//-------------------------------------------------------------------------
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) {
	} 
public:
	//-------------------------------------------------------------------------
	void updateBounds();
	//-------------------------------------------------------------------------
	void clearContent();
	//-------------------------------------------------------------------------
	/**
	 * set selection content.
	 */
	void setContent(const ContentContainer &container);
	//-------------------------------------------------------------------------
	void addElement(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	template <class Container>
	void addElements(const Container &c);
	//-------------------------------------------------------------------------
	const ContentContainer & getContent() const { return content; }
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const {
		return laf->getUI<FrxSelection>();
	}
}; // FrxSelection
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class Container>
void FrxSelection::addElements(const Container &c) {
	typename Container::const_iterator it = c.begin();
	for ( ; it!=c.end(); ++it ) {
		addElement(*it);
	}
}
}}} // namespace(s)

#endif /* SAMBAG_FRXSELECTION_H */
