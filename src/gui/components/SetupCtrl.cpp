/*
 * SetupCtrl.cpp
 *
 *  Created on: Thu Oct 25 13:56:01 2012
 *      Author: Johannes Unger
 */

#include "SetupCtrl.hpp"
#include <com/Settings.h>
#include <boost/foreach.hpp>
#include <com/PluginCollection.h>
#include <com/PPIError.h>
#include <sambag/com/exceptions/IllegalStateException.hpp>
#include <boost/thread.hpp>
#include <gui/components/FrxCircuidView.hpp>
#include <sambag/disco/components/Window.hpp>
#include <com/one4All.h>
#include <sambag/com/Thread.hpp>

namespace com {
extern std::string osSelectDirectory(const std::string &wndTitle, 
						    const std::string &startPath,
							void *parentWindow);
} // namespace com

namespace frx { namespace gui { namespace components {
//=============================================================================
//  Class SetupCtrl
//=============================================================================
//-----------------------------------------------------------------------------
void SetupCtrl::setHostInfo(::frx::processing::IHostInfo::Ptr hI) {
	hostInfo = hI;
}
//-----------------------------------------------------------------------------
std::string SetupCtrl::selectDirectory(const std::string &startDir,
	sdc::Window::Ptr parent) const 
{
	// get main view window
	while (true && parent) {
		sdc::Window::Ptr p = parent->getParentWindow();
		if (!p) {
			break;
		}
		parent = p;
	}
	void *p = ((bool)parent) ? parent->getWindowImpl()->getSystemHandle() : NULL; 
	return ::com::osSelectDirectory("select directory", startDir, p);
}
//-----------------------------------------------------------------------------
bool SetupCtrl::addPluginFolder(const std::string &path) {
	try {
		return ::com::getSettings().addPluginFolder(path);
	} catch (const ::com::ppiError::PPIError &ex) {
		::com::osMessageBox ( 
			"Error", std::string("adding folder failed: ") + ex.error, ::com::MSG_ALERT 
		);
		return false;
	} catch (const std::exception &ex) {
		::com::osMessageBox ( 
			"Error", std::string("adding folder failed: ") + ex.what(), ::com::MSG_ALERT
		);
		return false;
	}
	return false;
}
//-----------------------------------------------------------------------------
void SetupCtrl::setView(FrxCircuidViewPtr view) {
	this->view = view;
}
//-----------------------------------------------------------------------------
bool SetupCtrl::removePluginFolder(const std::string &path) {
	return ::com::getSettings().removePluginFolder(path);
}
//-----------------------------------------------------------------------------
size_t SetupCtrl::getNumPluginFolder() const {
	return ::com::getSettings().getPluginDirectoryList().size();
}
//-----------------------------------------------------------------------------
void SetupCtrl::getPluginFolders(std::list<std::string> &out) {
	BOOST_FOREACH(const std::string &str, 
		::com::getSettings().getPluginDirectoryList())
	{
		out.push_back(str);
	}
}
//-----------------------------------------------------------------------------
void SetupCtrl::saveSettings() {
	::com::getSettings().saveConfigFile();
}
//-----------------------------------------------------------------------------
namespace {
boost::thread scanThread;
sambag::com::Mutex mutex;
typedef SetupCtrl::NotifyFileFunc FileEvF;
typedef SetupCtrl::ScanCompletedFunc ScanComplF;
typedef SetupCtrl::ScanFailedFunc ScanFailedFunc;
void onLoadFile(void *src, const ::com::OnLoadFile &ev, const FileEvF &f) 
{
	std::cout<<ev.filename<<"...";
	f(ev.filename, SetupCtrl::OnOpening);
}
void onFileLoaded(void *src, const ::com::OnFileLoaded &ev, const FileEvF &f) 
{
	std::cout<<(ev.info.access == ::processing::PluginInfo::SUCCEED
		? "suceed" : "failed") <<std::endl;
	SetupCtrl::FileStatus s;
	switch (ev.info.access) {
		case ::processing::PluginInfo::SUCCEED:
			s = SetupCtrl::Succeed; break;
		case ::processing::PluginInfo::FAILED:
			s = SetupCtrl::Failed; break;
		case ::processing::PluginInfo::NOT_CHECKED:
			s = SetupCtrl::Skipped; break;
	}
	f(ev.filename, s);
}
void startScanImpl(const FileEvF &f, 
	const ScanComplF &sccF, 
	const ScanFailedFunc &failed,
	frx::processing::IHostInfo::Ptr hostInfo) 
{
	typedef ::com::OnLoadFile OnFileLoading;
	typedef ::com::OnFileLoaded OnFileLoaded;
	typedef ::com::PluginCollection::OnLoadFileSender LoadingEvSender;
	typedef ::com::PluginCollection::OnFileLoadedSender LoadedEvSender;
	typedef LoadingEvSender::EventConnection LoadEvConnection;
	typedef LoadedEvSender::EventConnection LoadedEvConnection;
	
    boost::unique_lock<boost::timed_mutex> lock( mutex, boost::try_to_lock);
	if (!lock.owns_lock()) {
        return;
    }
	::com::PluginCollection::Ptr db;
	try {
		 db = ::com::getPluginCollection();
	} catch (...) {
		failed("database access failed.");
		return;
	}
	LoadEvConnection loadCn = db->LoadingEvSender::
		addEventListener(boost::bind(&onLoadFile, _1, _2, f));
	LoadedEvConnection loadedCn = db->LoadedEvSender::
		addEventListener(boost::bind(&onFileLoaded, _1, _2, f));
	try {
		// scan
		db->update(hostInfo);
	} catch (...) {
		failed("database update failed.");
		return;
	}
	// disconnect
	loadCn.disconnect();
	loadedCn.disconnect();
	sccF(db->getNumSucceed(), db->getNumFailed(), db->getNumNotChecked());
}
} // namespace(s)
//-----------------------------------------------------------------------------
void SetupCtrl::startScan(const NotifyFileFunc &fileEventF, 
	const ScanCompletedFunc &scanCompletedF, const ScanFailedFunc &failed)
{
	if (!hostInfo) {
		SAMBAG_THROW(sambag::com::exceptions::IllegalStateException, 
			"tried to start scan with hostInfo == NULL");
	}
	scanThread = boost::thread(
		boost::bind(&startScanImpl, fileEventF, scanCompletedF, failed, hostInfo)	
	);
}
//-----------------------------------------------------------------------------
bool SetupCtrl::isAllScanned() const {
	try {
		::com::PluginCollection::Ptr db = ::com::getPluginCollection();
		return db->isAllScanned();
	} catch (...) {
		return false;
	}
}
//-----------------------------------------------------------------------------
void SetupCtrl::joinScan() {
	scanThread.join();
}
//-----------------------------------------------------------------------------
void SetupCtrl::stopScanning() {
	try {
		::com::PluginCollection::Ptr db = ::com::getPluginCollection();
		db->stopScanning();
	} catch(...) {
		// what should I do? ...
		return; // ... die
	}
}
//-----------------------------------------------------------------------------
bool SetupCtrl::getBooleanValue(const std::string &key) const {
	return ::com::getSettings().getBooleanValue(key);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setBooleanValue(const std::string &key, bool val) {
	return ::com::getSettings().setBooleanValue(key, val);
}
//-----------------------------------------------------------------------------
std::string SetupCtrl::getStringValue(const std::string &key) const {
	return ::com::getSettings().getStringValue(key);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setStringValue(const std::string &key, const std::string &val){
	return ::com::getSettings().setStringValue(key, val);
}
//-----------------------------------------------------------------------------
int SetupCtrl::getIntegerValue(const std::string &key) const {
	return ::com::getSettings().getIntegerValue(key);
}
//-----------------------------------------------------------------------------
void SetupCtrl::setIntegerValue(const std::string &key, int val) {
	return ::com::getSettings().setIntegerValue(key, val);
}
//-----------------------------------------------------------------------------
sambag::disco::Dimension SetupCtrl::getEditorSize() const {
	using namespace sambag::disco::components;
	if (!view)
		return NULL_DIMENSION;
	Window::Ptr win = view->getFirstContainer<Window>();
	if (!win)
		return NULL_DIMENSION;
	return win->getWindowSize();
}
//-----------------------------------------------------------------------------
void SetupCtrl::setEditorSize(const sambag::disco::Dimension &size)
{
	view->requestEditorResize(size);
}
}}} // namespace(s)
