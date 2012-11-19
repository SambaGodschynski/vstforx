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

namespace frx { namespace gui { namespace components {
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
protected:
	//-------------------------------------------------------------------------
	void parameterLabelChanged(float value, processing::IParameter::WPtr _p);
	//-------------------------------------------------------------------------
	void parameterChanged(void *src, 
		float value, const BrowserNode &node);
	//-------------------------------------------------------------------------
	void parameterLabelRedraw(sdc::AComponentPtr c, 
		processing::IParameter::WPtr _p,
		const BrowserNode &node);
	//-------------------------------------------------------------------------
	FrxColumnBrowserWPtr browser;
	//-------------------------------------------------------------------------
	WPtr self;
	//-------------------------------------------------------------------------
	FrxMainBrowserCtrl() {}
	//-------------------------------------------------------------------------
	IHostInfo::Ptr hostInfo;
	//-------------------------------------------------------------------------
	void initRoot(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
	//-------------------------------------------------------------------------
	Tree::Node plugins, processors, knobs;
	//-------------------------------------------------------------------------
	void addProcessors(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
	//-------------------------------------------------------------------------
	void addKnobs(FrxCircuidViewPtr view, FrxColumnBrowserPtr brws);
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
