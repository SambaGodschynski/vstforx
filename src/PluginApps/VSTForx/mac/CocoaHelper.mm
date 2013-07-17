/*
 * CocoaHelper.cpp
 *
 *  Created on: Thu Apr 25 13:33:51 2013
 *      Author: Johannes Unger
 */
#include "CocoaHelper.hpp"
#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import <com/Settings.h>
#include <iostream>

namespace {
	std::string toString(NSString *str) {
		if (!str) {
			return "";
		}
		return std::string([str UTF8String]);
	}
}

namespace frx { namespace com { 
//============================================================================= 
// class CocoaHelper.
//=============================================================================
std::string CocoaHelper::getResourceLocation(const std::string &path) {
	const std::string & idstr = FRX_BNDL_ID;
	NSString *_id = [NSString stringWithUTF8String:idstr.c_str()];
	NSString *_path = [NSString stringWithUTF8String:path.c_str()];
	
	NSBundle* myBundle = [NSBundle bundleWithIdentifier: _id];
	if (!myBundle) {
		return "";
	}
	NSString* res = [myBundle pathForResource:_path ofType:nil];
	return toString(res);
}
//-----------------------------------------------------------------------------
std::string CocoaHelper::getBundleLocation() {
	const std::string & idstr = FRX_BNDL_ID;
	NSString *_id = [NSString stringWithUTF8String:idstr.c_str()];
	NSBundle* myBundle = [NSBundle bundleWithIdentifier: _id];
	if (!myBundle) {
		return "";
	}
	return toString([myBundle bundlePath]);
}
}}