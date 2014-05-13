/*
 * FrxControl.cpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#include <list>
#include <map>
#include <string>
#include <stack>
#include "FrxControl.hpp"
#include "components/FrxCircuidView.hpp"
#include "components/FrxConcreteProcessor.hpp"
#include "components/FrxConcreteParameter.hpp"
#include "components/FrxConcreteIO.hpp"
#include "components/FrxConcreteConnections.hpp"
#include "components/Forward.hpp"
#include <sambag/com/Common.hpp>
#include <loki/MultiMethods.h>
#include <loki/Typelist.h>
#include <loki/LokiTypeinfo.h>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/Label.hpp>
#include <sambag/disco/components/MenuSelectionManager.hpp>
#include <sambag/disco/components/Window.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/com/Thread.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <sambag/com/events/PropertyChanged.hpp>
#include <processing/IModelController.hpp>
#include "IViewModelMap.hpp"
#include <exception>
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/assign.hpp>
#include <boost/unordered_set.hpp>
#include <gui/components/SetupWindow.hpp>
#include <processing/IParameter.hpp>
#include <processing/IProcessor.hpp>
#include <processing/IPluginAdapter.hpp>
#include <processing/IParameterConnection.hpp>
#include <processing/processing.h>
#include <sambag/disco/components/ui/ALookAndFeel.hpp>
#include <sambag/disco/components/DefaultBoundedRangeModel.hpp>
#include "components/SetupCtrl.hpp"
#include <gui/components/FrxProcessorBrowser.hpp>
#include <gui/components/FrxPluginBrowser.hpp>
#include <gui/components/FrxMainBrowser.hpp>
#include "components/FrxProcessorBrowserCtrl.hpp"
#include "components/FrxConnectionBrowser.hpp"
#include "components/FrxConnectionBrowserCtrl.hpp"
#include "components/FrxPluginBrowserCtrl.hpp"
#include "components/FrxMainBrowserCtrl.hpp"
#include "components/FrxPluginEditor.hpp"
#include "components/FrxPluginEditorCtrl.hpp"
#include "components/FrxIO.hpp"
#include <sambag/disco/TimedUpdater.hpp>
#include <gui/components/FrxFlag.hpp>
#include <gui/components/About.hpp>
#include <scripts/PluginScriptCtrl.hpp>
#include <sambag/lua/LuaMap.hpp>
#include <sambag/lua/LuaSequence.hpp>
namespace {
    const long FRX_REFRESH_PARAMETER=30;
}


namespace frx {
namespace processing {
    extern scripts::PluginScriptCtrl::Ptr
    getScriptControl(frx::gui::components::FrxCircuidViewPtr view);
}
namespace gui {
SAMBAG_DERIVATED_EXCEPTION_CLASS(
        sambag::com::exceptions::IllegalStateException, __ControllerMapEx
);
using namespace components;
std::string __lastBrowserSelection;
//------------------------------------------------------------------------------
boost::tuple<
	frx::processing::IModelController::Ptr,
	IViewModelMap::Ptr
>
getControllerAndMap(FrxCircuidViewPtr circ)
{
	// create model obj.
	frx::processing::IModelController::Ptr ctrl = 
		frx::processing::getModelController(circ);
	if (!ctrl) {
		SAMBAG_THROW(__ControllerMapEx, "IModelController == NULL");
	}
	IViewModelMap::Ptr map = getViewModelMap(circ);
	if (!map) {
		SAMBAG_THROW(__ControllerMapEx, "IViewModelMap == NULL");
	}
	return boost::make_tuple(ctrl, map);
}
namespace {
////////////////////////////////////////////////////////////////////////////////
typedef std::map<std::string, sdc::Window::Ptr> ExtraWindows;
ExtraWindows extraWindows;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  Private 
//-----------------------------------------------------------------------------
bool onModelObjectRemoved(fp::ModelObject::WPtr _mObj, FrxCircuidViewWPtr _view);
//-----------------------------------------------------------------------------
void ignoreFrxParameterEvents(FrxParameter::Ptr vp, bool val) {
	vp->putClientProperty("Ctrl.edit", val);
}
bool isIgnored(FrxParameter::Ptr p) {
	bool val = false;
	p->getClientProperty("Ctrl.edit", val);
	return val;
}
//-----------------------------------------------------------------------------
sambag::com::RecursiveMutex parameterMutex;
void knobChanged(void *src, 
	const sdc::DefaultBoundedRangeModelChanged &ev,
	frx::processing::IParameter::WPtr _par,
	FrxParameter::WPtr _knob) 
{
	FrxParameter::Ptr knob = _knob.lock();
	// ignore parameter->knob events
	if (isIgnored(knob)) {
		return;
	}
	frx::processing::IParameter::Ptr par = _par.lock();
	if (!knob || !par)
		return;
	typedef frx::processing::IParameter::Number Number;
	par->setValue((Number)ev.getSrc().getValue());
	
}
//-----------------------------------------------------------------------------
typedef std::pair<frx::processing::IParameter::WPtr, 
		components::FrxParameter::WPtr> ParameterRefreshInfo;
template <class T>
struct RefreshParameter {
	static bool updateImpl(const T &inf) {
		using namespace frx::processing;
		IParameter::Ptr p = inf.first.lock();
		components::FrxParameter::Ptr vp = inf.second.lock(); 
		if (!p || !vp) {
			return true;
		}
        if (vp->getRangeModel()->getValue() == p->getValue()) {
            if (vp->getLowerFlagText() == p->getDisplay()) {
                return true;
            }
        }
		ignoreFrxParameterEvents(vp, true);
		vp->getRangeModel()->setValue(p->getValue());
		vp->setUpperFlagText(p->getName());
		vp->setLowerFlagText(p->getDisplay());
        vp->redraw();
		ignoreFrxParameterEvents(vp, false);
        return true;
	}
    inline bool update(const T &inf) {
        return updateImpl(inf);
    }
};
void parameterChanged(void *src, float value, 
	frx::processing::IParameter::WPtr _par,
	FrxParameter::WPtr _knob)
{
    FrxParameter::Ptr knob = _knob.lock();
	if (!_par.lock() || !knob) {
		return;
	}
    
    if (knob->getWindowThreadId() == sambag::com::getThreadId()) {
        // redraw request came from window thread so we can
        // redraw immediately
        RefreshParameter<ParameterRefreshInfo>::updateImpl(
            std::make_pair(_par, _knob)
        );
        return;
    }
    // redraw request came not from the window thread so in order
    // to avoid threadlocks we need to sync the redrawing
	sd::TimedUpdater<ParameterRefreshInfo,
		RefreshParameter, FRX_REFRESH_PARAMETER>::instance().update(
		std::make_pair(_par, _knob)
	);
}
//-----------------------------------------------------------------------------
bool onModelObjectRemoved(fp::ModelObject::WPtr _mObj,
                          FrxCircuidViewWPtr _view)
{
	fp::ModelObject::Ptr mObj = _mObj.lock();
	FrxCircuidViewPtr view = _view.lock();
	if (!view) {
		return true; // nothing to do anymore
	}
	// get view obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
    try {
        boost::tie(ctrl, map) = getControllerAndMap(view);
    } catch (const __ControllerMapEx &ex) {
        // happens randomly when FrxCircuidView (which is object for signal
        // tracker) dissapears to late, and so an old signal was called
        return true;
    }
	ViewObject::Ptr obj = map->getViewObject(mObj);
	if (!obj) { // nothing to do anymore
		return true;
	}
	
	FrxComponent::Ptr c = boost::dynamic_pointer_cast<FrxComponent>(obj);
	map->remove(c, mObj);
	view->remove(c);
	view->AContainer::redraw();
	return true;
}
//-----------------------------------------------------------------------------
template <class ModelType>
typename ModelType::Ptr getModelObject(FrxCircuidViewPtr view, FrxComponent::Ptr c) 
{
	IViewModelMap::Ptr map = getViewModelMap(view);
	if (!map) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"access to IViewModelMap failed");
	}
	frx::processing::ModelObject::Ptr mObj = map->getModelObject(c);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"related modelobject == NULL");
	}
	typename ModelType::Ptr modelObj = 
		boost::dynamic_pointer_cast<ModelType>(mObj);
	if (!modelObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"accessing to model object failed.");
	}
	return modelObj;
}
//-----------------------------------------------------------------------------
typedef ::processing::ProcessAdapter::SwitchState SwitchState;
typedef boost::tuple<FrxProcessorNode::WPtr, SwitchState, SwitchState> StateData;
std::stack<StateData> states;
FrxIO::Ptr getStateChangedNode(FrxProcessorNode::Ptr pr, const SwitchState &sws) 
{
	try { 
		if (sws.first==true) { // isInput
			return pr->getInputs().at(sws.second);
		} else {
			return pr->getOutputs().at(sws.second);
		}
	} catch(...) {
		return FrxIO::Ptr();
	}
	return FrxIO::Ptr();
}
template <class T>
struct RefreshStates {
	bool update(const T &data) {
		FrxProcessorNode::WPtr _pr;
		SwitchState old, _new;
		boost::tie(_pr, old, _new) = data;
		FrxProcessorNode::Ptr pr = _pr.lock();
		if (!pr) {
			return true;
		}
		FrxIO::Ptr oldNode = getStateChangedNode(pr, old);
		FrxIO::Ptr newNode = getStateChangedNode(pr, _new);
		if (oldNode) {
			oldNode->setState(FrxIO::Activated, false);
		}
		if (newNode) {
			newNode->setState(FrxIO::Activated, true);
		}
        return true;
	}
};
void processorSwitchStateChanged(const StateData &data)
{
	sd::TimedUpdater<StateData, RefreshStates, 5>::instance().update(data);
}
//-----------------------------------------------------------------------------
void processorPropertyChanged(void *src, 
	const sce::PropertyChanged &ev, FrxProcessorNode::WPtr pr)
{
	if (ev.getPropertyName() == "switch state") {
		SwitchState old, _new;
		ev.getOldValue(old);
		ev.getNewValue(_new);
		processorSwitchStateChanged(StateData(pr, old, _new));
	}
	if (ev.getPropertyName() == "status message") {
        FrxProcessorNode::Ptr obj = pr.lock();
        if (!obj) {
            return;
        }
        std::string txt;
        ev.getNewValue(txt);
        SAMBAG_LOG_INFO<<obj->getName()<<" status: "<<txt;
        obj->setLowerFlagText(txt);
	}
}
//-----------------------------------------------------------------------------
void registerOnView(FrxCircuidViewPtr view, FrxProcessorNode::Ptr viewObj) 
{
    
    //std::cout<<std::hex<<"s "<<viewObj->getName()<<"("<<viewObj.get()<<")"<<": "<<view.get()<<std::endl;
    
	frx::processing::IProcessor::Ptr modelObj = 
		getModelObject<frx::processing::IProcessor>(view, viewObj);

	const FrxProcessorNode::IOContainer &ins = viewObj->getInputs();
	int num = std::min(ins.size(), modelObj->getNumInputs());
	for (int i=0; i<num; ++i) {
		frx::processing::INode::Ptr node = modelObj->getInput(i);
		node->addRemoveRequestExecuter(
			boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
		);
	}
	const FrxProcessorNode::IOContainer &outs = viewObj->getOutputs();
	num = std::min(outs.size(), modelObj->getNumOutputs());
	for (int i=0; i<num; ++i) {
		frx::processing::INode::Ptr node = modelObj->getOutput(i);
		node->addRemoveRequestExecuter(
			boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
		);
	}
	modelObj->addPropertyChangedListener(
		boost::bind(&processorPropertyChanged, _1, _2, 
			FrxProcessorNode::WPtr(viewObj)
		)
	);
    std::string status = modelObj->getStatusMessage();
    if (!status.empty()) {
        viewObj->setLowerFlagText(status);
    }
}
//-----------------------------------------------------------------------------
void registerOnView(FrxCircuidViewPtr view, FrxParameter::Ptr knob) {
	frx::processing::IParameter::Ptr par = 
		getModelObject<frx::processing::IParameter>(view, knob);
	
	frx::processing::IParameter::WPtr _par = par;
	FrxParameter::WPtr _knob = knob;

	knob->getRangeModel()->setValue(par->getValue());
	// knob listener
	knob->getRangeModel()->sce::EventSender<sdc::DefaultBoundedRangeModelChanged>::
		addTrackedEventListener ( boost::bind(&knobChanged, _1, _2, _par, _knob), par );
	// parameter listener
	par->getEventSender().addTrackedValueChangedListener(
		boost::bind(&parameterChanged, _1, _2, _par, _knob), knob
	);
    using frx::processing::ModelObject;
    ModelObject::Connection cn = par->addRemoveRequestExecuter(
                                   // make sure that knob will be removed
								   // when related model object does.
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
}
//-----------------------------------------------------------------------------
template <class ConnectionType>
fp::IConnection::Ptr 
connectModelObjects(fp::IModelController::Ptr ctrl, 
	fp::ModelObject::Ptr src,
	fp::ModelObject::Ptr dst) 
{
	return fp::IConnection::Ptr();
}
//-----------------------------------------------------------------------------
template <>
fp::IConnection::Ptr 
inline connectModelObjects<IOCn>(fp::IModelController::Ptr ctrl, 
	fp::ModelObject::Ptr src,
	fp::ModelObject::Ptr dst) 
{
	fp::INode::Ptr nsrc = boost::dynamic_pointer_cast<fp::INode>(src);
	fp::INode::Ptr ndst = boost::dynamic_pointer_cast<fp::INode>(dst);
	SAMBAG_ASSERT(nsrc && ndst);
	return ctrl->connect(nsrc, ndst);
}
//-----------------------------------------------------------------------------
template <>
fp::IConnection::Ptr 
inline connectModelObjects<ParameterCn>(fp::IModelController::Ptr ctrl, 
	fp::ModelObject::Ptr src,
	fp::ModelObject::Ptr dst) 
{
	fp::IParameter::Ptr nsrc = boost::dynamic_pointer_cast<fp::IParameter>(src);
	fp::IParameter::Ptr ndst = boost::dynamic_pointer_cast<fp::IParameter>(dst);
	SAMBAG_ASSERT(nsrc && ndst);
	return ctrl->connect(nsrc, ndst);
}

//-----------------------------------------------------------------------------
template <class ConnectionType>
fgc::FrxConnection::Ptr
perfomConnect(FrxCircuidView::Ptr view,
				   FrxComponent::Ptr src, 
				   FrxComponent::Ptr dst)
{
	// model stuff
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::ModelObject::Ptr msrc = map->getModelObject(src);
	frx::processing::ModelObject::Ptr mdst = map->getModelObject(dst);
	if (!msrc || !mdst) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to connect NULL object(s)."
		);
	}
	frx::processing::IConnection::Ptr mcnt = 
		connectModelObjects<ConnectionType>(ctrl, msrc, mdst);
	if (!mcnt)
		return fgc::FrxConnection::Ptr();
	// view stuff
	typename ConnectionType::Ptr cn = ConnectionType::create();
	cn->setSrcComponent(src);
	cn->setDstComponent(dst);
    map->registerObjects(cn, mcnt);
	view->add(cn, FrxCircuidView::Z_Wires);
	return cn;
}
//-----------------------------------------------------------------------------
template <class Browser>
typename Browser::Ptr openDetailsBrowser(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	typename Browser::Ptr browser;
	browser = Browser::create( view->getLastContainer<sdc::Window>() );
	browser->validate();
	browser->pack();
	browser->positionWindow();
	browser->open();
	return browser;
}
//-----------------------------------------------------------------------------
FrxPluginEditor::Ptr createPluginEditor(fgc::FrxCircuidViewPtr view, 
	fgc::FrxComponentPtr c, frx::processing::IPluginAdapter::Ptr plugin)
{
	FrxPluginEditor::Ptr ed;
    
    // bridged plugins have an own window impl, default is NULL
    sdc::AWindowImplPtr winImpl = plugin->getWindowImpl();
    if (winImpl) {
        ed=FrxPluginEditor::create(winImpl, view->getLastContainer<sdc::Window>());
    } else {
        ed=FrxPluginEditor::create(
            view->getLastContainer<sdc::Window>(),
            !plugin->isInternal()
        );
	}
    ed->setTitle(c->getName() + " editor");
	return ed;
}
//-----------------------------------------------------------------------------
void onComponentRemoving(void *src, const OnRemoving &ev, sdc::WindowWPtr _win) 
{
	sdc::Window::Ptr win = _win.lock();
	if (!win) {
		return;
	}
	win->close();
}
//-----------------------------------------------------------------------------
void onViewEvent(void *src, const FrxCircuidViewEvent &ev, sdc::WindowWPtr _win) 
{
	if (ev.type != FrxCircuidViewEvent::OnClosing) {
		return;
	}
	sdc::Window::Ptr win = _win.lock();
	if (!win) {
		return;
	}
	win->close();
}
//-----------------------------------------------------------------------------
void installBrowserListeners(sdc::WindowWPtr _browser, fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c = fgc::FrxComponentPtr() ) 
{
	if (c) {
		c->sce::EventSender<OnRemoving>::addTrackedEventListener(
			boost::bind(&onComponentRemoving, _1, _2, _browser),
			_browser
		);
	}
	view->sce::EventSender<FrxCircuidViewEvent>::addTrackedEventListener(
		boost::bind(&onViewEvent, _1, _2, _browser),
		_browser
	);
}
//-----------------------------------------------------------------------------
FrxColumnBrowser::Ptr __openMainBrowser(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr alwaysNull)
{
	FrxMainBrowser::Ptr browser;
	browser = FrxMainBrowser::create( view->getLastContainer<sdc::Window>() );
	installBrowserListeners(browser, view);
	getFrxControl(view).addWindow(browser, "FrxControl.extraWindow");
	browser->validate();
	browser->pack();
	FrxMainBrowserCtrl::Ptr ctrl = FrxMainBrowserCtrl::create();
	frx::processing::IModelController::Ptr mCtrl = frx::processing::getModelController(view);
	if (mCtrl) {
		ctrl->setHostInfo(mCtrl->getHostInfo());
	}
	browser->setCtrl(ctrl);
	browser->setTitle("Scene Browser:");
	browser->setWindowSize(sd::Dimension(800,400));
	browser->positionWindow();
	browser->open();
	browser->initTree(view);
	return browser;
}
//-----------------------------------------------------------------------------
void __openSetup(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	SetupWindow::Ptr setup;
	setup = SetupWindow::create( view->getLastContainer<sdc::Window>() );
	getFrxControl(view).addWindow(setup, "FrxControl.extraWindow");
	SetupCtrl::Ptr ctrl = SetupCtrl::create();
	frx::processing::IModelController::Ptr mCtrl = frx::processing::getModelController(view);
	if (mCtrl) {
		ctrl->setHostInfo(mCtrl->getHostInfo());
		ctrl->setView(view);
	}
	setup->setCtrl(ctrl);
	setup->validate();
	setup->positionWindow();
	setup->open();
}
//-----------------------------------------------------------------------------
void __onViewMouse(void *src, const sdce::MouseEvent &ev, About::WPtr _about) {
	if (ev.getType() == sdce::MouseEvent::DISCO_MOUSE_CLICKED) {
		About::Ptr about = _about.lock();
		if (about) {
			about->close();
		}
	}
}
void __openAbout(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	About::Ptr about = About::create( view->getLastContainer<sdc::Window>() );
	getFrxControl(view).addWindow(about, "FrxControl.extraWindow");
	about->validate();
	about->positionWindow();
	about->open();
	view->getContentPane()->sce::EventSender<sdce::MouseEvent>::addTrackedEventListener
	(
		boost::bind(&__onViewMouse, _1, _2, About::WPtr(about)),
		about
	);
}
////////////////////////////////////////////////////////////////////////////////
// Menu Entries
//-----------------------------------------------------------------------------
typedef std::pair<std::string, IFrxControl::CtrlCmd> Entry;
//-----------------------------------------------------------------------------
typedef std::list<Entry> Entries;
//-----------------------------------------------------------------------------
void createMainMenuEntries(FrxCircuidViewPtr view, Entries &out) {

	out.push_back( Entry("Modify Scene...",
		boost::bind(&__openMainBrowser, _1, _2)));
	out.push_back( Entry("Open Setup Dialog...",
		boost::bind(&__openSetup, _1, _2)));
	out.push_back( Entry("About...",
		boost::bind(&__openAbout, _1, _2)));

}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr createPopupMenu(FrxCircuidViewPtr view, 
	const Entries &entries) 
{
	using namespace sambag::disco::components;
	PopupMenuPtr res = PopupMenu::create();
    
	BOOST_FOREACH(const Entry &e, entries) {
		MenuItem::Ptr item = MenuItem::create();
		item->setText(e.first);
		item->EventSender<sdc::events::ActionEvent>::addTrackedEventListener(
			getFrxControl(view).createCtrlCommandFunction(view, fgc::FrxComponentPtr(), e.second),
			view
		);
		res->add(item);
	}
    return res;
}
//=============================================================================
// class Connector
//=============================================================================
struct Connector {
	FrxCircuidView::Ptr view;
	Connector(FrxCircuidView::Ptr view) : view(view) {}
	fgc::FrxConnection::Ptr OnError(FrxNode &a, FrxNode &b) {
        return fgc::FrxConnection::Ptr();
    }
	fgc::FrxConnection::Ptr Fire(FrxNode &a, FrxNode &b) {
        return fgc::FrxConnection::Ptr();
    }
	// consider direction: out->in
	fgc::FrxConnection::Ptr Fire(FrxInputNode &a, FrxOutputNode &b) { 
		return perfomConnect<IOCn>(view, b.getPtr(), a.getPtr());
	}
	fgc::FrxConnection::Ptr Fire(FrxInputNode &a, FrxEntryNode &b) {
		return perfomConnect<IOCn>(view, b.getPtr(), a.getPtr());
	}
	fgc::FrxConnection::Ptr Fire(FrxOutputNode &a, FrxExitNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	fgc::FrxConnection::Ptr Fire(FrxEntryNode &a, FrxExitNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	fgc::FrxConnection::Ptr Fire(FrxStdKnob &a, FrxStdKnob &b) {
		return perfomConnect<ParameterCn>(view, a.getPtr(), b.getPtr());
	}
};
} // namespace
//=============================================================================
//  Class FrxControl
//=============================================================================
namespace {
	FrxConnection::Ptr createConnectionForKnobAnd(FrxComponentPtr c) {
		// (this approach is ok for a few types only)
		if ( dynamic_cast<FrxProcessorNode*>(c.get()) ) {
			return ProcessorParameterCn::create();
		}
		if ( dynamic_cast<FrxConnection*>(c.get()) ) {
			return ParameterOPCn::create();
		}
		return FrxConnection::Ptr();
	}
	template<class TypeList>
	void _registerIfType(fgc::FrxCircuidViewPtr view, FrxComponentPtr c) {	
		typedef typename TypeList::Head T;
		typename T::Ptr ptr = 
			boost::dynamic_pointer_cast<T>(c);
		if (ptr) {
			registerOnView(view, ptr);
		}
		_registerIfType<typename TypeList::Tail>(view, c);
	}
	template<>
	void _registerIfType<Loki::NullType>(fgc::FrxCircuidViewPtr view, FrxComponentPtr c) 
	{	
	}
}
//-----------------------------------------------------------------------------
namespace {
    void __setPath(FrxColumnBrowser::WPtr browser, const std::string &path) {
        FrxColumnBrowser::Ptr b = browser.lock();
        if (!b) {
            return;
        }
        b->getBrowserImpl()->setSelectionPath(path);
    }
    void __onBrowserClose(FrxColumnBrowser::WPtr browser) {
        FrxColumnBrowser::Ptr b = browser.lock();
        if (!b) {
            return;
        }
        __lastBrowserSelection = b->getBrowserImpl()->selectionPathToString();
    }
} // namespace
void FrxControl::openSceneBrowser(fgc::FrxCircuidViewPtr view, const std::string &path)
{
    FrxColumnBrowser::Ptr b = __openMainBrowser(view, fgc::FrxComponentPtr());
    b->addOnCloseEventListener(
        boost::bind(&__onBrowserClose, FrxColumnBrowser::WPtr(b))
    );
    sdc::getWindowToolkit()->invokeLater(
        boost::bind(&__setPath, FrxColumnBrowser::WPtr(b), path),
        100
    );
}
//-----------------------------------------------------------------------------
void FrxControl::openSetup(fgc::FrxCircuidViewPtr view) {
    __openSetup(view, fgc::FrxComponentPtr());
}
//-----------------------------------------------------------------------------
void FrxControl::openAbout(fgc::FrxCircuidViewPtr view) {
    __openAbout(view, fgc::FrxComponentPtr());
}
//-----------------------------------------------------------------------------
void FrxControl::registerComponent(fgc::FrxCircuidViewPtr view, FrxComponentPtr c)
{
	typedef LOKI_TYPELIST_2( FrxParameter,
		FrxProcessorNode
	) Types;
	_registerIfType<Types>(view, c);
}
//-----------------------------------------------------------------------------
fgc::FrxComponentPtr FrxControl::_addRelatedKnobToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, frx::processing::IParameter::Ptr par)
{
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	// create knob
	FrxStdKnob::Ptr knob = FrxStdKnob::create();
	if (!knob) {
		return FrxComponentPtr();
	}
	// flag
	FrxFlag::Ptr flag = FrxFlag::create();
	flag->setTarget(knob);
	view->add(flag, FrxCircuidView::Z_Flags, true);
	knob->setUpperFlagText(par->getName());
	knob->setLowerFlagText(par->getDisplay());
	knob->setName( par->getName() );
	// register knob
	if (!map->registerObjects(knob, par))
		return fgc::FrxComponentPtr();
	registerComponent(view, knob);
	// create connection
	FrxConnection::Ptr cn = createConnectionForKnobAnd(c);
	if (!cn) {
		return fgc::FrxComponentPtr();
	}
	cn->setSrcComponent(c);
	cn->setDstComponent(knob);
	view->add(cn, FrxCircuidView::Z_Wires);
	view->add(knob, FrxCircuidView::Z_Knobs);
	return knob;
}
//-----------------------------------------------------------------------------
fgc::FrxComponentPtr FrxControl::addRelatedKnobToView(FrxCircuidViewPtr view, 
	FrxComponentPtr c, frx::processing::IParameter::Ptr par) 
{
	FrxComponentPtr knob = _addRelatedKnobToView(view, c, par);
	// add hover
	FrxSelection::Ptr sel = view->getSelection();
	sel->addElement(knob);
	sel->setVisible(true);
	return knob;
}
//-----------------------------------------------------------------------------
FrxComponentPtr FrxControl::addProcessorInput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, bool followMouse)
{
	FrxProcessorNode::Ptr proV = 
		boost::dynamic_pointer_cast<FrxProcessorNode>(c);
	if (!proV) {
		return FrxComponentPtr();
	}
	// get model object
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::IProcessor::Ptr proM =
		boost::dynamic_pointer_cast<frx::processing::IProcessor> (
			map->getModelObject(c)
		);
	if (!proM)
		return FrxComponentPtr();
	frx::processing::INode::Ptr io = ctrl->addInputTo(proM);
	if (!io)
		return FrxComponentPtr();
	// create view object
	FrxInputNode::Ptr viewIo = FrxInputNode::create();
	proV->addInputNode(view, viewIo);
	viewIo->setDisplayText(sambag::com::toString(proV->getInputs().size()));
	
	map->registerObjects(viewIo, io);
	io->addRemoveRequestExecuter(
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
    if (followMouse) {
        // add hover
        FrxHover::Ptr sel = FrxHover::create();
        view->add(sel);
        sel->addElement(viewIo);
        sel->setVisible(true);
    }
	return viewIo;
}
//-----------------------------------------------------------------------------
FrxComponentPtr FrxControl::addProcessorOutput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, bool followMouse)
{
	FrxProcessorNode::Ptr proV = 
		boost::dynamic_pointer_cast<FrxProcessorNode>(c);
	if (!proV) {
		return FrxComponentPtr();
	}
	// get model object
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::IProcessor::Ptr proM =
		boost::dynamic_pointer_cast<frx::processing::IProcessor> (
			map->getModelObject(c)
		);
	if (!proM)
		return FrxComponentPtr();
	frx::processing::INode::Ptr io = ctrl->addOutputTo(proM);
	if (!io)
		return FrxComponentPtr();
	// create view object
	FrxOutputNode::Ptr viewIo = FrxOutputNode::create();

	// create connection
	ProcessorOutputCn::Ptr cn = ProcessorOutputCn::create();
	proV->addOutputNode(view, viewIo);
	viewIo->setDisplayText(sambag::com::toString(proV->getOutputs().size()));
	map->registerObjects(viewIo, io);
	io->addRemoveRequestExecuter(
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
    if (followMouse) {
        // add hover
        FrxHover::Ptr sel = FrxHover::create();
        view->add(sel);
        sel->addElement(viewIo);
        sel->setVisible(true);
    }
	return viewIo;
}
//-----------------------------------------------------------------------------
void FrxControl::addParameterToView(fgc::FrxCircuidViewPtr view, 
		fgc::FrxParameterPtr knob)
{

	
	registerComponent(view, knob);
	view->add(knob, FrxCircuidView::Z_Knobs);
	// add hover
	FrxSelection::Ptr sel = view->getSelection();
	sel->addElement(knob);
	sel->setVisible(true);
	
}
//-----------------------------------------------------------------------------
namespace {
typedef std::list<sdc::AComponentPtr> Components;
typedef boost::function<void(fgc::FrxCircuidViewPtr, 
	Components &out, FrxProcessorNodePtr pr)> ExtraF;
typedef std::map<Loki::TypeInfo, ExtraF> ExtraMap;
ExtraMap extraMap;
void addOutParameter(fgc::FrxCircuidViewPtr view,
	Components &out, FrxProcessorNodePtr pr) 
{
	using namespace frx::processing;
	IViewModelMap::Ptr map = getViewModelMap(view);
	frx::processing::ModelObject::Ptr obj = 
		map->getModelObject(pr);
	if (!obj) {
		return;
	}
	ModelObject::Parameters outp;
	obj->getParameters("output parameter", outp);
	BOOST_FOREACH(IParameter::Ptr par, outp) {
		FrxComponentPtr knob = dynamic_cast<FrxControl*>(&getFrxControl(view))->
			_addRelatedKnobToView(view, pr, par);
		sc::Number x = pr->getX() + pr->getWidth()/2. + 15.;
		sc::Number y = pr->getY() - 15.;
		knob->setLocation(x,y);
		out.push_back(knob);
	}
}

void _addParametersImpl(fgc::FrxCircuidViewPtr view,
	Components &out, FrxProcessorNodePtr pr, int num, int *ids)
{
    using namespace frx::processing;
	IViewModelMap::Ptr map = getViewModelMap(view);
	frx::processing::ModelObject::Ptr obj = 
		map->getModelObject(pr);
	if (!obj) {
		return;
	}
    ModelObject::Parameters pars;
	obj->getParameters("*", pars);
	for (int i=0; i<num; ++i) {
        FrxComponentPtr knob = dynamic_cast<FrxControl*>(&getFrxControl(view))->
            _addRelatedKnobToView(view, pr, pars.at( ids[i] ));
        sc::Number x = pr->getX() + pr->getWidth()/2. + 55.;
        sc::Number y = pr->getY() - 15. + i*35;
        knob->setLocation(x,y);
        out.push_back(knob);
    }
}

void addParameters(fgc::FrxCircuidViewPtr view,
	Components &out, FrxProcessorNodePtr pr, int p_id1)
{
    int ids[] = { p_id1 };
    _addParametersImpl(view, out, pr, 1, &ids[0]);
}
void addParameters(fgc::FrxCircuidViewPtr view,
	Components &out, FrxProcessorNodePtr pr, int p_id1, int p_id2)
{
    int ids[] = { p_id1, p_id2 };
    _addParametersImpl(view, out, pr, 2, &ids[0]);
}
void addParameters(fgc::FrxCircuidViewPtr view,
	Components &out, FrxProcessorNodePtr pr, int p_id1, int p_id2, int p_id3)
{
    int ids[] = { p_id1, p_id2, p_id3 };
    _addParametersImpl(view, out, pr, 3, &ids[0]);
}



void _initExtraMap()
{
	using namespace boost::assign;
	extraMap = map_list_of
		(Loki::TypeInfo(typeid(FrxPeakTrackerNode)), &addOutParameter)
		(Loki::TypeInfo(typeid(FrxADSRNode)), &addOutParameter)
	;
    // these inserts does not work with boost::assign
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxVolumeNode)), boost::bind( &addParameters, _1, _2, _3, 0)));
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxPanNode)), boost::bind( &addParameters, _1, _2, _3, 0)));
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxInSwitchNode)), boost::bind( &addParameters, _1, _2, _3, 0)));
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxOutSwitchNode)), boost::bind( &addParameters, _1, _2, _3, 0)));
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxInStepNode)), boost::bind( &addParameters, _1, _2, _3, 0, 1, 6)));
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxOutStepNode)), boost::bind( &addParameters, _1, _2, _3, 0, 1, 6)));
    extraMap.insert(std::make_pair( Loki::TypeInfo(typeid(FrxMIDIReceiver)), boost::bind( &addParameters, _1, _2, _3, 0, 2, 8)));
}
    
/**
 * returns type related objects which should added to view
 * besides the main object.
 */
void addExtraContent(fgc::FrxCircuidViewPtr view, 
	std::list<sdc::AComponentPtr> &out, 
	FrxProcessorNodePtr pr)
{
	if (extraMap.empty()) {
		_initExtraMap();
	}
	ExtraMap::const_iterator it = extraMap.find(
		typeid(*(pr.get()))
	);
	if (it==extraMap.end()) {
		return;
	}
    try {
        it->second(view, out, pr);
    } catch (const std::exception &ex) {
        SAMBAG_LOG_ERR<<"FrxControl::addExtraContent failed: "<<ex.what();
    } catch(...) {
        SAMBAG_LOG_ERR<<"FrxControl::addExtraContent failed.";
    }
}
} //namespace(s)
void FrxControl::addProcessorToView(fgc::FrxCircuidViewPtr view, 
		FrxProcessorNodePtr pr)
{
    sdc::AComponent::Ptr root = view->getTopLevelRootPane();
    if (!root) {
        return;
    }
    //std::cout<<std::hex<<"a "<<pr->getName()<<"("<<pr.get()<<")"<<": "<<view.get()<<std::endl;
    SAMBAG_BEGIN_SYNCHRONIZED( root->getTreeLock() )
        // add to view
        view->add(pr, FrxCircuidView::Z_ProcessorNodes);
        pr->resetIOLocation();
        // register
        registerComponent(view, pr);
        // hover
        FrxSelection::Ptr sel = view->getSelection();
        sel->setVisible(true);
        Components toAdd;
        toAdd.push_back(pr);
        addExtraContent(view, toAdd, pr);
        toAdd.insert(toAdd.end(), pr->getInputs().begin(), pr->getInputs().end());
        toAdd.insert(toAdd.end(), pr->getOutputs().begin(), pr->getOutputs().end());
        sel->addElements(toAdd);
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
FrxControl::FrxControl() {
}
//-----------------------------------------------------------------------------
FrxControl::~FrxControl() {
}
namespace {
	// return true if removable
	bool _isRemovable( FrxComponentPtr c) {
		if (dynamic_cast<FrxIO*>(c.get())) {
			return false;
		}
		return true;
	}
} // namespace(s)
//-----------------------------------------------------------------------------
void FrxControl::removeComponent(FrxCircuidViewPtr _view, FrxComponentPtr _c)
{
    FrxCircuidViewPtr view(_view);
	FrxComponentPtr c(_c);
	if (!_isRemovable(c)) {
		return;
	}
	if (!c || !view)
		return;
	
    sdc::AComponent::Ptr root = view->getTopLevelRootPane();
    if (!root) {
        return;
    }
    //std::cout<<std::hex<<"r "<<c->getName()<<"("<<c.get()<<")"<<": "<<view.get()<<std::endl;
    SAMBAG_BEGIN_SYNCHRONIZED( root->getTreeLock() )
    
        // get model obj.
        frx::processing::IModelController::Ptr ctrl;
        IViewModelMap::Ptr map;
        boost::tie(ctrl, map) = getControllerAndMap(view);
        frx::processing::ModelObject::Ptr mObj = map->getModelObject(c);
        try {
            if (mObj) {
                mObj->remove(ctrl);
        }
            map->remove(c, mObj);
            view->remove(c);
            view->AContainer::redraw();
        } catch(const std::exception &ex) {
            std::stringstream ss;
            ss<<"removing of " << c->getName() << " failed: " << ex.what();
            view->errorMessage(ss.str());
        } catch (...) {
            std::stringstream ss;
            ss<<"removing of " << c->getName() << " failed: unkown error";
            view->errorMessage(ss.str());
        }
    
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxControl::getCircuidViewPopup(FrxCircuidViewPtr c) {
	Entries mainMenuEntries;
	createMainMenuEntries(c, mainMenuEntries);
	return createPopupMenu(c, mainMenuEntries);
}
//-----------------------------------------------------------------------------
fgc::FrxConnection::Ptr
FrxControl::connect(FrxCircuidViewPtr view, FrxNodePtr from, FrxNodePtr to)
{
	typedef LOKI_TYPELIST_5(
		FrxInputNode,
		FrxOutputNode, 
		FrxEntryNode, 
		FrxExitNode,
		FrxStdKnob
	) Types;
	typedef Loki::StaticDispatcher  <
		Connector,
        FrxNode, 
        Types,
		true,
        FrxNode,
        Types,
        fgc::FrxConnection::Ptr
    > Dispatcher;
	try {
		return Dispatcher::Go(*(from.get()), *(to.get()), Connector(view));
	} catch (...) {
		return fgc::FrxConnection::Ptr();
	}
}
//-----------------------------------------------------------------------------
void FrxControl::handleContextMenuPopup(const sdc::events::MouseEvent &ev) {
	using namespace sambag::disco::components;
	if (ev.getButtons() != sdc::events::MouseEvent::DISCO_BTN2) {
		if (currPopup)
			currPopup->hidePopup();
		return;
	}
    FrxCircuidView::Ptr circ = ev.getSource()->getFirstContainer<FrxCircuidView>();
    if (!circ) {
        return;
    }
    
	sdc::PopupMenuPtr popup = circ->getContextMenu(ev.getSource());
	
    if (popup) {
        // set context object to view
        FrxComponentPtr obj = ev.getSource()->getFirstContainer<FrxComponent>();
        if (obj && circ) {
            circ->putClientProperty("popupcontext", FrxComponentWPtr(obj));
        }
    } else { // no popup use mainview popup
        if (circ && !circ->getComponentPopupMenu()) { // lazy init of main menu
            popup = getCircuidViewPopup(circ);
            circ->setComponentPopupMenu( popup );
        } else {
            return;
        }
    }
	if (!popup->isPopupVisible()) {
		MenuSelectionManager &m = MenuSelectionManager::defaultManager();
		m.clearSelectedPath();
		IMenuElement::MenuElements p;
		p.push_back(popup);
		m.setSelectedPath(p);
		popup->setInvoker(ev.getSource());
		popup->showPopup(
			ev.getLocationOnScreen()
		);
	}
	currPopup = popup;
}
//-----------------------------------------------------------------------------
boost::tuple<fgc::FrxNodePtr, fgc::FrxNodePtr>
FrxControl::createEntryExtitNodes(fgc::FrxCircuidViewPtr circ)
{
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(circ);
	boost::tuple<fgc::FrxNodePtr, fgc::FrxNodePtr> res;

	// entry node
	boost::get<0>(res) = FrxEntryNode::create();
	circ->add(boost::get<0>(res), FrxCircuidView::Z_IO);
	map->registerObjects(boost::get<0>(res), ctrl->getEntry());
	
	//exit node
	boost::get<1>(res) = FrxExitNode::create();
	circ->add(boost::get<1>(res), FrxCircuidView::Z_IO);
	map->registerObjects(boost::get<1>(res), ctrl->getExit());

	return res;

}
//-----------------------------------------------------------------------------
void FrxControl::executeCtrlCommand(void *src,
		const sdc::events::ActionEvent &ev,
		fgc::FrxCircuidViewWPtr v, 
		fgc::FrxComponentWPtr c, 
		CtrlCmd cmd)
{
	FrxCircuidViewPtr view = v.lock();
	FrxComponentPtr comp = c.lock(); // can be null
	SAMBAG_ASSERT(view);
	try {
        cmd(view,comp);
	} catch(const std::exception &ex) {
		view->errorMessage("operation failed: " + std::string(ex.what()));
	} catch (...) {
		view->errorMessage("operation failed: unkown error.");
	}
}
//-----------------------------------------------------------------------------
sambag::com::events::EventSender<sdc::events::ActionEvent>::EventFunction
FrxControl::createCtrlCommandFunction(fgc::FrxCircuidViewPtr view,
	fgc::FrxComponentPtr comp,
	const IFrxControl::CtrlCmd &cmdF)
{
	FrxCircuidViewWPtr wView = view;
	FrxComponentWPtr wC = comp;
	return boost::bind(&FrxControl::executeCtrlCommand, 
		this, 
		_1, 
		_2, 
		wView, 
		wC,
		cmdF
	);
}
//-----------------------------------------------------------------------------
void FrxControl::showProcessorDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	// create browser
	FrxProcessorBrowser::Ptr browser = 
		openDetailsBrowser<FrxProcessorBrowser>(view, c);
	installBrowserListeners(browser, view, c);
	addWindow(browser);
	browser->setTitle(c->getName() + " details");
	FrxProcessorBrowserCtrl::Ptr ctrl = FrxProcessorBrowserCtrl::create();
	ctrl->setComponent(c);
	browser->setCtrl(ctrl);
	browser->initTree(view);
}
//-----------------------------------------------------------------------------
void FrxControl::showConnectionDetails(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	// create browser
	FrxConnectionBrowser::Ptr browser = 
		openDetailsBrowser<FrxConnectionBrowser>(view, c);
	addWindow(browser);
	installBrowserListeners(browser, view, c);
	browser->setTitle(c->getName() + " details");
	FrxConnectionBrowserCtrl::Ptr ctrl = FrxConnectionBrowserCtrl::create();
	ctrl->setComponent(c);
	browser->setCtrl(ctrl);
	browser->initTree(view);
}
//-----------------------------------------------------------------------------
void FrxControl::openClosePluginEditor(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	typedef frx::processing::IPluginAdapter Plugin;
	Plugin::Ptr plugin = boost::dynamic_pointer_cast<Plugin>(
		map->getModelObject(c)
	);
	if (!plugin)
		return;
    if (!plugin->hasEditor()) {
        return;
    }
    // check editor is open
	FrxPluginEditor::Ptr ed;
	FrxPluginEditor::WPtr wed;
	c->getClientProperty("plugin.editor", wed);
	ed = wed.lock();
	if (ed) { // and close
		c->putClientProperty("plugin.editor", FrxPluginEditor::WPtr());
		ed->close();
		return;
	}
	// else: create editor
    ed = createPluginEditor(view, c, plugin);
	addWindow(ed);
	c->putClientProperty("plugin.editor", FrxPluginEditor::WPtr(ed));
	installBrowserListeners(ed, view, c);
	FrxPluginEditorCtrl::Ptr pluginCtrl = FrxPluginEditorCtrl::create();
	pluginCtrl->setPlugin(plugin);
	ed->setControl(pluginCtrl);
	ed->positionWindow();
	ed->validate();
	ed->pack();
    ed->open();
}
//-----------------------------------------------------------------------------
void FrxControl::addParamterCnOp(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c, const ParameterCnOpTypeId &id)
{
	frx::processing::IModelController::Ptr mctrl;
	IViewModelMap::Ptr map;
	boost::tie(mctrl, map) = getControllerAndMap(view);
	using frx::processing::IConnection;
	IConnection::Ptr cn =
		boost::dynamic_pointer_cast<IConnection> (map->getModelObject(c));
	if (!cn)
		return;
	mctrl->addParameterCnOp(cn, id);
	view->sce::EventSender<FrxCircuidViewEvent>::notifyListeners(
		view.get(),
		FrxCircuidViewEvent(FrxCircuidViewEvent::ComponentUpdated, c)
	);
}
//-----------------------------------------------------------------------------
void FrxControl::
getParameterCnOpTypeIds(fgc::FrxCircuidViewPtr view, ParameterCnOpTypeIds &out) const 
{
	frx::processing::IModelController::Ptr mCtrl =
		frx::processing::getModelController(view);
	if (!mCtrl) {
		return;
	}
	mCtrl->getParameterCnOpTypeIds(out);
}
//-----------------------------------------------------------------------------
void FrxControl::getOperators(fgc::FrxCircuidViewPtr view,
    fgc::FrxConnection::Ptr con, std::vector<Operator> &out)
{
    frx::processing::IModelController::Ptr mctrl;
	IViewModelMap::Ptr map;
	boost::tie(mctrl, map) = getControllerAndMap(view);
    using frx::processing::IParameterConnection;
    IParameterConnection::Ptr mObj =
		boost::dynamic_pointer_cast<IParameterConnection> (map->getModelObject(con));
    if (!mObj) {
        return;
    }
    for (size_t i=0; i<mObj->getNumConnectionOps(); ++i) {
        out.push_back(Operator(i, mObj->getConnectionOpName(i)));
    }
}
//-----------------------------------------------------------------------------
void FrxControl::removeOperator(fgc::FrxCircuidViewPtr view,
    fgc::FrxConnection::Ptr con, OperatorId id)
{
    frx::processing::IModelController::Ptr mctrl;
	IViewModelMap::Ptr map;
	boost::tie(mctrl, map) = getControllerAndMap(view);
    using frx::processing::IParameterConnection;
    IParameterConnection::Ptr mObj =
		boost::dynamic_pointer_cast<IParameterConnection> (map->getModelObject(con));
    if (!mObj) {
        return;
    }
    mObj->removeConnectionOp(id);
}
//-----------------------------------------------------------------------------
void FrxControl::addWindow(sdc::WindowPtr win, const std::string &wndClass) {
	std::string key=wndClass;
	if (key=="") {
		key = sambag::com::toString((long)win.get());
	}
	//extraWindows[key] = win;
}
//=============================================================================
//-----------------------------------------------------------------------------
IFrxControl & getFrxControl(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxControl> FactoryHolder;
	return FactoryHolder::Instance();
}
}} // namespace(s)
