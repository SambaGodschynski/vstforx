/*
 * FrxMainBrowserCtrl.cpp
 *
 *  Created on: Wed Oct 31 10:48:34 2012
 *      Author: Johannes Unger
 */

#include "FrxMainBrowserCtrl.hpp"
#include "FrxMainBrowser.hpp"
#include <gui/FrxControl.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <string>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxParameterLabel.hpp>
#include <gui/components/FrxParameter.hpp>
#include <gui/components/ui/FrxBrowserListUI.hpp>
#include <boost/assign.hpp>
#include "FrxConcreteProcessor.hpp"

namespace frx { namespace gui { namespace components {
namespace {
//-----------------------------------------------------------------------------
// boost::tuples::tuple<std::string, FolderID>
typedef ::com::PluginCollection::Folder DBFolderType;
typedef ::com::PluginCollection::FolderID DBFolderID;
typedef FrxMainBrowserCtrl::Tree Tree;
typedef FrxMainBrowserCtrl::Tree::Node TreeNode;
//-----------------------------------------------------------------------------
void onSelectionPathChanged(void *src,
	const sdc::SelectionPathChanged &ev,
	FrxColumnBrowser::WPtr _browser)
{
	// lock weak ptr
	FrxColumnBrowser::Ptr browser = _browser.lock();
	SAMBAG_ASSERT(browser);
	// get selection path
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();
	const Tree::Path &path = tree->getSelectionPath();
	if (path.empty())
		return;
	const BrowserNode &bNode = tree->getNodeData(path.back());
	if (!bNode.isFolder())
		return;
	bNode.accept();
}
} // namespace(s)
//=============================================================================
//  Class FrxMainBrowserCtrl
//=============================================================================
namespace {
	template<class T>
	Loki::TypeInfo _type() {
		return Loki::TypeInfo(typeid(T));
	}
}
void FrxMainBrowserCtrl::initAdderMap() {
	adderMap = boost::assign::map_list_of
	(_type<FrxPluginNode>(), boost::bind(&FrxMainBrowserCtrl::addPluginToSceneTree, this, _1))
	(_type<FrxVolumeNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxPanNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxInStepNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxOutStepNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxInSwitchNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxOutSwitchNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxADSRNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1))
	(_type<FrxPeakTrackerNode>(), boost::bind(&FrxMainBrowserCtrl::addProcessorToSceneTree, this, _1));
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::onBrowserOk(void *src,
	const sdc::events::ActionEvent &ev)
{
	// lock weak ptr
	FrxCircuidViewPtr view = this->wView.lock();
	FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (!view || !browser) {
		return;
	}
	// get selection path
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();
	const Tree::Path &path = tree->getSelectionPath();
	if (path.empty())
		return;
	const BrowserNode &bNode = tree->getNodeData(path.back());
	try {
		BrowserNode::ResultPtr res = bNode.accept();
		FrxComponentPtr c;
		sambag::com::get(res, c);
		if (c) {
			// update scene tree
			addToSceneTree(c);
		}
	} catch(const std::exception &ex) {
	} catch (...) {
		// TODO
	}
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr 
FrxMainBrowserCtrl::fillPluginFolder(TreeNode parent, DBFolderID dbFolderId)
{

	FrxColumnBrowserPtr brws = this->browser.lock();
	FrxCircuidViewPtr view = wView.lock();
	SAMBAG_ASSERT(brws && view);
	FrxMainBrowserCtrl::Ptr ctrl = 
		boost::shared_dynamic_cast<FrxMainBrowserCtrl>(brws->getCtrl());
	SAMBAG_ASSERT(ctrl);
	Tree::Ptr tree = brws->getBrowserImpl();
	if (tree->getNumChildren(parent) > 0)
		return BrowserNode::ResultPtr();

	::com::PluginCollection &db = ::com::getPluginCollection();
	::com::PluginCollection::Folders folders;
	db.getSubFolders(dbFolderId, folders);
	// fill subfolder
	BOOST_FOREACH(const DBFolderType &dbF, folders) { 
		TreeNode treeFolder = 
			tree->addNode(parent);

		const std::string &name = boost::get<0>(dbF);
		DBFolderID dbID =  boost::get<1>(dbF);
		BrowserNode node(name, true);
		node.f = 
			boost::bind(&FrxMainBrowserCtrl::fillPluginFolder, 
			this,						// browser	
			treeFolder,				    // parent (browser) folder node
			dbID						// parent db-folderid
		);
		tree->setNodeData(treeFolder, node);
	}
	// fill add_plugins
	::com::PluginCollection::PluginInfoList plugs;
	db.getPlugInfoList(dbFolderId, plugs);
	BOOST_FOREACH(const ::processing::PluginInfo &pI, plugs) { 
		TreeNode plug = 
			tree->addNode(parent);

		const std::string &name = pI.name;
		BrowserNode node;
		ctrl->createPluginNode(node, name);
		node.f = 
			boost::bind(&FrxMainBrowserCtrl::addPlugin, 
			this,
			pI			 // plugin info
		);
		node.type = BrowserConstants::FRX_BROWSER_PLUGIN;
		tree->setNodeData(plug, node);
	}

	tree->updateLists();
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::addPlugin(::processing::PluginInfo pI) 
{
	FrxCircuidViewPtr view = wView.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add plugin with FrxCircuidViewPtr == NULL");
	}
	IFrxComponentFactory &fac = getComponentFactory(view);
	FrxProcessorNodePtr pr = fac.getPluginCreator()(view, pI);
	if (!pr)
		return BrowserNode::ResultPtr();
	getFrxControl(view).addProcessorToView(view, pr);
	return BrowserNode::ResultPtr(
		sambag::com::createObject(FrxComponentPtr(pr))
	);
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
addProcessor(IFrxComponentFactory::ProcessorCreator f) 
{
	FrxCircuidViewPtr view = wView.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with FrxCircuidViewPtr == NULL");
	}
	FrxProcessorNodePtr pr = f(view);
	if (!pr)
		return BrowserNode::ResultPtr();
	getFrxControl(view).addProcessorToView(view, pr);
	return BrowserNode::ResultPtr(
		sambag::com::createObject(FrxComponentPtr(pr))
	);
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
addFreeKnob(IFrxComponentFactory::FreeParameterCreator f) 
{
	FrxCircuidViewPtr view = wView.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add knob with FrxCircuidViewPtr == NULL");
	}
	FrxParameterPtr pr = f(view);
	if (!pr)
		return BrowserNode::ResultPtr();
	getFrxControl(view).addParameterToView(view, pr);
	return BrowserNode::ResultPtr(
		sambag::com::createObject(FrxComponentPtr(pr))
	);
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
addHostKnob( IFrxComponentFactory::HostParameterCreator f, int id) 
{
	FrxCircuidViewPtr view = wView.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add knob with FrxCircuidViewPtr == NULL");
	}
	FrxParameterPtr pr = f(view, id);
	if (!pr)
		return BrowserNode::ResultPtr();
	getFrxControl(view).addParameterToView(view, pr);
	return BrowserNode::ResultPtr(
		sambag::com::createObject(FrxComponentPtr(pr))
	);
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
addProcesorKnobToView(FrxComponentWPtr _c, processing::IParameter::WPtr _par)
{
	fgc::FrxCircuidViewPtr view = wView.lock();
	fgc::FrxComponentPtr c = _c.lock();
	frx::processing::IParameter::Ptr par = _par.lock();
	if (!view || !c || !par)
		return BrowserNode::ResultPtr();
	getFrxControl(view).addProcesorKnobToView(view, c, par);
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initListeners(FrxColumnBrowserPtr brws) {
	FrxMainBrowser::Ptr browser = 
		boost::shared_dynamic_cast<FrxMainBrowser>(brws);
	if (!browser)
		return;
	// add btnOk listener
	browser->getBtnAdd()->EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&FrxMainBrowserCtrl::onBrowserOk, this, _1, _2)
	);
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::setHostInfo(IHostInfo::Ptr hostInfo) {
	this->hostInfo = hostInfo;
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addMainProcessors() 
{
	FrxColumnBrowserPtr brws = this->browser.lock();
	FrxCircuidViewPtr view = wView.lock();

	Tree::Ptr tree = brws->getBrowserImpl();
	IFrxComponentFactory &fac = getComponentFactory(view);
	std::list<std::string> processorNames;
	fac.getProcessorNames(processorNames);
	BOOST_FOREACH(const std::string &name, processorNames) {
		IFrxComponentFactory::ProcessorCreator f = fac.getProcessorCreator(name);
		BrowserNode node;
		createProcessorNode(node, name);
		node.f = boost::bind(&FrxMainBrowserCtrl::addProcessor, 
			this, f);
		node.type = BrowserConstants::FRX_BROWSER_PROCESSOR;
		tree->addNode(add_processors, node);
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::parameterChanged(void *src, 
		float value, const BrowserNode &node)
{
	FrxColumnBrowserPtr brws = browser.lock();
	if (!brws)
		return;
	// TODO: update specific list or better specific entry
	brws->getBrowserImpl()->redraw();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::parameterLabelChanged(float value, 
	processing::IParameter::WPtr _p)
{
	FrxColumnBrowserPtr brws = browser.lock();
	processing::IParameter::Ptr p = _p.lock();
	if (!brws || !p)
		return;
	p->setValue(value);
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::parameterLabelRedraw( sdc::AComponentPtr c,
	processing::IParameter::WPtr _p,
	const BrowserNode &node)
{
	processing::IParameter::Ptr p = _p.lock();
	FrxParameterLabel::Ptr label = 
		boost::shared_dynamic_cast<FrxParameterLabel>(c);
	if (!label)
		return;
	label->setText(label->getText() + ":" + p->getDisplay());
	label->setValue(p->getValue());
}
//-----------------------------------------------------------------------------
void 
FrxMainBrowserCtrl::createParameterNode(BrowserNode &out,
	const std::string &name,
	processing::IParameter::Ptr obj)
{
	out.name = name;
	out.type = BrowserConstants::FRX_BROWSER_PARAMETER;
	if (!obj)
		return;
	processing::IParameter::WPtr wObj = obj;
	// install parameter listener:
	obj->getEventSender().addTrackedValueChangedListener(
		boost::bind(&FrxMainBrowserCtrl::parameterChanged, this,
		_1, _2, boost::cref(out)),
		browser
	);
	out.drawCallback = 
		boost::bind(&FrxMainBrowserCtrl::parameterLabelRedraw, this, _1, 
		wObj, boost::cref(out));
	out.valueChanged =
		boost::bind(&FrxMainBrowserCtrl::parameterLabelChanged, this, _1, wObj);
}
//-----------------------------------------------------------------------------
void 
FrxMainBrowserCtrl::createProcessorNode(BrowserNode &out, 
	const std::string &name,
	processing::IProcessor::Ptr obj)
{
	out.name = name;
	out.type = BrowserConstants::FRX_BROWSER_PROCESSOR;
}
//-----------------------------------------------------------------------------
void 
FrxMainBrowserCtrl::createPluginNode(BrowserNode &out, 
	const std::string &name,
	processing::IProcessor::Ptr obj)
{
	out.name = name;
	out.type = BrowserConstants::FRX_BROWSER_PLUGIN;
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addMainKnobs() 
{
	FrxColumnBrowserPtr brws = this->browser.lock();
	FrxCircuidViewPtr view = wView.lock();

	Tree::Ptr tree = brws->getBrowserImpl();
	IFrxComponentFactory &fac = getComponentFactory(view);
	BrowserNode node;
	createParameterNode(node, "free knob");
	IFrxComponentFactory::FreeParameterCreator f = fac.getFreeParameterCreator();
	node.type = BrowserConstants::FRX_BROWSER_PARAMETER;
	node.f = boost::bind(&FrxMainBrowserCtrl::addFreeKnob, 
		this, f);
	tree->addNode(add_knobs, node);
	// host add_knobs:
	frx::processing::IModelController::Ptr ctrl
		= frx::processing::getModelController(view);
	Tree::Node hostKnobs = tree->addNode(add_knobs, BrowserNode ("host add_knobs", true));
	int nbKnobs = ctrl->getNumHostParameter();
	IFrxComponentFactory::HostParameterCreator hPcreator = fac.getHostParameterCreator();
	for (int i=0; i<nbKnobs; ++i) {
		BrowserNode node;
		processing::IParameter::Ptr par = 
			ctrl->getHostParameter(i);
		createParameterNode(node, "host knob: " + sambag::com::toString(i), par);
		node.f = boost::bind(&FrxMainBrowserCtrl::addHostKnob, 
			this, hPcreator, i);
		tree->addNode(hostKnobs, node);
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initRoot(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws)
{
	Tree::Ptr tree = brws->getBrowserImpl();
	// add
	add = tree->addNode(tree->getRootNode(), BrowserNode("add to scene", true));
	// add_plugins
	add_plugins = 
		tree->addNode(add);
	FrxCircuidViewWPtr wView = view;
	BrowserNode node("plugins", true);
	node.f = 
		boost::bind(&FrxMainBrowserCtrl::fillPluginFolder, 
		this,							    // browser	
		add_plugins,							// parent (browser) folder node
		::com::PluginCollection::ROOT_FOLDER_ID // parent db-folderid
	);
	tree->setNodeData(add_plugins, node);

	// add_processors
	add_processors = 
		tree->addNode(add, BrowserNode("processors", true));
	addMainProcessors();
	
	// add_knobs
	add_knobs = 
		tree->addNode(add, BrowserNode("knobs", true));
	addMainKnobs();

	// scene tree
	scene = 
		tree->addNode(tree->getRootNode());
	node = BrowserNode("scene tree", true);
	node.f = 
		boost::bind(&FrxMainBrowserCtrl::createSceneTree, this, scene);
	tree->setNodeData(scene, node);
	
	scene_plugins = tree->addNode(scene, BrowserNode("plugins", true));
	scene_processors = tree->addNode(scene, BrowserNode("processors", true));
	scene_parameter = tree->addNode(scene, BrowserNode("parameter", true));
	scene_connections = tree->addNode(scene, BrowserNode("connections", true));

}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::
onRemovingFromScene(void*, const OnRemoving &ev, Tree::Node node) 
{
	FrxColumnBrowserPtr brws = browser.lock();
	if (!brws)
		return;
	Tree::Ptr tree = brws->getBrowserImpl();
	tree->removeNode(node);
	tree->updateLists();
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::
addPluginToSceneTree(FrxComponentPtr c)
{
	
	FrxCircuidViewPtr view = wView.lock();

	FrxProcessorNode::Ptr pr = 
		boost::shared_dynamic_cast<FrxProcessorNode>(c);
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!pr) {
		return Tree::NULL_NODE;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node plugin = tree->addNode(scene_plugins);

	BrowserNode browserNode(c->getName(), true);
	browserNode.f = // dynamic parameter insert
		boost::bind(&FrxMainBrowserCtrl::_addProcessorParameterNodes,
		this,
		FrxComponentWPtr(c),
		plugin
	);
	tree->setNodeData(plugin, browserNode);
	return plugin;
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::addProcessorToSceneTree(FrxComponentPtr c) {
	
	FrxCircuidViewPtr view = wView.lock();
	FrxProcessorNode::Ptr pr = 
		boost::shared_dynamic_cast<FrxProcessorNode>(c);
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!pr) {
		return Tree::NULL_NODE;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node processor = tree->addNode(scene_processors);

	BrowserNode browserNode(c->getName(), true);
	addProcessorParameterNodes(pr, processor);
	tree->setNodeData(processor, browserNode);
	return processor;
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::addParameterToSceneTree(FrxComponentPtr c) {
	return Tree::NULL_NODE;
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::addConnectionToSceneTree(FrxComponentPtr c) {
	return Tree::NULL_NODE;
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addToSceneTree(FrxComponentPtr c) {
	if (!sceneTreeInit) 
		return;
	if (adderMap.empty()) {
		initAdderMap();
	}
	AdderMap::const_iterator it = adderMap.find(
		Loki::TypeInfo(typeid(*(c.get())))
	);
	if (it==adderMap.end())
		return;
	Tree::Node res = it->second(c);
	if (res==Tree::NULL_NODE)
		return;
	c->FrxComponent::EventSender<OnRemoving>::addTrackedEventListener(
		boost::bind(&FrxMainBrowserCtrl::onRemovingFromScene, this, _1, _2, res),
		self
	);
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::createSceneTree(const Tree::Node &parent)
{
	if (sceneTreeInit) {
		return BrowserNode::ResultPtr();
	}
	
	sceneTreeInit = true;

	FrxCircuidViewPtr view = wView.lock();
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!view || !browser)
		return BrowserNode::ResultPtr();
	Tree::Ptr tree = browser->getBrowserImpl();

	std::list<sdc::AComponentPtr> components;
	view->findAllComponents( components,
		FrxCircuidView::Z_Knobs,
		FrxCircuidView::Z_ProcessorNodes
	);
	// collect components
	BOOST_FOREACH(sdc::AComponentPtr c, components) {
		FrxComponentPtr fc = 
			boost::shared_dynamic_cast<FrxComponent>(c);
		if (!fc) {
			continue;
		}
		addToSceneTree(fc);
	}
	tree->updateLists();
	BrowserNode::ResultPtr();
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initTree(FrxCircuidViewPtr view, 
	FrxColumnBrowserPtr brws)
{
	browser = brws;
	wView = view; 
	// create browser tree
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = brws->getBrowserImpl();
	initRoot(view, brws);
	
	/*BrowserNode::AcceptedFunction f = 
		boost::bind(&IFrxControl::addProcesorKnobToView, 
			&frxctrl,
			fgc::FrxCircuidViewWPtr(view), 
			fgc::FrxComponentWPtr(c),
			frx::processing::IParameter::WPtr(p)
	);
	tree->addNode(
		parameter, 
		BrowserNode(p->getName(), f)
	);*/
	// extra from installListeners() because not all browser need this:
	tree->EventSender<sdc::SelectionPathChanged>::addTrackedEventListener(
		boost::bind(&onSelectionPathChanged, _1, _2, FrxColumnBrowser::WPtr(brws)),
		self.lock()
	);
	tree->updateLists();
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
_addProcessorParameterNodes(FrxComponentWPtr c, Tree::Node parent)
{
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser.lock()->getBrowserImpl();
	if (tree->getNumChildren(parent) > 0)
		return BrowserNode::ResultPtr();
	
	addProcessorParameterNodes(c.lock(), parent);
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::
addProcessorParameterNodes(FrxComponentPtr c,
		const Tree::Node &parent)
{
	FrxCircuidViewPtr view = wView.lock();
	if (!view || ! c)
		return;
	// get ctrl, map
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	FrxColumnBrowserPtr browser = this->browser.lock();
	SAMBAG_ASSERT(ctrl && map && browser);
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser->getBrowserImpl();

	frx::processing::IProcessor::Ptr pr = 
		boost::shared_dynamic_cast<frx::processing::IProcessor>(map->getModelObject(c));
	if (!pr)
		return;

	// get frxctrl
	IFrxControl & frxctrl = getFrxControl(view);
	// create browser nodes
	for (size_t i=0; i<pr->getNumParameter(); ++i) {
		frx::processing::IParameter::Ptr p = pr->getParameter(i);
		BrowserNode::AcceptedFunction f = 
			boost::bind(&FrxMainBrowserCtrl::addProcesorKnobToView,
				this,
				fgc::FrxComponentWPtr(c),
				frx::processing::IParameter::WPtr(p)
			);
		BrowserNode node;
		createParameterNode(node, p->getName(), p);
		node.f = f;
		tree->addNode(
			parent, 
			node
		);
	}
	tree->updateLists();
}
}}} // namespace(s)
