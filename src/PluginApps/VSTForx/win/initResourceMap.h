/*
 * initResourceMap.h
 *
 *  Created on: Mon Feb 24 11:26:52 2014
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
