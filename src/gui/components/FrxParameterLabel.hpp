/*
 * FrxParameterLabel.hpp
 *
 *  Created on: Thu Nov 15 08:53:01 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXPARAMETERLABEL_H
#define SAMBAG_FRXPARAMETERLABEL_H

#include <memory>
#include <sambag/disco/components/Label.hpp>
#include <sambag/disco/components/DefaultBoundedRangeModel.hpp>
#include <gui/HandyNamespaces.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class FrxParameterLabel.
  * Label (especially for listentry purpose) with the ability to adjust a value.
  */
class FrxParameterLabel : 
	public sdc::Label,
	public sdc::DefaultBoundedRangeModel
{
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef sdc::Label Super;
	//-------------------------------------------------------------------------
	typedef std::shared_ptr<FrxParameterLabel> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxParameterLabel> WPtr;
	//-------------------------------------------------------------------------
	virtual sdc::ui::AComponentUIPtr 
	createComponentUI(sdc::ui::ALookAndFeelPtr laf) const;
protected:
	//-------------------------------------------------------------------------
	FrxParameterLabel();
	//-------------------------------------------------------------------------
	virtual void postConstructor();
private:
public:
	//-------------------------------------------------------------------------
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(FrxParameterLabel)
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 */
//	void validate() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 *
	 * @since 1.5
	 */
//	void invalidate() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 *
	 * @since 1.5
	 */
	//void redraw() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 */
//	void revalidate() {}
	//-------------------------------------------------------------------------
	/**
	 * Overridden for performance reasons.
	 * See the <a href="#override">Implementation Note</a>
	 * for more information.
	 */
//	void redraw(const sd::Rectangle &r) {}
}; // FrxParameterLabel
}}} // namespace(s)

#endif /* SAMBAG_FRXPARAMETERLABEL_H */
