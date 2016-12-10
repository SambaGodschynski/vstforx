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
#include <boost/lexical_cast.hpp>
#include <ostream>

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
	enum PluginType { UNKNOWN, VST2X, VST3X, DX, AU, LUA };
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
		ar & uid_legacy;
        
        if (version>=1) {
            ar & id;
        }
        if (version>=2) {
            ar & vendor;
        }
        if (version>=3) {
            ar & uid;
        } else {
            uid = boost::lexical_cast<std::string>(uid_legacy);
        }
	}
	//--------------------------------------------------------------------------------------------------------
public:
	std::string location;
	std::string name;
    std::string vendor;
    std::string uid;
	PluginType pluginType;
	int isSynth;
	int uid_legacy;
	time_t timestamp;
	AccessState access; // konnte geladen werden?
    int id; // database id
    //--------------------------------------------------------------------------------------------------------
    std::string getFactoryId() const {
        ::com::IdParser res;
        res.namespace_("processing").name("Plugin").details(location);
        switch (pluginType) {
            case PluginInfo::VST2X : return res.type("vst2x").toString();
            case PluginInfo::VST3X : return res.type("vst3x").toString();
            case PluginInfo::DX    : return res.type("dx").toString();
            case PluginInfo::AU    : return res.type("au").toString();
            case PluginInfo::LUA   : return res.type("lua").toString();
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
        uid_legacy(0),
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

inline std::ostream & operator<<(std::ostream &os, const processing::PluginInfo &inf) {
	os << inf.getFactoryId() << std::endl;
	os << inf.location << std::endl;
	os << inf.name << std::endl;
	os << inf.pluginType << std::endl;
	os << inf.isSynth << std::endl;
	os << inf.timestamp << std::endl;
	os << inf.access << std::endl;
	os << inf.uid_legacy << std::endl;
	os << inf.id << std::endl;
	os << inf.vendor << std::endl;
	os << inf.uid << std::endl;
	return os;
}

BOOST_CLASS_VERSION(processing::PluginInfo, 3)

#endif
