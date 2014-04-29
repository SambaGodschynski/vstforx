/*
 * IPlugin.hpp
 *
 *  Created on: Fri Mar  7 20:22:39 2014
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_IPLUGIN_H
#define SAMBAG_IPLUGIN_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <string>
#include <processing/pluginTypes/PluginImpl.hpp>


namespace processing { namespace parameter {
    class Parameter;
    typedef boost::shared_ptr<Parameter> ParameterPtr;
}}

namespace frx { namespace processing {
//=============================================================================
/** 
  * @class IPlugin.
  */
class IPlugin {
//=============================================================================
public:
	//-------------------------------------------------------------------------
	typedef boost::shared_ptr<IPlugin> Ptr;
	//-------------------------------------------------------------------------
	typedef boost::weak_ptr<IPlugin> WPtr;
public:
	//-------------------------------------------------------------------------
	/**
	 * @return true if syntheziser
	 */
	virtual bool isSynth() const = 0;
    //-------------------------------------------------------------------------
	/**
	 * @return true if has editor
	 */
	virtual bool hasEditor() const = 0;
	//-------------------------------------------------------------------------
	virtual void openEditor(sambag::disco::components::WindowPtr win) = 0;
	//-------------------------------------------------------------------------
	virtual void closeEditor(sambag::disco::components::WindowPtr win) = 0;
	//----- -------------------------------------------------------------------
	virtual void onEditorIdle() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-X Parameter
	 */
	virtual ::processing::parameter::ParameterPtr getEditorPosX() const = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return Editor-Pos-Y Parameter
	 */
	virtual ::processing::parameter::ParameterPtr getEditorPosY() const = 0;
    //-------------------------------------------------------------------------
	/**
	 * @return number of programs
	 */
	virtual size_t getNumPrograms() = 0;
	//-------------------------------------------------------------------------
	/**
	 * @param index
	 * @return program on index
	 */
	virtual std::string getProgramName( size_t index ) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @brief set program on index
	 * @param index
	 */
	virtual void setProgram( size_t index ) = 0;
	//-------------------------------------------------------------------------
	/**
	 * @return current program index, -1 if none
	 */
	virtual int getProgram() = 0;
    //-------------------------------------------------------------------------
    virtual APluginImpl::Ptr getPluginImpl() const = 0;
    //-------------------------------------------------------------------------
    virtual std::string getPlugName() const = 0;
}; // IPlugin
}} // namespace(s)

#endif /* SAMBAG_IPLUGIN_H */
