/*
 * ===========================================================================================================
 * ScriptCollection.h
 *      Author: Johannes Unger
 * ===========================================================================================================
 */
#ifndef FORX_SCRIPTCOLLECTION_H
#define FORX_SCRIPTCOLLECTION_H

#include "processing/processing.h"
#include "processing/parameter/parameter.h"
#include "com/Serialization.h"
#include <sambag/com/FileSystem.hpp>
#include "ScriptInfo.h"

namespace com {
//============================================================================================================
/**
 * @class ScriptCollection.
 */
class ScriptCollection {
//============================================================================================================
public:
	//--------------------------------------------------------------------------------------------------------
	template <typename Container>
	static void getScriptsInDirectory(const std::string &root, Container &out); 
};
//============================================================================================================
// template impl.
//============================================================================================================
namespace {
template <typename Container>
struct ScriptWalkerVisitor : public sambag::com::IWalkerVisitor {
	Container &c;
	virtual bool changeDirectory ( const sambag::com::Location & path ) {
		return true;
	}
	virtual void file ( const sambag::com::Location & file ) {
		if (file.extension() == ".lua") {
			ScriptInfo sc(file.string());
			sc.name = file.stem().string();
			c.push_back(sc);
		}
	}
	ScriptWalkerVisitor(Container &c) : c(c) {}
};
}
//------------------------------------------------------------------------------------------------------------
template <typename Container>
void ScriptCollection::getScriptsInDirectory(const std::string &root, Container &out) {
	using namespace sambag::com;
	ScriptWalkerVisitor<Container> vis(out);
	dirWalker(root, vis);
}

}// namespace com

#endif  // FORX_SCRIPTCOLLECTION_H


