/*
 * initResourceMap.h
 *
 *  Created on: Thu Jul 11 23:50:10 2013
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
