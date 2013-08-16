/*
 * FrxMainBrowserCtrl.cpp
 *
 *  Created on: Wed Oct 31 10:48:34 2012
 *      Author: Johannes Unger
 */

#include "FrxMainBrowserCtrl.hpp"
#include "FrxMainBrowser.hpp"
#include <gui/IFrxControl.hpp>
#include <processing/IModelController.hpp>
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
#include "FrxConcreteParameter.hpp"
#include "FrxConcreteConnections.hpp"
#include <gui/components/ShellPluginSelection.hpp>
#include "FrxCircuidView.hpp"
#include <gui/TimedUpdater.hpp>

namespace frx { namespace gui { namespace components {
namespace {
const std::string ADDED_TO_SCENE = " added to scene.";
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

	FrxMainBrowser::Ptr mbrowser = 
		boost::dynamic_pointer_cast<FrxMainBrowser>(browser);

	if (!mbrowser) {
		return;
	}
	if (!bNode.instantPerform) {
		mbrowser->getBtnAdd()->setEnabled(true);
	} else {
		mbrowser->getBtnAdd()->setEnabled(false);	
	}
	if (bNode.actionText!="") {
		mbrowser->getBtnAdd()->setText(bNode.actionText);	
	}
	if (bNode.instantPerform) {
		bNode.accept();
	}
}
} // namespace(s)
//=============================================================================
//  Class FrxMainBrowserCtrl
//=============================================================================
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
		handleBrowserNodeResult(res);
	} catch(const std::exception &ex) {
		view->errorMessage(ex.what());
	} catch (...) {
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::onFrxViewChanged(void *, const FrxCircuidViewEvent &ev) 
{
	if (ev.type == FrxCircuidViewEvent::ComponentAdded) {
		addToSceneTree(ev.component, Add);
	}
	if (ev.type == FrxCircuidViewEvent::ComponentUpdated) {
		addToSceneTree(ev.component, Update);
	}
	// removing listeners will be installend while adding component
	// to scenetree(because we have the concerning tree nodes there. 
	// So no further ComponentRemoved handling in here.
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::handleBrowserNodeResult(BrowserNode::ResultPtr res) {

}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::
showShellSelection(const ::processing::PluginInfo &pI, 
	const ::processing::ShellPluginInfos &infos) 
{
	ShellPluginSelection::Ptr shlsl;
	shlsl = ShellPluginSelection::create();
	FrxCircuidViewPtr view = wView.lock();
	getFrxControl(view).addWindow(shlsl);
	BOOST_FOREACH(const ::processing::ShellPluginInfo &inf, infos) {
		shlsl->addShellInfo(inf);
	}
	shlsl->sce::EventSender<ShellPluginSelection::ActionEvent>::addTrackedEventListener
	(
		boost::bind(&FrxMainBrowserCtrl::onShellPluginSelected, this, _1, _2),
		self
	);
	shlsl->setShellPlugin(pI);
	shlsl->pack();
	shlsl->open();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::
onShellPluginSelected(void*, const sdc::events::ActionEvent &ev)
{
	ShellPluginSelection::Ptr shlsl =
		boost::dynamic_pointer_cast<ShellPluginSelection>(ev.getSource());
	if (!shlsl)
		return;
	const ::processing::PluginInfo &pI
		= shlsl->getCurrentSelection();
	if (pI.location == "") {
		return;
	}
	BrowserNode::ResultPtr res = addPlugin(pI);
	handleBrowserNodeResult(res);
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr 
FrxMainBrowserCtrl::fillPluginFolder(TreeNode parent, DBFolderID dbFolderId)
{

	FrxColumnBrowserPtr brws = this->browser.lock();
	FrxCircuidViewPtr view = wView.lock();
	SAMBAG_ASSERT(brws && view);
	FrxMainBrowserCtrl::Ptr ctrl = 
		boost::dynamic_pointer_cast<FrxMainBrowserCtrl>(brws->getCtrl());
	SAMBAG_ASSERT(ctrl);
	Tree::Ptr tree = brws->getBrowserImpl();
	if (tree->getNumChildren(parent) > 0)
		return BrowserNode::ResultPtr();
	::com::PluginCollection::Ptr db;
	try {
		db = ::com::getPluginCollection();
	} catch (...) {
		return BrowserNode::ResultPtr();
	}
	::com::PluginCollection::Folders folders;
	try {
		db->getSubFolders(dbFolderId, folders);
	} catch (...) {
		return BrowserNode::ResultPtr();
	}
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
	try {
		db->getPlugInfoList(dbFolderId, plugs, true);
	} catch (...) {
		return BrowserNode::ResultPtr();
	}
	BOOST_FOREACH(const ::processing::PluginInfo &pI, plugs) { 
		TreeNode plug = 
			tree->addNode(parent);

		std::string name =
			boost::filesystem::path(pI.location).filename().string();
		if (pI.access == ::processing::PluginInfo::FAILED) {
			name+="<FAILED>";
		}
		BrowserNode node;
		ctrl->createPluginNode(node, name);
		node.f = 
			boost::bind(&FrxMainBrowserCtrl::addPlugin, 
			this,
			pI			 // plugin info
		);
		node.type = pI.isSynth ? BrowserConstants::FRX_BROWSER_PLUGIN_INSTRUMENT :
			BrowserConstants::FRX_BROWSER_PLUGIN;
		tree->setNodeData(plug, node);
	}

	tree->updateLists();
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::fillHistoryFolder(TreeNode parent,
    HistoryType type)
{
	FrxColumnBrowserPtr brws = this->browser.lock();
	FrxCircuidViewPtr view = wView.lock();
	SAMBAG_ASSERT(brws && view);
	FrxMainBrowserCtrl::Ptr ctrl = 
		boost::dynamic_pointer_cast<FrxMainBrowserCtrl>(brws->getCtrl());
	SAMBAG_ASSERT(ctrl);
    Tree::Ptr tree = brws->getBrowserImpl();
	::com::PluginCollection::Ptr db;
    ::com::PluginCollection::PluginInfoList plugs;
    std::vector<std::string> details;
	try {
		db = ::com::getPluginCollection();
        if (type == Recent) {
            db->getRecentPlugins(plugs, details);
        } else if(type == Favourite) {
            db->getFavouritePlugins(plugs, details);
        }
	} catch (...) {
		return BrowserNode::ResultPtr();
	}
    int c=0;
 	BOOST_FOREACH(const ::processing::PluginInfo &pI, plugs) { 
		TreeNode plug = 
			tree->addNode(parent);

		std::string name =
			boost::filesystem::path(pI.location).filename().string();
		if (pI.access == ::processing::PluginInfo::FAILED) {
			name+="<FAILED>";
		}
        name+=" "+details[c++];
		BrowserNode node;
		ctrl->createPluginNode(node, name);
		node.f = 
			boost::bind(&FrxMainBrowserCtrl::addPlugin, 
			this,
			pI			 // plugin info
		);
		node.type = pI.isSynth ? BrowserConstants::FRX_BROWSER_PLUGIN_INSTRUMENT :
			BrowserConstants::FRX_BROWSER_PLUGIN;
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
	FrxProcessorNodePtr pr;
	try {
		pr = fac.getPluginCreator()(view, pI);
	} catch(const ::processing::ShellPluginException &ex) {
        // select plugin and calls addPlugin again
		showShellSelection(pI, ex.content);
		return BrowserNode::ResultPtr();
	}
	if (!pr)
		return BrowserNode::ResultPtr();
	getFrxControl(view).addProcessorToView(view, pr);
	
    addPluginToHistory(pI);
    
    FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (browser) {
		browser->message(pr->getName() + ADDED_TO_SCENE);
	}
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addPluginToHistory(::processing::PluginInfo pI) {
    ::com::PluginCollection::Ptr db;
	try {
		db = ::com::getPluginCollection();
	} catch (...) {
		SAMBAG_LOG_ERR<<"adding plugin to history failed.";
        return;
	}
    db->addToHistory(pI);
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
	FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (browser) {
		browser->message(pr->getName() + ADDED_TO_SCENE);
	}
	return BrowserNode::ResultPtr();
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
	FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (browser) {
		browser->message(pr->getName() + ADDED_TO_SCENE);
	}
	return BrowserNode::ResultPtr();
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
	FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (browser) {
		browser->message(pr->getName() + ADDED_TO_SCENE);
	}
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
addRelatedKnobToView(FrxComponentWPtr _c, processing::IParameter::WPtr _par)
{
	fgc::FrxCircuidViewPtr view = wView.lock();
	fgc::FrxComponentPtr c = _c.lock();
	frx::processing::IParameter::Ptr par = _par.lock();
	if (!view || !c || !par)
		return BrowserNode::ResultPtr();
	FrxComponentPtr knob = 
		getFrxControl(view).addRelatedKnobToView(view, c, par);
	FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (browser) {
		browser->message(par->getName() + ADDED_TO_SCENE);
	}
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initListeners(FrxColumnBrowserPtr brws) {
	FrxMainBrowser::Ptr browser = 
		boost::dynamic_pointer_cast<FrxMainBrowser>(brws);
	if (!browser)
		return;
	// add btnOk listener
	browser->getBtnAdd()->sdc::EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&FrxMainBrowserCtrl::onBrowserOk, this, _1, _2)
	);
	Tree::Ptr tree = brws->getBrowserImpl();
	// sel. path listener
	tree->sdc::EventSender<sdc::SelectionPathChanged>::addTrackedEventListener(
		boost::bind(&onSelectionPathChanged, _1, _2, FrxColumnBrowser::WPtr(brws)),
		self.lock()
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
		createProcessorNode(node, getProcessorBeautyName(name));
		node.f = boost::bind(&FrxMainBrowserCtrl::addProcessor, 
			this, f);
		node.type = BrowserConstants::FRX_BROWSER_PROCESSOR;
		tree->addNode(add_processors, node);
	}
}
//-----------------------------------------------------------------------------
namespace {
	template <class T>
	struct RefreshBrowser { 
		void update(const T &val) {
			FrxColumnBrowserPtr brws = val.lock();
			if (!brws)
				return;
			// TODO: update specific list or better specific entry
			brws->getBrowserImpl()->redraw();
		}
	};
}
void FrxMainBrowserCtrl::parameterChanged(void *src, 
		float value, const BrowserNode &node)
{
	typedef TimedUpdater<FrxColumnBrowserWPtr, RefreshBrowser, 100> Updater;
	Updater::instance().update(browser);
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
	brws->message(p->getName() + ": " + p->getDisplay());
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::parameterLabelRedraw( sdc::AComponentPtr c,
	processing::IParameter::WPtr _p,
	const BrowserNode &node)
{
	processing::IParameter::Ptr p = _p.lock();
	FrxParameterLabel::Ptr label = 
		boost::dynamic_pointer_cast<FrxParameterLabel>(c);
	if (!label || !p)
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
namespace {
BrowserNode::ResultPtr _setPreset(
	FrxColumnBrowser::WPtr brws,
	processing::IProcessor::WPtr obj, 
	int presetIndex) 
{
	processing::IProcessor::Ptr pr = obj.lock();
	if (!pr) {
		return BrowserNode::ResultPtr();
	}
	pr->setPreset(presetIndex);
	FrxColumnBrowser::Ptr browser = brws.lock();
	if (browser) {
		browser->message("set " + pr->getPresetName(presetIndex) + ".");
	}
	return BrowserNode::ResultPtr();
}
} // namespace(s)
void FrxMainBrowserCtrl::createPresetNode(BrowserNode &out,
	const std::string &name,
	processing::IProcessor::Ptr obj,
	int presetIndex)
{
	out.name = name;
	out.type = BrowserConstants::FRX_BROWSER_PRESET;
	processing::IProcessor::WPtr pr = obj;
	out.f = boost::bind(&_setPreset, browser, pr, presetIndex);
	out.instantPerform = true;
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
	createParameterNode(node, "Free Parameter");
	IFrxComponentFactory::FreeParameterCreator f = fac.getFreeParameterCreator();
	node.type = BrowserConstants::FRX_BROWSER_PARAMETER;
	node.f = boost::bind(&FrxMainBrowserCtrl::addFreeKnob, 
		this, f);
	tree->addNode(add_knobs, node);
	// host add_knobs:
	frx::processing::IModelController::Ptr ctrl
		= frx::processing::getModelController(view);
	Tree::Node hostKnobs = tree->addNode(add_knobs, BrowserNode ("Host Parameter", true));
	int nbKnobs = ctrl->getNumHostParameter();
	IFrxComponentFactory::HostParameterCreator hPcreator = fac.getHostParameterCreator();
	for (int i=0; i<nbKnobs; ++i) {
		BrowserNode node;
		processing::IParameter::Ptr par = 
			ctrl->getHostParameter(i);
		createParameterNode(node, "Host Parameter: " + sambag::com::toString(i), par);
		node.f = boost::bind(&FrxMainBrowserCtrl::addHostKnob, 
			this, hPcreator, i);
		tree->addNode(hostKnobs, node);
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initRoot(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws)
{
	FrxCircuidViewWPtr wView = view;
	Tree::Ptr tree = brws->getBrowserImpl();

	// scene tree
	scene = 
		tree->addNode(tree->getRootNode());
		
	BrowserNode node("Main Scene", true);
	node.f = 
		boost::bind(&FrxMainBrowserCtrl::createSceneTree, this, scene);
	tree->setNodeData(scene, node);
	
	scene_plugins = tree->addNode(scene, BrowserNode("Plugins", true));
	scene_processors = tree->addNode(scene, BrowserNode("Processors", true));
	scene_parameter = tree->addNode(scene, BrowserNode("Parameter", true));
	// add_knobs
	add_knobs = 
		tree->addNode(scene_parameter, BrowserNode("Add Parameter", 
			BrowserConstants::FRX_BROWSER_ADD_CONTENT_FOLDER));
	addMainKnobs();
	scene_connections = tree->addNode(scene_parameter, BrowserNode("Parameter Connections", true));

	// add_plugins
	add_plugins = tree->addNode(scene_plugins);
	
	node = BrowserNode("Add Plugin", 
		BrowserConstants::FRX_BROWSER_ADD_CONTENT_FOLDER);
	node.f = 
		boost::bind(&FrxMainBrowserCtrl::fillPluginFolder, 
		this,							    // browser	
		add_plugins,							// parent (browser) folder node
		::com::PluginCollection::ROOT_FOLDER_ID // parent db-folderid
	);
	tree->setNodeData(add_plugins, node);
    
    // history
    his_recent = tree->addNode(scene_plugins);
    node = BrowserNode("History", BrowserConstants::FRX_BROWSER_HISTORY_FOLDER);
	node.f = 
		boost::bind(&FrxMainBrowserCtrl::fillHistoryFolder, this, his_recent, Recent);
	tree->setNodeData(his_recent, node);

    his_favourite = tree->addNode(his_recent);
    node = BrowserNode("Favourites", BrowserConstants::FRX_BROWSER_FOLDER);
	node.f = 
		boost::bind(&FrxMainBrowserCtrl::fillHistoryFolder, this, his_favourite, Favourite);
	tree->setNodeData(his_favourite, node);


	// add_processors
	add_processors = 
		tree->addNode(scene_processors, BrowserNode("Add Processor", 
			BrowserConstants::FRX_BROWSER_ADD_CONTENT_FOLDER));
	addMainProcessors();
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
void FrxMainBrowserCtrl::onSceneIOChanged(void*, 
	const frx::processing::IProcessor::IOChangedEvent &ev, 
	Tree::Node node)
{
	FrxColumnBrowserPtr brws = browser.lock();
	FrxCircuidViewPtr view = wView.lock();
	IViewModelMap::Ptr map = getViewModelMap(view);
	if (!brws || !view || !map)
		return;
	Tree::Ptr tree = brws->getBrowserImpl();
	FrxComponentPtr c = boost::dynamic_pointer_cast<FrxComponent> (
		map->getViewObject(ev.src)
	);
	if (!c)
		return;
	// remove old entries
	std::list<Tree::Node> children;
	tree->getChildren(node, children);
	BOOST_FOREACH(const Tree::Node &i, children) {
		tree->removeNode(i);
	}
	// add new entries
	addModelObjectParameter(c, node);
	tree->updateLists();
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::
addPluginToSceneTree(FrxComponentPtr c, Reason reason)
{
	
	FrxCircuidViewPtr view = wView.lock();

	FrxProcessorNode::Ptr pr = 
		boost::dynamic_pointer_cast<FrxProcessorNode>(c);
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!pr || !browser) {
		return Tree::NULL_NODE;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node plugin = tree->addNode(scene_plugins);

	BrowserNode browserNode(c->getName(), true);
	browserNode.f = // dynamic parameter insert
		boost::bind(&FrxMainBrowserCtrl::_addModelObjectParameter,
		this,
		FrxComponentWPtr(c),
		plugin
	);
	tree->setNodeData(plugin, browserNode);
	tree->updateLists();
	return plugin;
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::addProcessorToSceneTree(FrxComponentPtr c, Reason reason)
{
	
	FrxCircuidViewPtr view = wView.lock();
	FrxProcessorNode::Ptr pr = 
		boost::dynamic_pointer_cast<FrxProcessorNode>(c);
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!pr || !browser) {
		return Tree::NULL_NODE;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node newNode = tree->addNode(scene_processors);

	BrowserNode browserNode(c->getName(), true);
	addModelObjectParameter(pr, newNode);
	tree->setNodeData(newNode, browserNode);
	// install io changed event sender
	IViewModelMap::Ptr map = getViewModelMap(view);
	frx::processing::IProcessor::Ptr primpl = 
		boost::dynamic_pointer_cast<frx::processing::IProcessor>(
			map->getModelObject(pr)
		);
	SAMBAG_ASSERT(primpl);
	primpl->addTrackedIOChangedListener(
		boost::bind(&FrxMainBrowserCtrl::onSceneIOChanged, this, _1, _2, newNode),
		self
	);
	tree->updateLists();
	// return result
	return newNode;
}
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::addParameterToSceneTree(FrxComponentPtr c, Reason reason)
{
	FrxCircuidViewPtr view = wView.lock();
	FrxParameter::Ptr parComp = 
		boost::dynamic_pointer_cast<FrxParameter>(c);
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!parComp || !browser) {
		return Tree::NULL_NODE;
	}
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	frx::processing::IParameter::Ptr prPar = 
		boost::dynamic_pointer_cast<frx::processing::IParameter> (
			map->getModelObject(parComp)
		);
	if (!prPar)
		return Tree::NULL_NODE;

	BrowserNode nodeData;
	createParameterNode(nodeData, prPar->getName(), prPar);
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node newNode = tree->addNode(scene_parameter, nodeData);
	tree->updateLists();
	return newNode;
}
//-----------------------------------------------------------------------------
FrxMainBrowserCtrl::Id FrxMainBrowserCtrl::getId(FrxComponentPtr c) const {
	return (Id)c.get();
}
//-----------------------------------------------------------------------------
FrxMainBrowserCtrl::TreeNode FrxMainBrowserCtrl::getTreeNode(Id id) const {
	NodeMap::const_iterator it = nodeMap.find(id);
	if (it==nodeMap.end()) {
		return Tree::NULL_NODE;
	}
	return it->second;
}	
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::updateConnectionInSceneTree(FrxComponentPtr c) {
	TreeNode n = getTreeNode(getId(c));
	if (n==Tree::NULL_NODE) {
		return n;
	}
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!browser) {
		return Tree::NULL_NODE;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	tree->removeNode(n);
	// TODO: cheap approach
	return addConnectionToSceneTree(c, Add);
}	
//-----------------------------------------------------------------------------
Tree::Node FrxMainBrowserCtrl::addConnectionToSceneTree(FrxComponentPtr c, Reason reason)
{
	if (reason==Update) {
		return updateConnectionInSceneTree(c);
	}
	FrxCircuidViewPtr view = wView.lock();
	FrxConnection::Ptr pr = 
		boost::dynamic_pointer_cast<FrxConnection>(c);
	FrxColumnBrowserPtr browser = this->browser.lock();
	if (!pr || !browser) {
		return Tree::NULL_NODE;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node newNode = tree->addNode(scene_connections);
	nodeMap[getId(c)] = newNode;
	BrowserNode browserNode(c->getName(), true);
	addModelObjectParameter(pr, newNode);
	tree->setNodeData(newNode, browserNode);
	tree->updateLists();
	// return result
	return newNode;
}
//-----------------------------------------------------------------------------
FrxMainBrowserCtrl::AddComponentHandler
FrxMainBrowserCtrl::getAddComponentHandler(FrxComponentPtr c)
{
    // processors:
    if ( dynamic_cast<FrxPluginNode*>(c.get()) ) {
        return &FrxMainBrowserCtrl::addPluginToSceneTree;
    } else if ( dynamic_cast<FrxProcessorNode*>(c.get()) ) {
        return &FrxMainBrowserCtrl::addProcessorToSceneTree;
    }
    
    // parameters
    if ( dynamic_cast<FrxStdKnob*>(c.get()) ) {
        return &FrxMainBrowserCtrl::addParameterToSceneTree;
    }
    
    // connections:
    if ( dynamic_cast<ParameterCn*>(c.get()) ) {
        return &FrxMainBrowserCtrl::addConnectionToSceneTree;
    }
    
    // default:
    return NULL;
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addToSceneTree(FrxComponentPtr c, Reason reason) {
	if (!sceneTreeInit) 
		return;
    AddComponentHandler hnd = getAddComponentHandler(c);
    if (!hnd) {
        return;
    }
	Tree::Node res = (this->*hnd)(c, reason);
	if (res==Tree::NULL_NODE)
		return;
	c->sce::EventSender<OnRemoving>::addTrackedEventListener(
		boost::bind(&FrxMainBrowserCtrl::onRemovingFromScene, this, _1, _2, res),
		self
	);
	FrxColumnBrowser::Ptr browser = this->browser.lock();
	if (!browser) {
		return;
	}
	Tree::Ptr tree = browser->getBrowserImpl();
	tree->updateLists();
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
		FrxCircuidView::Z_Wires
	);
	// collect components
	BOOST_FOREACH(sdc::AComponentPtr c, components) {
		FrxComponentPtr fc = 
			boost::dynamic_pointer_cast<FrxComponent>(c);
		if (!fc) {
			continue;
		}
		addToSceneTree(fc, Add);
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
	view->sdc::EventSender<FrxCircuidViewEvent>::addTrackedEventListener(
		boost::bind(&FrxMainBrowserCtrl::onFrxViewChanged, this, _1, _2),
		self.lock()
	);
	// create browser tree
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = brws->getBrowserImpl();
	initRoot(view, brws);

	tree->updateLists();
}
//-----------------------------------------------------------------------------
BrowserNode::ResultPtr FrxMainBrowserCtrl::
_addModelObjectParameter(FrxComponentWPtr c, Tree::Node parent)
{
	typedef FrxColumnBrowser::BrowserImpl Tree;
	Tree::Ptr tree = browser.lock()->getBrowserImpl();
	if (tree->getNumChildren(parent) > 0)
		return BrowserNode::ResultPtr();
	
	addModelObjectParameter(c.lock(), parent);
	tree->updateLists();
	return BrowserNode::ResultPtr();
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addPresets(FrxComponentPtr c, const Tree::Node &parent)
{
	using frx::processing::IProcessor;
	FrxCircuidViewPtr view = wView.lock();
	if (!view || ! c)
		return;
	frx::processing::IModelController::Ptr ctrl;
	IViewModelMap::Ptr map;
	boost::tie(ctrl, map) = getControllerAndMap(view);
	IProcessor::Ptr pr = boost::dynamic_pointer_cast<IProcessor>
		(map->getModelObject(c));
	if (!pr)
		return;
	size_t numPresets = pr->getNumPresets();
	if (numPresets==0) {
		return;
	}
	typedef FrxColumnBrowser::BrowserImpl Tree;
	FrxColumnBrowserPtr browser = this->browser.lock();
	Tree::Ptr tree = browser->getBrowserImpl();
	Tree::Node presets = tree->addNode(parent, BrowserNode("presets", true));
	for (size_t i=0; i<numPresets; ++i) {
		BrowserNode node;
		createPresetNode(node, pr->getPresetName(i), pr, i);
		tree->addNode(
			presets, 
			node
		);
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::
addModelObjectParameter(FrxComponentPtr c,
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

	frx::processing::ModelObject::Ptr pr = map->getModelObject(c);
	if (!pr)
		return;

	// get frxctrl
	IFrxControl & frxctrl = getFrxControl(view);
	using frx::processing::IProcessor;
	using frx::processing::IParameter;
	frx::processing::ModelObject::ParameterGroupKeys keys;
	pr->getParameterGroupKeys(keys);
	// create parameter nodes
	BOOST_FOREACH(const IProcessor::ParameterGroupKey &key, keys) {
		Tree::Node parameterParent;
		if (key==".") {
			parameterParent = parent;
		} else {
			parameterParent = tree->addNode(parent, BrowserNode(key, true));
		}

		IProcessor::Parameters parameters;
		pr->getParameters(key, parameters);
		BOOST_FOREACH(IParameter::Ptr p, parameters) {
			BrowserNode::AcceptedFunction f = 
				boost::bind(&FrxMainBrowserCtrl::addRelatedKnobToView,
				this,
				fgc::FrxComponentWPtr(c),
				frx::processing::IParameter::WPtr(p)
			);
			BrowserNode node;
			createParameterNode(node, p->getName(), p);
			node.f = f;
			tree->addNode(
				parameterParent, 
				node
			);
		}
	}
	addPresets(c, parent);
}
}}} // namespace(s)
