/*
 * SetupCtrl.hpp
 *
 *  Created on: Thu Oct 25 13:56:01 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_SETUPCTRL_H
#define SAMBAG_SETUPCTRL_H

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <string>
#include <list>
#include "Forward.hpp"
#include <processing/IHostInfo.h>
#include <sambag/disco/Geometry.hpp>
#include <gui/HandyNamespaces.hpp>
#include <sambag/disco/components/Forward.hpp>

namespace frx { namespace gui { namespace components {
//=============================================================================
/** 
  * @class SetupCtrl.
  */
class SetupCtrl {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<SetupCtrl> Ptr;
protected:
	//-------------------------------------------------------------------------
	SetupCtrl(){}
private:
 	//-------------------------------------------------------------------------
	::frx::processing::IHostInfo::Ptr hostInfo;
	//-------------------------------------------------------------------------
	FrxCircuidViewPtr view;
public:
	//-------------------------------------------------------------------------
	static Ptr create() {
		return Ptr(new SetupCtrl());
	}
	//-------------------------------------------------------------------------
	::frx::processing::IHostInfo::Ptr getHostInfo() const {
		return hostInfo;
	}
	//-------------------------------------------------------------------------
	/**
	 * initiates editor resize request.
	 * @param the new size
	 * @note dosen't save permanent in config.
	 */
	void setEditorSize(const sambag::disco::Dimension &size);
	//-------------------------------------------------------------------------
	sambag::disco::Dimension getEditorSize() const;
	//-------------------------------------------------------------------------
	void setHostInfo(::frx::processing::IHostInfo::Ptr hI);
	//-------------------------------------------------------------------------
	void setView(FrxCircuidViewPtr view);
	//-------------------------------------------------------------------------
	enum FileStatus{OnOpening, Succeed, Failed, Skipped};
	//-------------------------------------------------------------------------
	typedef boost::function<void(const std::string&, FileStatus)> NotifyFileFunc;
	typedef boost::function<void(int, int, int)> ScanCompletedFunc;
	typedef boost::function<void(std::string)> ScanFailedFunc;
	//-------------------------------------------------------------------------
	/**
	 * starts plugin scan (in a seperate thread so don't forget joinScan())
	 * @param scanning event callback function.
	 * @param scanning event callback function.
	 * @see joinScan()
	 */
	void startScan( const NotifyFileFunc &fileEventF, 
		const ScanCompletedFunc &scanCompletedF, const ScanFailedFunc &failed);
	//-------------------------------------------------------------------------
	bool isAllScanned() const;
	//-------------------------------------------------------------------------
	void stopScanning();
	//-------------------------------------------------------------------------
	/**
	 * joins scanning thread
	 * @see boost::thread::join()
	 */
	void joinScan();
	//-------------------------------------------------------------------------
	std::string selectDirectory(const std::string &startDir, 
		sdc::WindowPtr parent) const;
	//-------------------------------------------------------------------------
	bool addPluginFolder(const std::string &path);
	//-------------------------------------------------------------------------
	bool removePluginFolder(const std::string &path);
	//-------------------------------------------------------------------------
	size_t getNumPluginFolder() const;
	//-------------------------------------------------------------------------
	void getPluginFolders(std::list<std::string> &out);
	//-------------------------------------------------------------------------
	void saveSettings();
	//-------------------------------------------------------------------------
	bool getBooleanValue(const std::string &key) const;
	//-------------------------------------------------------------------------
	void setBooleanValue(const std::string &key, bool val);
	//-------------------------------------------------------------------------
	std::string getStringValue(const std::string &key) const;
	//-------------------------------------------------------------------------
	void setStringValue(const std::string &key, const std::string &val);
	//-------------------------------------------------------------------------
	int getIntegerValue(const std::string &key) const;
	//-------------------------------------------------------------------------
	void setIntegerValue(const std::string &key, int val);
}; // SetupCtrl
}}} // namespace(s)

#endif /* SAMBAG_SETUPCTRL_H */
