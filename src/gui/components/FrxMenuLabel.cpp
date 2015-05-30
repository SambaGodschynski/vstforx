/*
 * FrxMenuLabel.cpp
 *
 *  Created on: Thu Apr  3 11:37:15 2014
 *      Author: Johannes Unger
 */

#include "FrxMenuLabel.hpp"
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/Style.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class FrxMenuLabel
//=============================================================================
//-----------------------------------------------------------------------------
FrxMenuLabel::FrxMenuLabel() {
	setName("FrxMenuLabel");
}
//-----------------------------------------------------------------------------
void FrxMenuLabel::postConstructor() {
    setOpaque(false);
    sds::Style menuLabelStyle = sds::Style::DEFAULT_STYLE;
	sdcu::getUIManager().getProperty("FrxComponent.menu.label.style", menuLabelStyle);
    setForeground( menuLabelStyle.strokePattern() );
	setBackground( menuLabelStyle.fillPattern() );
	setFont( menuLabelStyle.font() );
}
//-----------------------------------------------------------------------------
sd::Dimension FrxMenuLabel::getPreferredSize() {
    sd::Dimension sz = Super::getMinimumSize();
    sz.height( sz.height() + 10. );
    return sz; 
}
}}} // namespace(s)
