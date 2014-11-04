//
//  Utils
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "Utils.h"
#include <cocos2d.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#   include <mach/mach.h>
#endif


void report_memory () {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                   TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    if( kerr == KERN_SUCCESS ) {
        CCLOG("Memory in use %u : %uKB : %uMB", info.resident_size, info.resident_size / 1024, info.resident_size / 1024 / 1024);
        CCLOG("VM in use %u : %uKB : %uMB", info.virtual_size, info.virtual_size / 1024, info.virtual_size / 1024 / 1024);
    } else {
        CCLOG("Error with task_info(): %s", mach_error_string(kerr));
    }
#endif
}
