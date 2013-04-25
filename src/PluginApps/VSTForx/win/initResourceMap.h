/*
 * initResourceMap.h
 *
 *  Created on: Fri Mar 01 13:09:32 2013
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
