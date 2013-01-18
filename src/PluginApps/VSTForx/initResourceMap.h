/*
 * initResourceMap.h
 *
 *  Created on: Fri Jan 18 21:00:49 2013
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
