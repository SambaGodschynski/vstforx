/*
 * FrxControl.cpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#include "FrxControl.hpp"
#include "components/FrxCircuidView.hpp"
#include "components/FrxConcreteProcessor.hpp"
#include "components/FrxConcreteParameter.hpp"
#include "components/FrxConcreteIO.hpp"
#include "components/FrxConcreteConnections.hpp"
#include "components/Forward.hpp"
#include <boost/foreach.hpp>
#include <sambag/com/Common.hpp>
#include <loki/MultiMethods.h>
#include <loki/TypeList.h>
#include <sambag/disco/components/PopupMenu.hpp>
#include <sambag/disco/components/MenuSelectionManager.hpp>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <processing/IModelController.hpp>
#include "IViewModelMap.hpp"
#include <exception>
#include "__ModelExecutors.hpp"
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>
#include "components/FrxSerializationRegister.hpp"
#include <list>
#include <map>
#include <string>
#include <gui/components/SetupWindow.hpp>
#include <processing/IParameter.hpp>
#include <processing/IProcessor.hpp>
#include <processing/IPluginAdapter.hpp>
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

namespace frx { namespace gui {
using namespace components;
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
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"IModelController == NULL");
	}
	IViewModelMap::Ptr map = getViewModelMap(circ);
	if (!map) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"IViewModelMap == NULL");
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
void knobChanged(void *src, 
	const sdc::DefaultBoundedRangeModelChanged &ev,
	frx::processing::IParameter::WPtr _par) 
{
	frx::processing::IParameter::Ptr par = _par.lock();
	if (!par)
		return;
	par->setValue(ev.getSrc().getValue());
}
//-----------------------------------------------------------------------------
void parameterChanged(void *src, float value, FrxParameter::WPtr _knob) {
	FrxParameter::Ptr knob = _knob.lock();
	if (!knob)
		return;
	knob->getRangeModel()->setValue(value);
}
//-----------------------------------------------------------------------------
bool onModelObjectRemoved(fp::ModelObject::WPtr _mObj, FrxCircuidViewWPtr _view)
{
	fp::ModelObject::Ptr mObj = _mObj.lock();
	FrxCircuidViewPtr view(_view);
	if (!view)
		return true; // nothing to do anymore
	// get model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	ViewObject::Ptr obj = map->getViewObject(mObj);
	if (!obj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"access to view object failed while removing model obj."
		);
	}
	FrxComponent::Ptr c = boost::shared_dynamic_cast<FrxComponent>(obj);
	map->remove(c, mObj);
	view->remove(c);
	view->AContainer::redraw();
	return true;
}
namespace {
	template <class ModelType>
	typename ModelType::Ptr getModelObject(FrxCircuidViewPtr view, FrxComponent::Ptr c) 
	{
		IViewModelMap::Ptr map = getViewModelMap(view);
		if (!map) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
				"tried to add processor with IViewModelMap == NULL");
		}
		typename ModelType::Ptr modelObj = 
			boost::shared_dynamic_cast<ModelType>(
				map->getModelObject(c)
		);
		if (!modelObj) {
			SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
				"tried register view object with related model == NULL");
		}
		return modelObj;
	}
} // namespace
//-----------------------------------------------------------------------------
void registerOnView(FrxCircuidViewPtr view, FrxProcessorNode::Ptr viewObj) 
{

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
}
//-----------------------------------------------------------------------------
void registerOnView(FrxCircuidViewPtr view, FrxInputNode::Ptr obj) {
	frx::processing::INode::Ptr io = 
		getModelObject<frx::processing::INode>(view, obj);
	// removing request
	io->addRemoveRequestExecuter(
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
}
//-----------------------------------------------------------------------------
void registerOnView(FrxCircuidViewPtr view, FrxOutputNode::Ptr obj) {
	frx::processing::INode::Ptr io = 
		getModelObject<frx::processing::INode>(view, obj);
	// removing request
	io->addRemoveRequestExecuter(
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
}
//-----------------------------------------------------------------------------
void registerOnView(FrxCircuidViewPtr view, FrxParameter::Ptr knob) {
	frx::processing::IParameter::Ptr par = 
		getModelObject<frx::processing::IParameter>(view, knob);

	knob->getRangeModel()->setValue(par->getValue());
	// knob listener
	knob->getRangeModel()->EventSender<sdc::DefaultBoundedRangeModelChanged>::
		addTrackedEventListener ( boost::bind(&knobChanged, _1, _2, par), par );
	// parameter listener
	par->getEventSender().addTrackedValueChangedListener(
		boost::bind(&parameterChanged, _1, _2, FrxParameter::WPtr(knob)), knob
	);
	par->addRemoveRequestExecuter( // make sure that knob will be removed
								   // when related model object does.
		boost::bind(&onModelObjectRemoved, _1, FrxCircuidViewWPtr(view))
	);
}
//-----------------------------------------------------------------------------
template <class ConnectionType>
bool perfomConnect(FrxCircuidView::Ptr view, 
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
		return false;
	// view stuff
	typename ConnectionType::Ptr cn = ConnectionType::create();
	cn->setSrcComponent(src);
	cn->setDstComponent(dst);
	view->add(cn, FrxCircuidView::Z_Wires);
	map->registerObjects(cn, mcnt);
	return true;
}
//-----------------------------------------------------------------------------
template <class Browser>
typename Browser::Ptr openBrowser(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	typename Browser::Ptr browser;
	browser = Browser::create();
	browser->validate();
	browser->pack();
	browser->open();
	return browser;
}
//-----------------------------------------------------------------------------
FrxPluginEditor::Ptr createPluginEditor(fgc::FrxCircuidViewPtr view, 
	fgc::FrxComponentPtr c)
{
	FrxPluginEditor::Ptr ed;
	ed = FrxPluginEditor::create();
	ed->setTitle(c->getName() + " editor");
	return ed;
}
//-----------------------------------------------------------------------------
FrxColumnBrowser::Ptr openMainBrowser(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	FrxMainBrowser::Ptr browser;
	browser = FrxMainBrowser::create();
	getFrxControl(view).addWindow(browser, "FrxControl.extraWindow");
	browser->validate();
	browser->pack();
	FrxMainBrowserCtrl::Ptr ctrl = FrxMainBrowserCtrl::create();
	frx::processing::IModelController::Ptr mCtrl = frx::processing::getModelController(view);
	if (mCtrl) {
		ctrl->setHostInfo(mCtrl->getHostInfo());
	}
	browser->setCtrl(ctrl);
	browser->setTitle("Main-Browser:");
	browser->open();
	browser->initTree(view);
	return browser;
}
//-----------------------------------------------------------------------------
void openSetup(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	SetupWindow::Ptr setup;
	setup = SetupWindow::create();
	getFrxControl(view).addWindow(setup, "FrxControl.extraWindow");
	SetupCtrl::Ptr ctrl = SetupCtrl::create();
	frx::processing::IModelController::Ptr mCtrl = frx::processing::getModelController(view);
	if (mCtrl) {
		ctrl->setHostInfo(mCtrl->getHostInfo());
		ctrl->setView(view);
	}
	setup->setCtrl(ctrl);
	setup->validate();
	setup->setTitle("VSTForx Setup");
	setup->open();
}
////////////////////////////////////////////////////////////////////////////////
// Menu Entries
//-----------------------------------------------------------------------------
typedef std::pair<std::string, IFrxControl::CtrlCmd> Entry;
//-----------------------------------------------------------------------------
typedef std::list<Entry> Entries;
//-----------------------------------------------------------------------------
void createMainMenuEntries(Entries &out) {
	out.push_back( Entry("open main browser...",
		boost::bind(&openMainBrowser, _1, _2)));
	out.push_back( Entry("open setup dialog...",
		boost::bind(&openSetup, _1, _2)));
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
	bool OnError(FrxNode &a, FrxNode &b) {return false;}
	bool Fire(FrxNode &a, FrxNode &b) {return false;}
	// consider direction: out->in
	bool Fire(FrxInputNode &a, FrxOutputNode &b) { 
		return perfomConnect<IOCn>(view, b.getPtr(), a.getPtr());
	}
	bool Fire(FrxInputNode &a, FrxEntryNode &b) {
		return perfomConnect<IOCn>(view, b.getPtr(), a.getPtr());
	}
	bool Fire(FrxOutputNode &a, FrxExitNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	bool Fire(FrxEntryNode &a, FrxExitNode &b) {
		return perfomConnect<IOCn>(view, a.getPtr(), b.getPtr());
	}
	bool Fire(FrxStdKnob &a, FrxStdKnob &b) {
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
	void _castSwitch(fgc::FrxCircuidViewPtr view, FrxComponentPtr c) {	
		typedef TypeList::Head T;
		typename T::Ptr ptr = 
			boost::shared_dynamic_cast<T>(c);
		if (ptr) {
			registerOnView(view, ptr);
		}
		_castSwitch<TypeList::Tail>(view, c);
	}
	template<>
	void _castSwitch<Loki::NullType>(fgc::FrxCircuidViewPtr view, FrxComponentPtr c) 
	{	
	}
}
//-----------------------------------------------------------------------------
void FrxControl::registerComponent(fgc::FrxCircuidViewPtr view, FrxComponentPtr c)
{
	typedef LOKI_TYPELIST_4(FrxInputNode, 
		FrxOutputNode, 
		FrxParameter,
		FrxProcessorNode) Types;
	_castSwitch<Types>(view, c);
}
//-----------------------------------------------------------------------------
fgc::FrxComponentPtr FrxControl::addRelatedKnobToView(FrxCircuidViewPtr view, 
	FrxComponentPtr c, frx::processing::IParameter::Ptr par) 
{
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	// create knob
	FrxStdKnob::Ptr knob = FrxStdKnob::create();
	if (!knob) {
		return FrxComponentPtr();
	}
	// register knob
	map->registerObjects(knob, par);
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
	// add hover
	FrxSelection::Ptr sel = view->getSelection();
	sel->addElement(knob);
	sel->setVisible(true);
	return knob;
}
//-----------------------------------------------------------------------------
FrxComponentPtr FrxControl::addProcessorInput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	// get model object
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::IProcessor::Ptr proM =
		boost::shared_dynamic_cast<frx::processing::IProcessor> (
			map->getModelObject(c)
		);
	if (!proM)
		return FrxComponentPtr();
	frx::processing::INode::Ptr io = ctrl->addInputTo(proM);
	if (!io)
		return FrxComponentPtr();
	// create view object
	FrxInputNode::Ptr viewIo = FrxInputNode::create();
	// create connection
	ProcessorInputCn::Ptr cn = ProcessorInputCn::create();
	cn->setSrcComponent(c);
	cn->setDstComponent(viewIo);
	view->add(viewIo, FrxCircuidView::Z_IO, true);
	view->add(cn, FrxCircuidView::Z_Wires);
	map->registerObjects(viewIo, io);
	registerComponent(view, viewIo);
	// add hover
	FrxHover::Ptr sel = FrxHover::create();
	view->add(sel);
	sel->addElement(viewIo);
	sel->setVisible(true);
	return viewIo;
}
//-----------------------------------------------------------------------------
FrxComponentPtr FrxControl::addProcessorOutput(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	// get model object
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::IProcessor::Ptr proM =
		boost::shared_dynamic_cast<frx::processing::IProcessor> (
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
	cn->setSrcComponent(c);
	cn->setDstComponent(viewIo);
	view->add(viewIo, FrxCircuidView::Z_IO, true);
	view->add(cn, FrxCircuidView::Z_Wires);
	map->registerObjects(viewIo, io);
	registerComponent(view, viewIo);
	// add hover
	FrxHover::Ptr sel = FrxHover::create();
	view->add(sel);
	sel->addElement(viewIo);
	sel->setVisible(true);
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
void FrxControl::addProcessorToView(fgc::FrxCircuidViewPtr view, 
		FrxProcessorNodePtr pr)
{
	// add to view
	view->add(pr, FrxCircuidView::Z_ProcessorNodes);
	pr->resetIOLocation();
	// register
	registerComponent(view, pr);
	// hover
	FrxSelection::Ptr sel = view->getSelection();
	sel->setVisible(true);
	std::list<sdc::AComponentPtr> toAdd;
	toAdd.push_back(pr);
	toAdd.insert(toAdd.end(), pr->getInputs().begin(), pr->getInputs().end());
	toAdd.insert(toAdd.end(), pr->getOutputs().begin(), pr->getOutputs().end());
	sel->addElements(toAdd);
}
//-----------------------------------------------------------------------------
FrxControl::FrxControl() {
}
//-----------------------------------------------------------------------------
FrxControl::~FrxControl() {
}
//-----------------------------------------------------------------------------
void FrxControl::removeComponent(FrxCircuidViewPtr _view, FrxComponentPtr _c)
{
	FrxCircuidViewPtr view(_view);
	FrxComponentPtr c(_c);
	if (!c || !view)
		return;
	// get model obj.
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::ModelObject::Ptr mObj = map->getModelObject(c);
	if (!mObj) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"access to model object failed while removing."
		);
	}

	if (!mObj->requestRemove(mObj)) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"removing model object failed."
		);
	}
	map->remove(c, mObj);
	view->remove(c);
	view->AContainer::redraw();
}
//-----------------------------------------------------------------------------
sdc::PopupMenuPtr FrxControl::getCircuidViewPopup(FrxCircuidViewPtr c) {
	Entries mainMenuEntries;
	createMainMenuEntries(mainMenuEntries);
	return createPopupMenu(c, mainMenuEntries);
}
//-----------------------------------------------------------------------------
bool FrxControl::connect(FrxCircuidViewPtr view, FrxNodePtr from, FrxNodePtr to) 
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
        bool
    > Dispatcher;
	Dispatcher disp;
	try {
		return disp.Go(*(from.get()), *(to.get()), Connector(view));
	} catch (...) {
		return false;
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
	sdc::PopupMenuPtr popup = ev.getSource()->getComponentPopupMenu();
	if (!popup)
		return;
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
		openBrowser<FrxProcessorBrowser>(view, c);
	
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
		openBrowser<FrxConnectionBrowser>(view, c);
	addWindow(browser);
	browser->setTitle(c->getName() + " details");
	FrxConnectionBrowserCtrl::Ptr ctrl = FrxConnectionBrowserCtrl::create();
	ctrl->setComponent(c);
	browser->setCtrl(ctrl);
	browser->initTree(view);
}
//-----------------------------------------------------------------------------
void FrxControl::openPluginEditor(fgc::FrxCircuidViewPtr view, 
		fgc::FrxComponentPtr c)
{
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	typedef frx::processing::IPluginAdapter Plugin;
	Plugin::Ptr plugin = boost::shared_dynamic_cast<Plugin>(
		map->getModelObject(c)
	);
	if (!plugin)
		return;
	// create editor
	FrxPluginEditor::Ptr ed = createPluginEditor(view, c);
	addWindow(ed);
	FrxPluginEditorCtrl::Ptr pluginCtrl = FrxPluginEditorCtrl::create();
	pluginCtrl->setPlugin(plugin);
	ed->setControl(pluginCtrl);
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
		boost::shared_dynamic_cast<IConnection> (map->getModelObject(c));
	if (!cn)
		return;
	mctrl->addParameterCnOp(cn, id);
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
namespace {
	void onWindowClose(void *src, const sdc::OnCloseEvent &ev, std::string key)
	{
		//extraWindows.erase(key);
	}
} // namespace(s)
void FrxControl::addWindow(sdc::WindowPtr win, const std::string &wndClass) {
	std::string key=wndClass;
	if (key=="") {
		key = sambag::com::toString((long)win.get());
	}
	extraWindows[key] = win;
	win->addOnCloseEventListener(
		boost::bind(&onWindowClose, _1, _2, key)
	);
}
//=============================================================================
//-----------------------------------------------------------------------------
IFrxControl & getFrxControl(FrxCircuidViewPtr view) {
	typedef Loki::SingletonHolder<FrxControl> FactoryHolder;
	return FactoryHolder::Instance();
}
}} // namespace(s)
