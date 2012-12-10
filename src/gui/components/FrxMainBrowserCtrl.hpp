/*
 * FrxMainBrowserCtrl.hpp
 *
 *  Created on: Wed Oct 31 10:48:34 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXMAINBROWSERCTRL_H
#define SAMBAG_FRXMAINBROWSERCTRL_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "IFrxColumnBrowserCtrl.hpp"
#include <processing/IHostInfo.h>
#include "FrxColumnBrowser.hpp"
#include <processing/IParameter.hpp>
#include <processing/IProcessor.hpp>
#include <com/PluginCollection.h>
#include "IFrxComponentFactory.hpp"
#include <loki/LokiTypeinfo.h>
#include <sambag/com/ArithmeticWrapper.hpp>
#include "FrxComponent.hpp"
#include <processing/pluginTypes/VstShellPlugin.hpp>
#include <sambag/disco/components/events/ActionEvent.hpp>

namespace frx { namespace gui { namespace components {
struct FrxCircuidViewEvent;
//=============================================================================
/** 
  * @class FrxMainBrowserCtrl.
  * @note: for performance reasons the plugin tree will be created
  *        dynamically
  */
class FrxMainBrowserCtrl : public IFrxColumnBrowserCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<FrxMainBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<FrxMainBrowserCtrl> WPtr;
	//-------------------------------------------------------------------------
	typedef ::frx::processing::IHostInfo IHostInfo;
	//-------------------------------------------------------------------------
	typedef IFrxColumnBrowserCtrl Super;
	//-------------------------------------------------------------------------
	typedef FrxColumnBrowser::BrowserImpl Tree;
	//-------------------------------------------------------------------------
	typedef Tree::Node TreeNode;
	//-------------------------------------------------------------------------
	typedef ::com::PluginCollection::FolderID DBFolderID;
protected:
	//-------------------------------------------------------------------------
	void onSceneIOChanged(void*, 
		const frx::processing::IProcessor::IOChangedEvent &ev, 
		Tree::Node node);
	//-------------------------------------------------------------------------
	void onRemovingFromScene(void*, const OnRemoving &ev, Tree::Node node);
	//-------------------------------------------------------------------------
	void onBrowserOk(void *src, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	void parameterLabelChanged(float value, processing::IParameter::WPtr _p);
	//-------------------------------------------------------------------------
	void parameterChanged(void *src, 
		float value, const BrowserNode &node);
	//-------------------------------------------------------------------------
	void onFrxViewChanged(void *, const FrxCircuidViewEvent &ev);
	//-------------------------------------------------------------------------
	void parameterLabelRedraw(sdc::AComponentPtr c, 
		processing::IParameter::WPtr _p,
		const BrowserNode &node);
	//-------------------------------------------------------------------------
	FrxColumnBrowserWPtr browser;
	//-------------------------------------------------------------------------
	FrxCircuidViewWPtr wView;
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	FrxMainBrowserCtrl() {}
	//-------------------------------------------------------------------------
	IHostInfo::Ptr hostInfo;
	//-------------------------------------------------------------------------
	void initRoot(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
	//-------------------------------------------------------------------------
	Tree::Node add, add_plugins, add_processors, add_knobs, scene, scene_processors,
		scene_plugins, scene_parameter, scene_connections;
	//-------------------------------------------------------------------------
	void addModelObjectParameter(FrxComponentPtr c,
		const Tree::Node &parent);
	//-------------------------------------------------------------------------
	// wrapper for BrowserNode Accepted function
	BrowserNode::ResultPtr _addModelObjectParameter(FrxComponentWPtr c,
		Tree::Node parent);
	//-------------------------------------------------------------------------
	void addMainProcessors();
	//-------------------------------------------------------------------------
	void addMainKnobs();
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr createSceneTree(const Tree::Node &parent);
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr addPlugin(::processing::PluginInfo pI);
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr 
	addProcessor(IFrxComponentFactory::ProcessorCreator f);
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr 
	addFreeKnob(IFrxComponentFactory::FreeParameterCreator f);
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr 
	addHostKnob(IFrxComponentFactory::HostParameterCreator f, int id);
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr 
	addRelatedKnobToView(FrxComponentWPtr _c, frx::processing::IParameter::WPtr _par);
	//-------------------------------------------------------------------------
	BrowserNode::ResultPtr 
	fillPluginFolder(TreeNode parent, DBFolderID dbFolderId);
	//-------------------------------------------------------------------------
	void addToSceneTree(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	void showShellSelection(const ::processing::PluginInfo &plugin, 
		const ::processing::ShellPluginInfos &infos);
	//-------------------------------------------------------------------------
	void onShellPluginSelected(void*, const sdc::events::ActionEvent &ev);
private:
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> sceneTreeInit;
	//-------------------------------------------------------------------------
	typedef boost::function<Tree::Node(FrxComponentPtr)> 
		SceneTreeAdder;
	typedef std::map<Loki::TypeInfo, SceneTreeAdder> AdderMap;
	AdderMap adderMap;
	//-------------------------------------------------------------------------
	void initAdderMap();
	//-------------------------------------------------------------------------
	Tree::Node addPluginToSceneTree(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	Tree::Node addProcessorToSceneTree(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	Tree::Node addParameterToSceneTree(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	Tree::Node addConnectionToSceneTree(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	void handleBrowserNodeResult(BrowserNode::ResultPtr res);
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	void createParameterNode(BrowserNode &out, 
		const std::string &name,
		processing::IParameter::Ptr obj = processing::IParameter::Ptr());
	//-------------------------------------------------------------------------
	void createProcessorNode(BrowserNode &out, 
		const std::string &name,
		processing::IProcessor::Ptr obj = processing::IProcessor::Ptr());
	//-------------------------------------------------------------------------
	void createPluginNode(BrowserNode &out,
		const std::string &name,
		processing::IProcessor::Ptr obj = processing::IProcessor::Ptr());
	//-------------------------------------------------------------------------
	virtual void initListeners(FrxColumnBrowserPtr brws);
	//-------------------------------------------------------------------------
	IHostInfo::Ptr getHostInfo() const {
		return hostInfo;
	}
	//-------------------------------------------------------------------------
	void setHostInfo(IHostInfo::Ptr hostInfo);
	//-------------------------------------------------------------------------
	static Ptr create() {
		Ptr res(new FrxMainBrowserCtrl());
		res->self = res;
		return res;
	}
	//-------------------------------------------------------------------------
	virtual void initTree(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
public:
}; // FrxMainBrowserCtrl
}}} // namespace(s)

#endif /* SAMBAG_FRXMAINBROWSERCTRL_H */
