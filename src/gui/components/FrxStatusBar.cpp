/*
 * FrxStatusBar.cpp
 *
 *  Created on: Thu May 30 10:54:18 2013
 *      Author: Johannes Unger
 */

#include "FrxStatusBar.hpp"
#include <sambag/disco/IResourceManager.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>
#include <sambag/disco/svg/graphicElements/Style.hpp>
#include <sambag/disco/components/SolidBorder.hpp>
#include <sambag/disco/svg/StyleParser.hpp>
#include <sambag/disco/components/FlowLayout.hpp>

namespace frx { namespace gui { namespace components {
namespace {
//-----------------------------------------------------------------------------
class StatusLabel : public sdc::Label {
public:
	typedef boost::shared_ptr<StatusLabel> Ptr;
	typedef sdc::Label Super;
protected:
	StatusLabel(){
		setOpaque(false);
	}
public:
	SAMBAG_STD_STATIC_COMPONENT_CREATOR(StatusLabel)
	virtual sd::Dimension getPreferredSize() {
		sd::Dimension sz = Super::getMinimumSize();
		sz.height( sz.height() + 5. );
		return sz; 
	}
};
} // namespace(s)
//=============================================================================
//  Class FrxStatusBar
//=============================================================================
//-----------------------------------------------------------------------------
void FrxStatusBar::postConstructor() {
	initStatusBar();
}
//-----------------------------------------------------------------------------
void FrxStatusBar::setStatusMessage(const std::string &txt, 
	const std::string &iconname) 
{
	statusMessage->setText(txt);
	if (iconname == "") {
		statusMessage->setIcon(sd::ISurface::Ptr());
		return;
	}
	sd::ISurface::Ptr icon = 
		sd::getResourceManager().getImage("StatusMessage.icon." + iconname);
	statusMessage->setIcon(icon);

	sdc::AComponentPtr stc = statusMessage->getParent();
	if (!stc)
		return;
	stc->revalidate();
	stc->redraw();
}
//-----------------------------------------------------------------------------
void FrxStatusBar::initStatusBar() {
	using sd::svg::graphicElements::Style;
	sdc::Panel::Ptr panel = getPtr();
	// border
	sdc::SolidBorder::Ptr border = sdc::SolidBorder::create();
	Style statusStyle = 
		createStyle("stroke-width: 1; stroke: black;font-size: 13; font-family: arial");
	sdc::ui::getUIManager().getProperty("StatusMessage.style", statusStyle);
	border->setStyle(statusStyle);
	panel->setBorder(border);
	// layout
	panel->setLayout(sdc::FlowLayout::create(sdc::FlowLayout::LEFT, 0, 0));
	statusMessage = StatusLabel::create();
	statusMessage->setFont( statusStyle.font());
	panel->add(statusMessage);
	setStatusMessage("Ready", "hint");
}
//-----------------------------------------------------------------------------
sdc::LabelPtr FrxStatusBar::getStatusLabel() const {
	return statusMessage;
}
}}} // namespace(s)
