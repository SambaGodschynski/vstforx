/*
 * Forward.hpp
 *
 *  Created on: Thu Sep 27 14:49:27 2012
 *      Author: Johannes Unger
 */

#ifndef SAMBAG_FRXFORWARD_H
#define SAMBAG_FRXFORWARD_H


#include <memory>

namespace frx { namespace gui { namespace components {
	class FrxComponent;
	typedef std::shared_ptr<FrxComponent> FrxComponentPtr; 
	typedef std::weak_ptr<FrxComponent> FrxComponentWPtr; 
	class FrxCircuidView;
	typedef std::shared_ptr<FrxCircuidView> FrxCircuidViewPtr; 
	typedef std::weak_ptr<FrxCircuidView> FrxCircuidViewWPtr; 
	class FrxSelection;
	typedef std::shared_ptr<FrxSelection> FrxSelectionPtr; 
	typedef std::weak_ptr<FrxSelection> FrxSelectionWPtr; 
	class FrxNode;
	typedef std::shared_ptr<FrxNode> FrxNodePtr; 
	typedef std::weak_ptr<FrxNode> FrxNodeWPtr; 
	class FrxColumnBrowser;
	typedef std::shared_ptr<FrxColumnBrowser> FrxColumnBrowserPtr; 
	typedef std::weak_ptr<FrxColumnBrowser> FrxColumnBrowserWPtr; 
	class FrxProcessorNode;
	typedef std::shared_ptr<FrxProcessorNode> FrxProcessorNodePtr; 
	typedef std::weak_ptr<FrxProcessorNode> FrxProcessorNodeWPtr;
	class FrxParameter;
	typedef std::shared_ptr<FrxParameter> FrxParameterPtr; 
	typedef std::weak_ptr<FrxParameter> FrxParameterWPtr;
	class FrxProcessorEditor;
	typedef std::shared_ptr<FrxProcessorEditor> FrxProcessorEditorPtr; 
	typedef std::weak_ptr<FrxProcessorEditor> FrxProcessorEditorWPtr;
}}}

#endif