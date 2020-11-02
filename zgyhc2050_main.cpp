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


#include <unistd.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <cutils/log.h>
#include <string.h>
#include <media/AudioTrack.h>
#include "trackConfig.h"
#include "aaudioConfig.h"

#define LOG_TAG "zgyhc_main"
//#define LOG_NDEBUG 0

using namespace android;

int main(int argc, char** argv) {
    char                *pFilePath = NULL;
    int                 flag = 0;
    audio_stream_type_t enStreamType = AUDIO_STREAM_MUSIC;
    bool                bAaudio = false;

    if (argc == 1) {
        printf("[%s:%d] printf Too few parameters:%d, need 4\n", __func__, __LINE__, argc);
        return -1;
    } else if (argc == 2) {
        if (!memcmp(argv[1], "-h", 2) || !(memcmp(argv[1], "--help", 2))) {
            printf("argv[1]: file name\n");
            printf("argv[2]: stream flag:\n");
            printf("\t0: normal lpcm\n");
            printf("\t1: hwsync lpcm\n");
            printf("argv[3]: stream type:\n");
            return 0;
        } else {
            ALOGI("[%s:%d] No flag and stream type is specified, now set default pcm, MUSIC type.", __func__, __LINE__);
        }
    } else if (argc == 3) {
        if (!memcmp(argv[2], "aaudio", 4)) {
            bAaudio = true;
        } else {
            flag = atoi(argv[2]);
            if (flag != 0 && flag != 1) {
                ALOGW("[%s:%d] Invalid flag parameters:%d, now set default pcm type.", __func__, __LINE__, flag);
                flag = 0;
            }
            ALOGI("[%s:%d] No stream type specified, now set default MUSIC type.", __func__, __LINE__);
        }
    } else if (argc == 4) {
        flag = atoi(argv[2]);
        if (flag != 0 && flag != 1) {
            ALOGW("[%s:%d] Invalid flag parameters:%d, now set default pcm type.", __func__, __LINE__, flag);
            flag = 0;
        }
        enStreamType = (audio_stream_type_t)atoi(argv[3]);
        if (enStreamType <= AUDIO_STREAM_DEFAULT || enStreamType > AUDIO_STREAM_PATCH) {
            ALOGW("[%s:%d] Invalid stream type parameters:%d, now set default MUSIC type.", __func__, __LINE__, enStreamType);
            enStreamType = AUDIO_STREAM_MUSIC;
        }
    } else {
        printf("[%s:%d] printf Too few parameters:%d, need 4\n", __func__, __LINE__, argc);
        return -1;
    }

    sp < ProcessState > proc(ProcessState::self());
    sp < IServiceManager > sm = defaultServiceManager();
    pFilePath = argv[1];
    if (bAaudio) {
        printf("[%s:%d] AAudio case.\n", __func__, __LINE__);
        ALOGI("[%s:%d] AAudio case.", __func__, __LINE__);
        initAAudioConfig(pFilePath);
    } else {
        new_android_audiotrack(pFilePath, flag, enStreamType);
    }
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
