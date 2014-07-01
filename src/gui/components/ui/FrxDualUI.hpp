/*
 * FrxDualUI.hpp
 *
 *  Created on: Mon Aug 27 10:35:54 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FrxDualUI_H
#define SAMBAG_FrxDualUI_H

#include <boost/shared_ptr.hpp>
#include <sambag/disco/components/ui/AComponentUI.hpp>
#include <gui/components/FrxCircuidView.hpp>

namespace frx { namespace gui {
namespace components { namespace ui { 
///////////////////////////////////////////////////////////////////////////////
//=============================================================================
/** 
  * @class FrxDualUI Base.
  */
struct AFrxDualUI : public sdcu::AComponentUI {
    typedef boost::shared_ptr<AFrxDualUI> Ptr;
    virtual sdcu::AComponentUI::Ptr getUI() const = 0;
    virtual ~AFrxDualUI(){}
};
//=============================================================================
//=============================================================================
/** 
  * @class FrxDualUI.
  * @brief tries to install UI A and if fails, fallback to B.
  */
template <class _UIA, class _UIB>
class FrxDualUI : public AFrxDualUI {
//=============================================================================
public:
    //-------------------------------------------------------------------------
	typedef _UIA UIA;
    //-------------------------------------------------------------------------
	typedef _UIB UIB;
    //-------------------------------------------------------------------------
	typedef FrxDualUI<UIA, UIB> ThisClass;
	//-------------------------------------------------------------------------
	typedef sdcu::AComponentUI Super;
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<ThisClass> Ptr;
protected:
    sdcu::AComponentUIPtr ui;
    bool usingA;
    //-------------------------------------------------------------------------
    void installationFailed(sdc::AComponentPtr c);
public:
    //-------------------------------------------------------------------------
    sdcu::AComponentUI::Ptr getUI() const {
        return ui;
    }
	//-------------------------------------------------------------------------
	static Ptr create();
    //-------------------------------------------------------------------------
    virtual bool contains(sdc::AComponentPtr c, const sd::Point2D &p)
    {
        return ui->contains(c, p);
    }
    virtual int getBaseline(sdc::AComponentPtr c,
        const sd::Coordinate &width, const sd::Coordinate &height)
    {
        return ui->getBaseline(c, width, height);
    }
    virtual sd::Dimension getMaximumSize(sdc::AComponentPtr c)
    {
        return ui->getMaximumSize(c);
    }
    virtual sd::Dimension getMinimumSize(sdc::AComponentPtr c)
    {
        return ui->getMinimumSize(c);
    }
    virtual sd::Dimension getPreferredSize(sdc::AComponentPtr c)
    {
        return ui->getPreferredSize(c);
    }
    virtual void draw(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c)
    {
        ui->draw(cn, c);
    }
    virtual void installUI(sdc::AComponentPtr c);
    virtual void uninstallUI(sdc::AComponentPtr c)
    {
        ui->uninstallUI(c);
    }
    virtual void update(sd::IDrawContext::Ptr cn, sdc::AComponentPtr c)
    {
        ui->update(cn, c);
    }
}; // FrxDualUI
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
template <class UIA, class UIB>
void FrxDualUI<UIA, UIB>::installationFailed(sdc::AComponentPtr c) {
    if (usingA) {
        // installing A failed:
        ui->uninstallUI(c);
        ui = UIB::create();
        usingA = false;
        // try again
        installUI(c);
    } else {
        throw std::runtime_error("DualUI: installing of fallback UI failed");
    }
}
//-----------------------------------------------------------------------------
template <class UIA, class UIB>
typename FrxDualUI<UIA, UIB>::Ptr FrxDualUI<UIA, UIB>::create() {
    Ptr res = Ptr(new ThisClass());
    try {
        res->ui = UIA::create();
        res->usingA = true;
    } catch(...) {
        res->ui = UIB::create();
        res->usingA = false;
    }
    return res;
}
//-----------------------------------------------------------------------------
template <class UIA, class UIB>
void FrxDualUI<UIA, UIB>::installUI(sdc::AComponentPtr c) {
    try {
        ui->installUI(c);
    } catch(const std::exception &ex) {
        SAMBAG_LOG_ERR<<"installing of ui failed: "<<ex.what();
        installationFailed(c);
    } catch(...) {
        installationFailed(c);
    }
}
}}}} // namespace(s)

#endif /* SAMBAG_FrxDualUI_H */
