
#include "VstForxEditor.hpp"
#include <sambag/disco/components/WindowToolkit.hpp>
#include <sambag/com/ArbitraryType.hpp>
#include <sambag/disco/Geometry.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <gui/components/FrxConcreteIO.hpp>
#include <gui/components/ui/FrxLookAndFeel.hpp>
#include <sambag/disco/components/ui/UIManager.hpp>

extern void* hInstance;
namespace frx { namespace gui { namespace components {
//=============================================================================
//	Klasse VstForxEditor:
//=============================================================================
//-----------------------------------------------------------------------------
VstForxEditor::VstForxEditor (AudioEffect *aEff) : 
AEffEditor(aEff),
plug(NULL)
{
	size.left = 0;
	size.top = 0;
	size.right = 800;
	size.bottom = 600;
}
//-----------------------------------------------------------------------------
VstForxEditor::~VstForxEditor () {
}
//-----------------------------------------------------------------------------
void VstForxEditor::setPlugin(frx::processing::VstForxPlug *plug) {
	this->plug = plug;
}
//-----------------------------------------------------------------------------
FrxCircuidViewPtr VstForxEditor::createEmptyView(sdc::Window::Ptr win) {
	FrxCircuidView::Ptr circ = FrxCircuidView::create();
	win->getContentPane()->add(circ);

	const sd::Dimension &winSize = win->getContentPane()->getSize();
	
	// entry node
	FrxComponent::Ptr comp = FrxEntryNode::create();
	circ->add(comp, FrxCircuidView::Z_IO);
	sd::Coordinate xLoc = winSize.width()/2. - comp->getWidth()/2.;
	sd::Coordinate yLoc = 0.;
	comp->setLocation(xLoc, yLoc);
	
	//exit node
	comp = FrxExitNode::create();
	circ->add(comp, FrxCircuidView::Z_IO);
	xLoc = winSize.width()/2. - comp->getWidth()/2.;
	yLoc = winSize.height() - comp->getHeight();
	comp->setLocation(xLoc, yLoc);
	return circ;
}
//-----------------------------------------------------------------------------
sdc::Window::Ptr VstForxEditor::createMainWindow(const sd::Rectangle &bounds) {
	using namespace sambag::com;
	using namespace sambag::disco::components;
	ArbitraryType::Ptr pData = createObject(
		std::make_pair(systemWindow, hInstance)
	);
	sdc::Window::Ptr win = 
		getWindowToolkit()->createNestedWindow(pData, bounds);
	SAMBAG_ASSERT(win);
	sdc::ui::UIManager::instance().installLookAndFeel(win->getRootPane(),
		ui::FrxLookAndFeel::create()
	);
	return win;
}
//-----------------------------------------------------------------------------
bool VstForxEditor::open( void *ptr ) {
	using namespace sambag::com;
	using namespace sambag::disco;
	AEffEditor::open(ptr);
	sambag::disco::Rectangle bounds(Point2D(size.left, size.top), 
		Point2D(size.right, size.bottom)
	);
	window = createMainWindow(bounds);
	circView = createEmptyView(window);
	window->validate();
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::close() {
	AEffEditor::close();
}
//-----------------------------------------------------------------------------
bool VstForxEditor::getRect (ERect** rect) {
	*rect = &size;
	return true;
}
//-----------------------------------------------------------------------------
void VstForxEditor::idle() {
/*	if (!window)
		return;
	window->invalidateWindow();*/
}
}}} // namespace(s)



