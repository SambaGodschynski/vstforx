/*
 * initResourceMap.h
 *
 *  Created on: Wed Aug 28 22:13:37 2013
 *      Author: createResources.py
 * 
 * !! CREATED AUTOMATICALLY DO NOT CHANGE !!
 */


namespace frx { namespace createResourcesPy {
/**
 * RegisterCallbackFunction
 */
typedef void (*RegisterResourceFunc) (const char*, long id);

/**
 * maps filename -> resourceID
 */
void initResourceMap(RegisterResourceFunc registerF);

}} // namespace
