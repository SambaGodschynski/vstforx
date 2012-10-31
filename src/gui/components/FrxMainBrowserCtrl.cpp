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
	SAMBAG_ASSERT(view);
	IFrxControl &ctrl = getFrxControl(view);
	ctrl.addPlugin(view, pI);
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
	if (!bNode.isFolder)
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
	Tree::Ptr tree = brws->getBrowserImpl();
	if (tree->getNumChildren(parent) > 0)
		return;

	::com::PluginCollection &db = ::com::getPluginCollection();
	::com::PluginCollection::Folders folders;
	db.getSubFolders(dbFolderId, folders);
	
	BOOST_FOREACH(const DBFolderType &dbF, folders) {
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
	BOOST_FOREACH(const ::processing::PluginInfo &pI, plugs) {
		TreeNode plug = 
			tree->addNode(parent);

		const std::string &name = pI.name;
		BrowserNode node(name, false);
		node.f = 
			boost::bind(&addPlugin, 
			_view,		 // view	
			pI			 // plugin info
		);
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
	
	// knobs
	knobs = 
		tree->addNode(tree->getRootNode(), BrowserNode("Knobs", true));
}
//-----------------------------------------------------------------------------
void FrxMainBrowserCtrl::initTree(FrxCircuidViewPtr view, 
	FrxColumnBrowserPtr brws)
{
	
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
