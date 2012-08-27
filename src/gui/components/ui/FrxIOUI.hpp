/*
 * FrxIOUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXIOUI_H
#define SAMBAG_FRXIOUI_H

#include <boost/shared_ptr.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include "FrxNodeUI.hpp"

namespace frx { namespace gui {
namespace components { namespace ui { 
namespace sd = sambag::disco;
namespace sdc = sd::components;
namespace sdcu = sdc::ui;
//=============================================================================
/** 
  * @class FrxIOUI.
  */
template <class _ConcreteIO>
class FrxIOUI : public FrxNodeUI {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef FrxNodeUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxIOUI> Ptr;
	//-------------------------------------------------------------------------
	typedef _ConcreteIO ConcreteIO;
protected:
	//-------------------------------------------------------------------------
	typedef FrxIOUI<ConcreteIO> ThisClassType;
	//-------------------------------------------------------------------------
	FrxIOUI(){}
private:
public:
	//-------------------------------------------------------------------------
	virtual void installUI(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res = Ptr(new ThisClassType());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c);
}; // FrxIOUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
namespace {
	template <class IOType>
	void drawIO(sd::IDrawContext::Ptr cn, FrxIO::Ptr io) {
		cn->translate(io->getPivot());
		cn->arc(sd::Point2D(0, 0), io->getWidth() / 2.5);
		cn->setFillColor(io->getForeground());
		cn->fill();
	}
	template <class IOType>
	void setIOSize(sdc::AComponentPtr c) {
		c->setSize(sd::Dimension(50, 50));
	}
	template <>
	void setIOSize<ioTypes::Input>(sdc::AComponentPtr c) {
		c->setSize(sd::Dimension(25, 25));
	}
	template <>
	void setIOSize<ioTypes::Output>(sdc::AComponentPtr c) {
		c->setSize(sd::Dimension(25, 25));
	}
} // namespace
//-----------------------------------------------------------------------------
template <class CIO>
void FrxIOUI<CIO>::draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c) {
	//Super::draw(cn, c);
	FrxIO::Ptr io = boost::shared_dynamic_cast<FrxIO>(c);
	drawIO<CIO>(cn, io);
}
//-----------------------------------------------------------------------------
template <class CIO>
void FrxIOUI<CIO>::installUI(sdc::AComponentPtr c) {
	setIOSize<CIO>(c);
	Super::installUI(c);
}
}}}} // namespace(s)

#endif /* SAMBAG_FRXIOUI_H */
