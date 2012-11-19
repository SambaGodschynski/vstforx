/*
 * FrxMainBrowserCtrl.cpp
 *
 *  Created on: Wed Oct 31 10:48:34 2012
 *      Author: Johannes Unger
 */

#include "FrxMainBrowserCtrl.hpp"
#include "FrxMainBrowser.hpp"
#include <com/PluginCollection.h>
#include <gui/FrxControl.hpp>
#include "IFrxComponentFactory.hpp"
#include <boost/foreach.hpp>
#include <list>
#include <string>
#include <sambag/com/Exception.hpp>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <gui/components/FrxParameterLabel.hpp>
#include <gui/components/ui/FrxBrowserListUI.hpp>

namespace frx { namespace gui { namespace components {
namespace {
//-----------------------------------------------------------------------------
// boost::tuples::tuple<std::string, FolderID>
typedef ::com::PluginCollection::Folder DBFolderType;
typedef ::com::PluginCollection::FolderID DBFolderID;
typedef FrxMainBrowserCtrl::Tree Tree;
typedef FrxMainBrowserCtrl::Tree::Node TreeNode;
//-----------------------------------------------------------------------------
void addPlugin(FrxCircuidViewWPtr _view, ::processing::PluginInfo pI) {
	FrxCircuidViewPtr view = _view.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add plugin with FrxCircuidViewPtr == NULL");
	}
	IFrxComponentFactory &fac = getComponentFactory(view);
	FrxProcessorNodePtr pr = fac.getPluginCreator()(view, pI);
	if (!pr)
		return;
	getFrxControl(view).addProcessorToView(view, pr);
}
//-----------------------------------------------------------------------------
void addProcessor(FrxCircuidViewWPtr _view, IFrxComponentFactory::ProcessorCreator f) 
{
	FrxCircuidViewPtr view = _view.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add processor with FrxCircuidViewPtr == NULL");
	}
	FrxProcessorNodePtr pr = f(view);
	if (!pr)
		return;
	getFrxControl(view).addProcessorToView(view, pr);
}
//-----------------------------------------------------------------------------
void addFreeKnob(FrxCircuidViewWPtr _view, 
	IFrxComponentFactory::FreeParameterCreator f) 
{
	FrxCircuidViewPtr view = _view.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add knob with FrxCircuidViewPtr == NULL");
	}
	FrxParameterPtr pr = f(view);
	if (!pr)
		return;
	getFrxControl(view).addParameterToView(view, pr);
}
//-----------------------------------------------------------------------------
void addHostKnob(FrxCircuidViewWPtr _view, 
	IFrxComponentFactory::HostParameterCreator f, int id) 
{
	FrxCircuidViewPtr view = _view.lock();
	if (!view) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to add knob with FrxCircuidViewPtr == NULL");
	}
	FrxParameterPtr pr = f(view, id);
	if (!pr)
		return;
	getFrxControl(view).addParameterToView(view, pr);
}
//-----------------------------------------------------------------------------
void onBrowserOk(void *src,
	const sdc::events::ActionEvent &ev,
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
	try {
		bNode.accept();
	} catch(const std::exception &ex) {
	} catch (...) {
		// TODO
	}
}
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
//-----------------------------------------------------------------------------
void fillPluginFolder(FrxColumnBrowserWPtr _brws, FrxCircuidViewWPtr _view,
	TreeNode parent, DBFolderID dbFolderId)
{

	FrxColumnBrowserPtr brws = _brws.lock();
	FrxCircuidViewPtr view = _view.lock();
	SAMBAG_ASSERT(brws && view);
	FrxMainBrowserCtrl::Ptr ctrl = 
		boost::shared_dynamic_cast<FrxMainBrowserCtrl>(brws->getCtrl());
	SAMBAG_ASSERT(ctrl);
	Tree::Ptr tree = brws->getBrowserImpl();
	if (tree->getNumChildren(parent) > 0)
		return;

	::com::PluginCollection &db = ::com::getPluginCollection();
	::com::PluginCollection::Folders folders;
	db.getSubFolders(dbFolderId, folders);
	
	BOOST_FOREACH(const DBFolderType &dbF, folders) { // subfolder
		TreeNode treeFolder = 
			tree->addNode(parent);

		const std::string &name = boost::get<0>(dbF);
		DBFolderID dbID =  boost::get<1>(dbF);
		BrowserNode node(name, true);
		node.f = 
			boost::bind(&fillPluginFolder, 
			_brws,						// browser	
			_view,						// view
			treeFolder,				    // parent (browser) folder node
			dbID						// parent db-folderid
		);
		tree->setNodeData(treeFolder, node);
	}

	::com::PluginCollection::PluginInfoList plugs;
	db.getPlugInfoList(dbFolderId, plugs);
	BOOST_FOREACH(const ::processing::PluginInfo &pI, plugs) { // plugins
		TreeNode plug = 
			tree->addNode(parent);

		const std::string &name = pI.name;
		BrowserNode node;
		ctrl->createPluginNode(node, name);
		node.f = 
			boost::bind(&addPlugin, 
			_view,		 // view	
			pI			 // plugin info
		);
		node.type = BrowserConstants::FRX_BROWSER_PLUGIN;
		tree->setNodeData(plug, node);
	}

	tree->updateLists();
}
} // namespace(s)
//=============================================================================
//  Class FrxMainBrowserCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initListeners(FrxColumnBrowserPtr brws) {
	FrxMainBrowser::Ptr browser = 
		boost::shared_dynamic_cast<FrxMainBrowser>(brws);
	if (!browser)
		return;
	// add btnOk listener
	browser->getBtnAdd()->EventSender<sdc::events::ActionEvent>::addEventListener(
		boost::bind(&onBrowserOk, _1, _2, FrxColumnBrowser::WPtr(browser))
	);
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::setHostInfo(IHostInfo::Ptr hostInfo) {
	this->hostInfo = hostInfo;
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::addProcessors(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws) 
{
	Tree::Ptr tree = brws->getBrowserImpl();
	IFrxComponentFactory &fac = getComponentFactory(view);
	std::list<std::string> processorNames;
	fac.getProcessorNames(processorNames);
	BOOST_FOREACH(const std::string &name, processorNames) {
		IFrxComponentFactory::ProcessorCreator f = fac.getProcessorCreator(name);
		BrowserNode node;
		createProcessorNode(node, name);
		node.f = boost::bind(&addProcessor, FrxCircuidViewWPtr(view), f);
		node.type = BrowserConstants::FRX_BROWSER_PROCESSOR;
		tree->addNode(processors, node);
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
void FrxMainBrowserCtrl::addKnobs(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws) 
{
	Tree::Ptr tree = brws->getBrowserImpl();
	IFrxComponentFactory &fac = getComponentFactory(view);
	BrowserNode node;
	createParameterNode(node, "free knob");
	IFrxComponentFactory::FreeParameterCreator f = fac.getFreeParameterCreator();
	node.type = BrowserConstants::FRX_BROWSER_PARAMETER;
	node.f = boost::bind(&addFreeKnob, FrxCircuidViewWPtr(view), f);
	tree->addNode(knobs, node);
	// host knobs:
	frx::processing::IModelController::Ptr ctrl
		= frx::processing::getModelController(view);
	Tree::Node hostKnobs = tree->addNode(knobs, BrowserNode ("host knobs", true));
	int nbKnobs = ctrl->getNumHostParameter();
	IFrxComponentFactory::HostParameterCreator hPcreator = fac.getHostParameterCreator();
	for (int i=0; i<nbKnobs; ++i) {
		BrowserNode node;
		processing::IParameter::Ptr par = 
			ctrl->getHostParameter(i);
		createParameterNode(node, "host knob: " + sambag::com::toString(i), par);
		node.f = boost::bind(&addHostKnob, FrxCircuidViewWPtr(view), hPcreator, i);
		tree->addNode(hostKnobs, node);
	}
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initRoot(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws)
{
	Tree::Ptr tree = brws->getBrowserImpl();
	// plugins
	plugins = 
		tree->addNode(tree->getRootNode());

	BrowserNode node("Plugins", true);
	node.f = 
		boost::bind(&fillPluginFolder, 
		FrxColumnBrowserWPtr(brws),			// browser	
		FrxCircuidViewWPtr(view),			// view
		plugins,							// parent (browser) folder node
		::com::PluginCollection::ROOT_FOLDER_ID // parent db-folderid
	);
	tree->setNodeData(plugins, node);

	// processors
	processors = 
		tree->addNode(tree->getRootNode(), BrowserNode("Processors", true));
	addProcessors(view, brws);
	
	// knobs
	knobs = 
		tree->addNode(tree->getRootNode(), BrowserNode("Knobs", true));
	addKnobs(view, brws);
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initTree(FrxCircuidViewPtr view, 
	FrxColumnBrowserPtr brws)
{
	browser = brws;
	
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
}}} // namespace(s)
