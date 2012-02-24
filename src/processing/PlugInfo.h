/*
 * ===========================================================================================================
 * Pluginfo.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef PLUGIN_INFO_H
#define PLUGIN_INFO_H

#include "com/Serialization.h"
#include <string>

namespace processing {
using namespace std;
//============================================================================================================
/**
 * @class PluginInfo
 * Database / Plugin Transferobjekt
 */
//============================================================================================================
struct PluginInfo {
friend class boost::serialization::access;
public:
	//--------------------------------------------------------------------------------------------------------
	enum PluginType { UNKNOWN, VST2X, VST3X, DX, AU };
	//--------------------------------------------------------------------------------------------------------
	enum AccessState { NOT_CHECKED, SUCCEED, FAILED };
private:
	//--------------------------------------------------------------------------------------------------------
	/**
	 * (De)Serialisiert PluginInfo-Objekt
	 * @param ar boost::Archive-Objekt
	 * @param version
	 */
	template < typename Archive >
	void serialize ( Archive &ar, const unsigned int version) {
		ar & location;
		ar & name;
		ar & pluginType;
		ar & isSynth;
		ar & timestamp;
		ar & access;
		ar & uid;
	}
	//--------------------------------------------------------------------------------------------------------
public:
	string location;
	string name;
	PluginType pluginType;
	int isSynth;
	int uid;
	time_t timestamp;
	AccessState access; // konnte geladen werden?
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn valides PluginInfo-Objekt
	 */
	bool isValid() const {
		return location != "";
	}
	//--------------------------------------------------------------------------------------------------------
	PluginInfo() : 
	access (NOT_CHECKED), isSynth(0), pluginType(UNKNOWN), uid(0), timestamp(0) {}
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return String-Repraesentation
	 */
	string toString() const;
	//--------------------------------------------------------------------------------------------------------
	/**
	 *
	 * @param t unix-timestamp
	 * @return true, wenn uebergebenes timestamp ungleich PluginInfo::timestamp
	 */
	bool hasChanged( const time_t &t ) { 
		return t != timestamp;
	} 
	//---------------------------------------------------------------------------------------------------------
	bool operator == ( const PluginInfo &pi ) const {
		return location == pi.location;
	}
	//--------------------------------------------------------------------------------------------------------
	bool operator != ( const PluginInfo &pi ) const {
		return !(*this == pi);
	}
};
} // namespace

#endif
