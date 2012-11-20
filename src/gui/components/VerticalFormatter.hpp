/*
 * VerticalFormatter.hpp
 *
 *  Created on: Tue Nov  6 21:47:07 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VERTICALFORMATTER_H
#define SAMBAG_VERTICALFORMATTER_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/Geometry.hpp>
#include <sambag/com/ArithmeticWrapper.hpp>
#include "IFormatter.hpp"

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class VerticalFormatter.
  */
class VerticalFormatter : public IFormatter {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<VerticalFormatter> Ptr;
	//-------------------------------------------------------------------------
	typedef IFormatter Super;
protected:
	//-------------------------------------------------------------------------
	VerticalFormatter(){}
	//-------------------------------------------------------------------------
	sd::Point2D lastPos;
	//-------------------------------------------------------------------------
	sd::Coordinate gap;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<int> numCompound;
	//-------------------------------------------------------------------------
	std::vector<sdc::AComponentPtr> tmp;
	//-------------------------------------------------------------------------
	void translateComponent(sdc::AComponentPtr c);
	//-------------------------------------------------------------------------
	void translateCompound();
public:
	//-------------------------------------------------------------------------
	/**
	 * handles the next num elements as one object (with a common bounding box)
	 */
	virtual void setCompoundCounter(size_t num);
	//-------------------------------------------------------------------------
	virtual size_t getCompoundCounter() const {
		return numCompound;
	}
	//-------------------------------------------------------------------------
	virtual ~VerticalFormatter() {
		
	}
	//-------------------------------------------------------------------------
	void resetOrigin(const sd::Point2D &p = sd::Point2D(0,0));
	//-------------------------------------------------------------------------
	virtual void reset() {
		resetOrigin();
	}
	//-------------------------------------------------------------------------
	virtual sd::Point2D getCursor() const {
		return lastPos;
	}
	//-------------------------------------------------------------------------
	virtual void setCursor(const sd::Point2D &p);
	//-------------------------------------------------------------------------
	const sd::Coordinate & getGap() const {
		return gap;
	}
	//-------------------------------------------------------------------------
	void setGap(const sd::Coordinate &val);
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new VerticalFormatter());
	}
	//-------------------------------------------------------------------------
	virtual void addElement(sdc::AComponentPtr c);
}; // VerticalFormatter
}}} // namespace(s)

#endif /* SAMBAG_VERTICALFORMATTER_H */
