/*
 * VstForxResourceManager.hpp
 *
 *  Created on: Fri Jan 18 10:28:40 2013
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_VSTFORXRESOURCEMANAGER_H
#define SAMBAG_VSTFORXRESOURCEMANAGER_H

#include <loki/Singleton.h>
#include <sambag/disco/FileResourceManager.hpp>
#include <sambag/disco/Win32IntResourceManager.hpp>
#include <gui/HandyNamespaces.hpp>
namespace frx {
//=============================================================================
/** 
  * @class VstForxResourceManager.
  * Adapter with FileResourceManager as adptor and Win32IntResourceManager
  * as adaptee.
  */
class VstForxResourceManager : public sd::FileResourceManager {
//=============================================================================
friend struct Loki::CreateUsingNew<VstForxResourceManager>;
public:
protected:
	//-------------------------------------------------------------------------
	VstForxResourceManager();
private:
	//-------------------------------------------------------------------------
	sd::Win32IntResourceManager *adaptee;
public:
	//-------------------------------------------------------------------------
	void initMap();
	//-------------------------------------------------------------------------
	sd::Win32IntResourceManager * getAdaptee() const {
		return adaptee;
	}
	//-------------------------------------------------------------------------
	static VstForxResourceManager & instance();
	//-------------------------------------------------------------------------
	/**
	 * @override
	 * load image from internal resource using Win32IntResourceManager
	 * as path to resource id map.
	 */
	virtual ImagePtr loadImage(const std::string &path);
}; // VstForxResourceManager
} // namespace(s)

#endif /* SAMBAG_VSTFORXRESOURCEMANAGER_H */
