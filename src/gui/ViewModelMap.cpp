/*
 * ViewModelMap.cpp
 *
 *  Created on: Wed Oct 10 12:06:10 2012
 *      Author: Johannes Unger
 */

#include "ViewModelMap.hpp"

namespace frx { namespace gui {
//=============================================================================
//  Class ViewModelMap
//=============================================================================
//-----------------------------------------------------------------------------
ViewModelMap::Ptr ViewModelMap::create() {
	return Ptr(new ViewModelMap());
}
//-----------------------------------------------------------------------------
ViewModelMap::ViewModelMap() : closed(false) {
}
//-----------------------------------------------------------------------------
std::string ViewModelMap::toString() const {
	std::stringstream ss;
	if (isLocked()) {
		ss<<"locked"<<std::endl;
		return ss.str();
	}
	BOOST_FOREACH(const Map::left_map::value_type &v, map.left) {
		ss<<typeid(*(v.first.get())).name()<<" : "<<typeid(*(v.second.get())).name()<<std::endl;
	}
	return ss.str();
}
//-----------------------------------------------------------------------------
void ViewModelMap::checkState() {
	if (!isLocked())
		return;
	SAMBAG_THROW(
		sambag::com::exceptions::IllegalStateException,
		"tried to acces ViewModelMap in closed state."
	);
}
//-----------------------------------------------------------------------------
processing::ModelObject::Ptr 
ViewModelMap::getModelObject(ViewObject::Ptr obj) 
{
	checkState();
	Map::left_map::iterator it = map.left.find(obj);
	if (it==map.left.end())
		return processing::ModelObject::Ptr();
	return it->second;
}
//-----------------------------------------------------------------------------
ViewObject::Ptr 
ViewModelMap::getViewObject(frx::processing::ModelObject::Ptr obj)
{
	checkState();
	Map::right_map::iterator it = map.right.find(obj);
	if (it==map.right.end())
		return ViewObject::Ptr();
	return it->second;
}
//-----------------------------------------------------------------------------
void ViewModelMap::registerObjects(ViewObject::Ptr vobj,
	frx::processing::ModelObject::Ptr mobj)
{
	checkState();
	if (!vobj || !mobj) {
		SAMBAG_THROW(
			sambag::com::exceptions::IllegalStateException,
			"tried to register NULL in ViewModelMap."
		);
	}
	map.insert(Map::value_type(vobj, mobj));
}
//-----------------------------------------------------------------------------
void ViewModelMap::remove(ViewObject::Ptr vobj,
	frx::processing::ModelObject::Ptr mobj)
{
	checkState();
	Map::left_map::iterator it = map.left.find(vobj);
	if (it==map.left.end())
		return;
	if (it->second == mobj) {
		map.left.erase(it);
	}
}
//-----------------------------------------------------------------------------
bool ViewModelMap::isLocked() const {
	return closed;
}
//-----------------------------------------------------------------------------
size_t ViewModelMap::getSize() const {
	if (isLocked())
		return bedroom.size();
	return map.size();
}
}} // namespace(s)
