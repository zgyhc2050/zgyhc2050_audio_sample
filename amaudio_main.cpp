/*
 * Copyright (C) 2010 Amlogic Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



/*
 This file is used for testing audio fuction.
 */

#include <unistd.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <cutils/log.h>

#include "android_out.h"

using namespace android;

int main(int argc, char** argv) {
    printf("<%s::%d>", __FUNCTION__, __LINE__);
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    char *in_file = NULL;
    int  track_type = 0;
    if (argc <  3) {
        ALOGI("usage: file name ， track type (0,normal lpcm,1,hwsync lpcm\n");
    }    
    sp < ProcessState > proc(ProcessState::self());
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    sp < IServiceManager > sm = defaultServiceManager();
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    in_file = argv[1];
    track_type = atoi(argv[2]);
    new_android_audiotrack(in_file,track_type);
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    return 0;
}
