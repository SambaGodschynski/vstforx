/*
 * FrxMainBrowserCtrl.hpp
 *
 *  Created on: Wed Oct 31 10:48:34 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXMAINBROWSERCTRL_H
#define SAMBAG_FRXMAINBROWSERCTRL_H

#include <memory>
#include "IFrxColumnBrowserCtrl.hpp"
#include <processing/IHostInfo.h>
#include "FrxColumnBrowser.hpp"
#include <processing/IParameter.hpp>
#include <processing/IProcessor.hpp>
#include <com/PluginCollection.h>
#include "IFrxComponentFactory.hpp"
#include <loki/LokiTypeInfo.h>
#include <sambag/com/ArithmeticWrapper.hpp>
#include "FrxComponent.hpp"
#include <processing/pluginTypes/VstShellPlugin.hpp>
#include <sambag/disco/components/events/ActionEvent.hpp>
#include <sambag/disco/components/Timer.hpp>
#include <unordered_map>

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
	typedef std::shared_ptr<FrxMainBrowserCtrl> Ptr;
	//-------------------------------------------------------------------------
	typedef std::weak_ptr<FrxMainBrowserCtrl> WPtr;
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
		float value, const BrowserNodeData &node);
	//-------------------------------------------------------------------------
	void onFrxViewChanged(void *, const FrxCircuidViewEvent &ev);
	//-------------------------------------------------------------------------
	void parameterLabelRedraw(sdc::AComponentPtr c, 
		processing::IParameter::WPtr _p,
		const BrowserNodeData &node);
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
	Tree::Node add, add_plugins, add_processors, add_knobs, scene,
		scene_plugins, scene_parameter, scene_connections, his_recent, his_favourite,
        remotes;
	//-------------------------------------------------------------------------
	void addModelObjectParameter(FrxComponentPtr c,
		const Tree::Node &parent);
	//-------------------------------------------------------------------------
	void addPresets(FrxComponentPtr c, const Tree::Node &parent);
	//-------------------------------------------------------------------------
	// wrapper for BrowserNodeData Accepted function
	BrowserNodeData::ResultPtr _addModelObjectParameter(FrxComponentWPtr c,
		Tree::Node parent);
	//-------------------------------------------------------------------------
	void addMainProcessors();
	//-------------------------------------------------------------------------
	void addMainKnobs();
	//-------------------------------------------------------------------------
	BrowserNodeData::ResultPtr createSceneTree(const Tree::Node &parent);
	//-------------------------------------------------------------------------
	BrowserNodeData::ResultPtr addPlugin(::processing::PluginInfo pI);
    //-------------------------------------------------------------------------
	void addPluginToHistory(::processing::PluginInfo pI);
	//-------------------------------------------------------------------------
	/**
     * Browser entry callback:
     * add processor to view 
     */
    BrowserNodeData::ResultPtr
	addProcessor(IFrxComponentFactory::ProcessorCreator f);
	//-------------------------------------------------------------------------
	/**
     * Browser entry callback:
     * add free knob to view 
     */
	BrowserNodeData::ResultPtr 
	addFreeKnob(IFrxComponentFactory::FreeParameterCreator f);
	//-------------------------------------------------------------------------
    /**
     * Browser entry callback:
     * add hostknob to view 
     */
	BrowserNodeData::ResultPtr 
	addHostKnob(IFrxComponentFactory::HostParameterCreator f, int id);
	//-------------------------------------------------------------------------
	/**
     * Browser entry callback:
     * add processor related knob to view 
     */
	BrowserNodeData::ResultPtr 
	addRelatedKnobToView(FrxComponentWPtr _c, frx::processing::IParameter::WPtr _par);
	//-------------------------------------------------------------------------
	/**
     * Browser entry callback:
     * when clicked on plugin folder: fill folder
     */
	BrowserNodeData::ResultPtr 
	fillPluginFolder(TreeNode parent, DBFolderID dbFolderId);
	//-------------------------------------------------------------------------
	/**
     * Browser entry callback:
     * when clicked on plugin history folder: fill folder
     */
	enum HistoryType{ Recent, Favourite };
    BrowserNodeData::ResultPtr
	fillHistoryFolder(TreeNode parent, HistoryType type);
	//-------------------------------------------------------------------------
	void showShellSelection(const ::processing::PluginInfo &plugin, 
		const ::processing::ShellPluginInfos &infos);
	//-------------------------------------------------------------------------
	void onShellPluginSelected(void*, const sdc::events::ActionEvent &ev);
	//-------------------------------------------------------------------------
	/**
     * Browser entry callback:
     * when clicked on remote folder: fill folder
     */
    sambag::com::ArithmeticWrapper<int, -1> remotesChangedTimestamp;
	BrowserNodeData::ResultPtr fillRemoteFolder();
    //-------------------------------------------------------------------------
    /**
     * updates remote folders content
     */
    void updateRemoteFolder();
    void onRemotePoll(void *src, const sdc::TimerEvent &ev);
private:
    //-------------------------------------------------------------------------
    sdc::Timer::Ptr remotePoll;
	//-------------------------------------------------------------------------
	typedef long int Id;
	enum { NoId = INT_MAX };
	typedef std::unordered_map<Id, TreeNode> NodeMap; 
	//-------------------------------------------------------------------------
	Id getId(FrxComponentPtr c) const;
	//-------------------------------------------------------------------------
	TreeNode getTreeNode(Id id) const;
	//-------------------------------------------------------------------------
	NodeMap nodeMap;
	//-------------------------------------------------------------------------
	sambag::com::ArithmeticWrapper<bool> sceneTreeInit;
	//-------------------------------------------------------------------------
	enum Reason { Add, Update };
	typedef std::function<Tree::Node(FrxComponentPtr, Reason)> 
        SceneTreeEventHandler;
	//-------------------------------------------------------------------------
    typedef Tree::Node (FrxMainBrowserCtrl::* AddComponentHandler)
        (FrxComponentPtr c, Reason reason);
    AddComponentHandler getAddComponentHandler(FrxComponentPtr c);
    //-------------------------------------------------------------------------
	Tree::Node addPluginToSceneTree(FrxComponentPtr c, Reason reason);
	//-------------------------------------------------------------------------
	Tree::Node addProcessorToSceneTree(FrxComponentPtr c, Reason reason);
	//-------------------------------------------------------------------------
	Tree::Node addParameterToSceneTree(FrxComponentPtr c, Reason reason);
	//-------------------------------------------------------------------------
	Tree::Node updateConnectionInSceneTree(FrxComponentPtr c);
	//-------------------------------------------------------------------------
	Tree::Node addConnectionToSceneTree(FrxComponentPtr c, Reason reason);
	//-------------------------------------------------------------------------
	void handleBrowserNodeResult(BrowserNodeData::ResultPtr res);
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	void addToSceneTree(FrxComponentPtr c, Reason reason);
public:
	//-------------------------------------------------------------------------
	void createParameterNode(BrowserNodeData &out, 
		const std::string &name,
		processing::IParameter::Ptr obj = processing::IParameter::Ptr());
	//-------------------------------------------------------------------------
	void createProcessorNode(BrowserNodeData &out, 
		const std::string &name,
		processing::IProcessor::Ptr obj = processing::IProcessor::Ptr());
	//-------------------------------------------------------------------------
	void createPluginNode(BrowserNodeData &out,
		const std::string &name,
		processing::IProcessor::Ptr obj = processing::IProcessor::Ptr());
	//-------------------------------------------------------------------------
	void createRemoteNode(BrowserNodeData &out,
		const std::string &rcId);
	//-------------------------------------------------------------------------
	void createPresetNode(BrowserNodeData &out,
		const std::string &name,
		processing::IProcessor::Ptr obj = processing::IProcessor::Ptr(),
		int presetIndex = 0);
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
