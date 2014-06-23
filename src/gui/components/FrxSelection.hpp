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
#include <sambag/com/ArithmeticWrapper.hpp>
#include "IFormatter.hpp"
#include <list>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
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
	virtual void postConstructor();
	//-------------------------------------------------------------------------
	ContentContainer content;
	//-------------------------------------------------------------------------
	void extendBounds(sdc::AComponent::Ptr c);
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxSelection)
	//-------------------------------------------------------------------------
	/**
	 * we need to distinguish whether content was selected or
	 * added by browser.
	 */
	IFormatter::Ptr formatter;
	sambag::com::ArithmeticWrapper<bool> contentViaSelection;
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
	IFormatter::Ptr getFormatter() const {
		return formatter;
	}
	//-------------------------------------------------------------------------
	void setFormatter(IFormatter::Ptr fomatter);
	//-------------------------------------------------------------------------
	virtual void updateBounds();
	//-------------------------------------------------------------------------
	virtual void clearContent();
	//-------------------------------------------------------------------------
	/**
	 * set selection content. without using formater.
	 * @note: clears previous setted elements.
	 */
	virtual void setContent(const ContentContainer &container);
	//-------------------------------------------------------------------------
	/**
	 * @brief add element using formatter.
	 */
	virtual void addElement(sdc::AComponent::Ptr c,
        const sd::Point2D &startPos=NULL_POINT2D);
	//-------------------------------------------------------------------------
	/**
	 * add bunch of elements using formatter.
	 */
	template <class Container>
	void addElements(const Container &c);
	//-------------------------------------------------------------------------
	const ContentContainer & getContent() const { return content; }
	//-------------------------------------------------------------------------
	ContentContainer & getContent() { return content; }
	//-------------------------------------------------------------------------
	sdcu::AComponentUIPtr createComponentUI(sdcu::ALookAndFeelPtr laf) const {
		return laf->getUI<FrxSelection>();
	}
}; // FrxSelection
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class Container>
void FrxSelection::addElements(const Container &c) {
	if (formatter) // the next numEl elements are compound:
		formatter->setCompoundCounter(c.size());
	typename Container::const_iterator it = c.begin();
	for ( ; it!=c.end(); ++it ) {
		addElement(*it);
	}
}
}}} // namespace(s)

#endif /* SAMBAG_FRXSELECTION_H */
