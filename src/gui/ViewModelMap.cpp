/*
 * ViewModelMap.cpp
 *
 *  Created on: Wed Oct 10 12:06:10 2012
 *      Author: Johannes Unger
 */

#include "ViewModelMap.hpp"
#include <com/Serialization.h>

namespace frx { namespace gui {
//=============================================================================
//  Class ViewModelMap
//=============================================================================
//-----------------------------------------------------------------------------
void ViewModelMap::serialize(com::iArchive &ar, const unsigned int version) {
	map.clear();
	ar & boost::serialization::base_object<IViewModelMap> ( *this );
	ar & bedroom;
	ar & closed;
	ar & map;
}
//-----------------------------------------------------------------------------
void ViewModelMap::serialize(com::oArchive &ar, const unsigned int version) {
	ar & boost::serialization::base_object<IViewModelMap> ( *this );
	ar & bedroom;
	ar & closed;
	ar & map;
}
//-----------------------------------------------------------------------------
ViewModelMap::Ptr ViewModelMap::clone() const {
	Ptr neu = create();
	neu->bedroom = bedroom;
	SAMBAG_ASSERT(neu->bedroom.size() == bedroom.size());
	neu->closed = closed;
	neu->map = map;
	SAMBAG_ASSERT(neu->map.size() == map.size());
	return neu;
}
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
	ss<<"size="<<map.size();
	BOOST_FOREACH(const Map::left_map::value_type &v, map.left) {
		ss<<std::endl;
		//ss<<typeid(*(v.first.get())).name()<<"("<<(void*)v.first.get()<<")";
		ss<<v.first->getObjectName()<<"("<<(void*)v.first.get()<<")";
		ss<<" <<==>> ";
		ss<<typeid(*(v.second.get())).name()<<"("<<(void*)v.second.get()<<")";
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
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        checkState();
        Map::left_map::iterator it = map.left.find(obj);
        if (it==map.left.end()) {
            return processing::ModelObject::Ptr();
        }
        return it->second;
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
ViewObject::Ptr 
ViewModelMap::getViewObject(frx::processing::ModelObject::Ptr obj)
{
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        checkState();
        Map::right_map::iterator it = map.right.find(obj);
        if (it==map.right.end()) {
            return ViewObject::Ptr();
        }
        return it->second;
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
bool ViewModelMap::registerObjects(ViewObject::Ptr vobj,
	frx::processing::ModelObject::Ptr mobj)
{
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        checkState();
        if (!vobj || !mobj) {
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                "tried to register NULL in ViewModelMap."
            );
        }
        Map::const_iterator it;
        bool inserted;
        boost::tie(it, inserted) = map.insert(Map::value_type(vobj, mobj));
        return inserted;
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void ViewModelMap::remove(ViewObject::Ptr vobj,
	frx::processing::ModelObject::Ptr mobj)
{
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        checkState();
        Map::left_map::iterator it = map.left.find(vobj);
        if (it==map.left.end())
            return;
        if (it->second == mobj) {
            map.left.erase(it);
        }
    SAMBAG_END_SYNCHRONIZED
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
//-----------------------------------------------------------------------------
void ViewModelMap::getModelObjects(ModelObjects &out) const {
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        if (isLocked()) {
            BOOST_FOREACH(frx::processing::ModelObject::Ptr x, bedroom) {
                out.push_back(x);
            }
            return;
        }
        BOOST_FOREACH(const Map::left_map::value_type &x, map.left) {
            out.push_back(x.second);
        }
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void ViewModelMap::lock(::com::oArchive &ar) {
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        ViewObjects l;
        BOOST_FOREACH(const Map::left_map::value_type &v, map.left) {
            l.push_back(v.first);
            bedroom.push_back(v.second);
        }
        ar & l;
        map.clear();
        closed = true;
    SAMBAG_END_SYNCHRONIZED
}
//-----------------------------------------------------------------------------
void ViewModelMap::unlock(::com::iArchive &ar) {
    SAMBAG_BEGIN_SYNCHRONIZED(mutex)
        if (!isLocked()) {
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                "map not locked."
            );
        }
        ViewObjects l;
        ar>>l;
        if (l.size() != bedroom.size()) {
            SAMBAG_THROW(
                sambag::com::exceptions::IllegalStateException,
                "map unlock failed."
            );
        }
        ViewObjects::const_iterator vit = l.begin();
        ModelBedroom::const_iterator mit = bedroom.begin();
        while(vit!=l.end()) {
            map.insert(Map::value_type(*vit, *mit));
            ++vit;
            ++mit;
        }
        closed = false;
		bedroom.clear();
    SAMBAG_END_SYNCHRONIZED
}
}} // namespace(s)
