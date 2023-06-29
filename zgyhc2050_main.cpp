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
#include <unistd.h>
#include <getopt.h>
#include <media/AudioTrack.h>
#include "trackConfig.h"
#include "aaudioConfig.h"

#define LOG_TAG "zgyhc_main"
//#define LOG_NDEBUG 0

using namespace android;

static void usage(const char* pname)
{
    fprintf(stderr, "Usage: %s [-p file-path] [-t stream-type] [-c channel-number] [-f flags] [-m]\n", pname);
    fprintf(stderr, R"init(
General options:
  -p, --file_path=<path>            Set the file path.
  -t, --stream_type=<type>          Set the stream type.
  -c, --channel_num=<number>        Set the channel number.
  -f, --stream_flags=<flags>        Set the flags.
  -r, --sample_rate=<rate>          Set the sample rate.
  -m, --mmap_audio=<aaudio>         Set the AAudio enable. (Dispensable param, default true. 1:true 0:false)
  -H, --help_param=<param>          Query help infomation for each param. (The parameter character above. eg: -H t.)
)init");
}

static void helpInfo(const char* param)
{
    switch (*param) {
    case 'p':
        fprintf(stderr,
            "eg: -p /data/a.wav\n"
            "the file path. (Must be a PCM file)\n");
        break;
    case 't':
        fprintf(stderr,
            "eg: -t 3\n"
            "audio_stream_type_t enum\n"
            "AUDIO_STREAM_VOICE_CALL        = 0\n"
            "AUDIO_STREAM_SYSTEM            = 1\n"
            "AUDIO_STREAM_RING              = 2\n"
            "AUDIO_STREAM_MUSIC             = 3\n"
            "AUDIO_STREAM_ALARM             = 4\n"
            "AUDIO_STREAM_NOTIFICATION      = 5\n"
            "AUDIO_STREAM_BLUETOOTH_SCO     = 6\n"
            "AUDIO_STREAM_ENFORCED_AUDIBLE  = 7\n"
            "AUDIO_STREAM_DTMF              = 8\n"
            "AUDIO_STREAM_TTS               = 9\n"
            "AUDIO_STREAM_ACCESSIBILITY     = 10\n"
            "AUDIO_STREAM_REROUTING         = 11\n"
            "AUDIO_STREAM_PATCH             = 12\n");
        break;
    case 'c':
        fprintf(stderr,
            "eg: -c 6\n"
            "the audio channel number.\n");
        break;
    case 's':
        fprintf(stderr,
            "eg: -s 44100\n"
            "the audio sample rate.\n");
        break;
    case 'f':
        fprintf(stderr,
            "eg: -f 0x441\n"
            "audio_output_flags_t enum. (Enumeration values can be merged)\n"
            "AUDIO_OUTPUT_FLAG_NONE             = 0x0\n"
            "AUDIO_OUTPUT_FLAG_DIRECT           = 0x1\n"
            "AUDIO_OUTPUT_FLAG_PRIMARY          = 0x2\n"
            "AUDIO_OUTPUT_FLAG_FAST             = 0x4\n"
            "AUDIO_OUTPUT_FLAG_DEEP_BUFFER      = 0x8\n"
            "AUDIO_OUTPUT_FLAG_COMPRESS_OFFLOAD = 0x10\n"
            "AUDIO_OUTPUT_FLAG_NON_BLOCKING     = 0x20\n"
            "AUDIO_OUTPUT_FLAG_HW_AV_SYNC       = 0x40\n"
            "AUDIO_OUTPUT_FLAG_TTS              = 0x80\n"
            "AUDIO_OUTPUT_FLAG_RAW              = 0x100\n"
            "AUDIO_OUTPUT_FLAG_SYNC             = 0x200\n"
            "AUDIO_OUTPUT_FLAG_IEC958_NONAUDIO  = 0x400\n"
            "AUDIO_OUTPUT_FLAG_DIRECT_PCM       = 0x2000\n"
            "AUDIO_OUTPUT_FLAG_MMAP_NOIRQ       = 0x4000\n"
            "AUDIO_OUTPUT_FLAG_VOIP_RX          = 0x8000\n"
            "AUDIO_OUTPUT_FLAG_INCALL_MUSIC     = 0x10000\n");
        break;
    case 'm':
        fprintf(stderr,
            "eg: -m\n"
            "the aaudio enable.\n");
        break;
    default:
        fprintf(stderr, "not supported param:%c.\n", *param);
        break;
    }
    return;
}

static struct option long_options[] = {
    {"file_path",              required_argument,  NULL, 'p'},
    {"stream_type",            required_argument,  NULL, 't'},
    {"channel_num",            required_argument,  NULL, 'c'},
    {"stream_flags",           required_argument,  NULL, 'f'},
    {"sample_rate",            required_argument,  NULL, 'r'},
    {"mmap_audio",             optional_argument,  NULL, 'm'},
//        {"settings",               no_argument,        NULL, 's'},
    {"stream_volume",          required_argument,  NULL, 'v'},
    {"help_param",             required_argument,  NULL, 'H'},
//        {"http-user", required_argument, &lopt, 2 },
    {"help",                   no_argument,        NULL, 'h'},
    {0, 0, 0, 0 }
};

int main(int argc, char** argv) {
    zgyhc_audio_info_st     stAudioInfo = {
        .pFilePath = NULL,
        .enFlags = AUDIO_OUTPUT_FLAG_NONE,
        .enStreamType = AUDIO_STREAM_MUSIC,
        .channelNum = 2,
        .channelMask = AUDIO_CHANNEL_OUT_STEREO,
        .sampleRate = 48000,
        .bAaudio = false,
    };

//    bool bSettings = false;
    int opt;
    while ((opt = getopt_long(argc, argv, "p:t:c:f:r:m::hH:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                stAudioInfo.pFilePath = optarg;
                break;
            case 't':
                stAudioInfo.enStreamType = (audio_stream_type_t)atoi(optarg);
                break;
            case 'c':
                stAudioInfo.channelNum = atoi(optarg);
                break;
            case 'f':
                stAudioInfo.enFlags = (audio_output_flags_t)atoi(optarg);
                break;
            case 'r':
                stAudioInfo.sampleRate = atoi(optarg);
                break;
            case 'm':
                if (optarg) {
                    stAudioInfo.bAaudio = (atoi(optarg) == 0) ? false : true;
                } else {
                    stAudioInfo.bAaudio = true;
                }
                break;
            case 'H':
                helpInfo(optarg);
                return 1;
            case '?':
            case 'h':
                usage(argv[0]);
                return 1;
        }
    }
    if (stAudioInfo.enStreamType <= AUDIO_STREAM_DEFAULT || stAudioInfo.enStreamType > AUDIO_STREAM_PATCH) {
        ALOGW("[%s:%d] Invalid stream type parameters:%d, now set default MUSIC type.", __func__, __LINE__, stAudioInfo.enStreamType);
        stAudioInfo.enStreamType = AUDIO_STREAM_MUSIC;
    }

    if (stAudioInfo.channelNum == AUDIO_TRACK_TYPE_CH_STEREO) {
        stAudioInfo.channelMask = AUDIO_CHANNEL_OUT_STEREO;
    } else if (stAudioInfo.channelNum == AUDIO_TRACK_TYPE_CH_5POINT1) {
        stAudioInfo.channelMask = AUDIO_CHANNEL_OUT_5POINT1;
    } else if (stAudioInfo.channelNum == AUDIO_TRACK_TYPE_CH_7POINT1) {
        stAudioInfo.channelMask = AUDIO_CHANNEL_OUT_7POINT1;
    } else {
        ALOGW("[%s:%d] Invalid channel:%d, now set default 2 Channel.", __func__, __LINE__, stAudioInfo.channelNum);
        stAudioInfo.channelNum = 2;
        stAudioInfo.channelMask = AUDIO_CHANNEL_OUT_STEREO;
    }

    printf("[%s:%d] input pFilePath:%s, enStreamType:%d, channel_num:%d, flag:%#x, aaudio:%d\n", __func__, __LINE__,
        stAudioInfo.pFilePath, stAudioInfo.enStreamType, stAudioInfo.channelNum, stAudioInfo.enFlags, stAudioInfo.bAaudio);
    ALOGI("[%s:%d] pFilePath:%s, enStreamType:%d, channel_num:%d, flag:%#x, aaudio:%d", __func__, __LINE__,
        stAudioInfo.pFilePath, stAudioInfo.enStreamType, stAudioInfo.channelNum, stAudioInfo.enFlags, stAudioInfo.bAaudio);
    if (stAudioInfo.pFilePath == NULL) {
        printf("[%s:%d] file is null.\n", __func__, __LINE__);
        return 0;
    }
    sp < ProcessState > proc(ProcessState::self());
    sp < IServiceManager > sm = defaultServiceManager();
    if (stAudioInfo.bAaudio) {
        printf("[%s:%d] AAudio case.\n", __func__, __LINE__);
        ALOGI("[%s:%d] AAudio case.", __func__, __LINE__);
        initAAudioConfig(&stAudioInfo);
    } else {
        initAudioTrackConfig(&stAudioInfo);
    }
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
