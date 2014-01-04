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
#include <com/one4All.h>

namespace processing {
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
        
        if (version>=1) {
            ar & id;
        }
	}
	//--------------------------------------------------------------------------------------------------------
public:
	std::string location;
	std::string name;
	PluginType pluginType;
	int isSynth;
	int uid;
	time_t timestamp;
	AccessState access; // konnte geladen werden?
    int id; // database id
    //--------------------------------------------------------------------------------------------------------
    std::string getFactoryId() {
        ::com::IdParser res;
        res.namespace_("processing").name("Plugin").details(location);
        switch (pluginType) {
            case PluginInfo::VST2X : return res.type("vst2x").toString();
            case PluginInfo::VST3X : return res.type("vst3x").toString();
            case PluginInfo::DX    : return res.type("dx").toString();
            case PluginInfo::AU    : return res.type("au").toString();
            default                : return res.type("unknown-plugin").toString();
        }
        return "";
    }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return true, wenn valides PluginInfo-Objekt
	 */
	bool isValid() const {
		return location != "";
	}
	//--------------------------------------------------------------------------------------------------------
	PluginInfo() : pluginType(UNKNOWN),
        isSynth(0),
        uid(0),
        timestamp(0),
        access (NOT_CHECKED),
        id(-1)
    {
    }
	//--------------------------------------------------------------------------------------------------------
	/**
	 * @return String-Repraesentation
	 */
	std::string toString() const;
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

BOOST_CLASS_VERSION(processing::PluginInfo, 1)

#endif
