/*
 * CocoaHelper.cpp
 *
 *  Created on: Thu Apr 25 13:33:51 2013
 *      Author: Johannes Unger
 */
#include "CocoaImpl.h"
#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>
#import <objc/message.h>
#import <com/Settings.h>

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
// class CocoaImpl.
//=============================================================================
//-----------------------------------------------------------------------------
std::string CocoaImpl::selectDirectory(const std::string &wndTitle,
                                         const std::string &startPath)
{
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    // Configure your panel the way you want it
    [panel setCanChooseFiles:NO];
    [panel setCanChooseDirectories:YES];
    [panel setAllowsMultipleSelection:NO];
    [panel setLevel: NSFloatingWindowLevel];
    [panel setTitle:[NSString stringWithUTF8String:wndTitle.c_str()]];
    NSURL *_startPath = [NSURL
                        fileURLWithPath:[NSString stringWithUTF8String:startPath.c_str()]
                         isDirectory: YES];
    [panel setDirectoryURL:_startPath];
    [panel runModal];
    NSArray *res = [panel URLs];
    if ([res count] > 0) {
        NSURL *url = [res objectAtIndex:0];
        if (url) {
            return toString([url path]);
        }
    }
    return "";
}
//-----------------------------------------------------------------------------
void CocoaImpl::startProcess(const char *path, int argc, const char **argv) {
    NSTask *task = [[NSTask alloc] init];
    [task setLaunchPath: [NSString stringWithUTF8String:path]];
    
    if (argc > 0) {
        NSMutableArray *args = [[NSMutableArray alloc] initWithCapacity: argc];
        for (int i=0; i<argc; ++i) {
            [args addObject: [NSString stringWithUTF8String:argv[i]]];
        }
        [task setArguments: args];
    }
    [task launch];
}
}}