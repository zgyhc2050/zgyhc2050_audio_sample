#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <media/AudioTrack.h>

#include "trackConfig.h"
#include "zgyhc_common.h"

using namespace android;

#define LOG_TAG "zgyhc_trackConfig"
//#define LOG_NDEBUG 0

static AudioTrack *glpTracker = NULL;
static sp<AudioTrack> gmpAudioTracker;
//static pthread_mutex_t device_change_lock = PTHREAD_MUTEX_INITIALIZER;
char file_path[PROPERTY_VALUE_MAX] = {0};
long file_length = 0;
int fp_cur_pos = 0;


void trackExitSignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        ALOGD("[%s:%d] ", __func__, __LINE__);
    }
}


static int get_file_path(void *path)
{
    //char system_file_path[PROPERTY_VALUE_MAX];

    //int ret =property_get(FilePath, (char *)path, NULL);
    if (1)
    {
        ALOGI("<%s::%d>--[path:%s]", __FUNCTION__, __LINE__, path);
        FILE *fp = fopen((const char *)path, "r");
        if(fp) {
            fseek(fp,0,SEEK_SET);
            fseek(fp,0,SEEK_END);
            file_length = ftell(fp);
            fseek(fp,0,SEEK_SET);
            fclose(fp);
        }
        fp = NULL;

        return 0;
    }
    else{
        ALOGI("<%s::%d>--[property_get return 0]", __FUNCTION__, __LINE__);
        return -1;
    }
}
/*
static void read_data_from_test_file(void *destiny_buffer, int length,char *url)
{
    //ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    int get_data_length = 0;
    int file_eof = 0;
    FILE *fp = fopen((const char *)url, "r");
    if(fp) {
        fseek(fp, fp_cur_pos, SEEK_SET);
        file_eof = feof(fp);
        if (file_eof == 0) {
            get_data_length = fread(destiny_buffer, 1, length, fp);
            //ALOGI("<%s::%d>--[get_data_length:%d]--[length:%d]", __FUNCTION__, __LINE__, get_data_length, length);
            fp_cur_pos = fp_cur_pos + get_data_length;
//usleep(length * 1000/ 4 / 48 - 1800);

            if(get_data_length < length) {
                memset(destiny_buffer, 0, length);
            }

            if(fp_cur_pos >= file_length)//read the test data again
                fp_cur_pos = 0;
        }
        else {
            //ALOGI("<%s::%d>--[file to end]", __FUNCTION__, __LINE__);
        }
        fclose(fp);
    }
    else {
        //ALOGI("<%s::%d>--[open %s fail, set 0 data]", __FUNCTION__, __LINE__, file_path);
        memset(destiny_buffer, 0, length);
    }
    fp = NULL;


    return ;
}
*/

void *outputThread(void *arg)
{
    char *pFilePath = (char *)arg;
    if (pFilePath == nullptr) {
        ALOGE("[%s:%d] audio file path invalid", __func__, __LINE__);
        return nullptr;
    }
    char *pBuffer = (char *)calloc(1, 3072);

    while (1) {
        int require = 3072;
        int writtensize = 0;
        loopReadFile(pFilePath, pBuffer, 3072);
        do {
            writtensize = glpTracker->write(pBuffer + (3072 - require), require, false);
            if (writtensize > 0) {
                require -= writtensize;
                usleep(5000);
            } else {
//                ALOGE("[%s:%d] write data error:%d", __func__, __LINE__, writtensize);
            }
        } while (require > 0);
    }

}

/*
static void AudioTrackCallback(int event, void* user, void *info) {

    AudioTrack::Buffer *buffer = static_cast<AudioTrack::Buffer *>(info);
    char *file_name = (char *)user;
    if (event != AudioTrack::EVENT_MORE_DATA) {
        ALOGD("%s, audio track envent = %d!\n", __FUNCTION__, event);
        return;
    }
    if (buffer == NULL || buffer->size == 0) {
        return;
    }
    pthread_mutex_lock(&device_change_lock);
    //ALOGI("<%s::%d>--[raw:0x%x]--[size:%d]", __FUNCTION__, __LINE__, buffer->raw, buffer->size);
    //memset((void *)buffer->raw, 0, buffer->size);
    read_data_from_test_file((void *)buffer->raw, buffer->size,file_name);
    pthread_mutex_unlock(&device_change_lock);

    return ;
}
*/
static int AudioTrackRelease(void) {
    if (glpTracker != NULL ) {
        glpTracker->pause();
        glpTracker->flush();
        glpTracker->stop();
        glpTracker = NULL;
    }

    if (gmpAudioTracker != NULL ) {
        gmpAudioTracker.clear();
    }

    return 0;
}

pthread_t writeThreadId;

static int AudioTrackInit(int type, char *pFilePath, audio_stream_type_t stream) {
    status_t Status;
    int flags  = AUDIO_OUTPUT_FLAG_NONE;
    ALOGD("%s, entering...\n", __FUNCTION__);
    gmpAudioTracker = new AudioTrack();
    if (gmpAudioTracker == NULL) {
        ALOGE("%s, new AudioTrack failed.\n", __FUNCTION__);
        return -1;
    }
    if (type == 1)
        flags = AUDIO_OUTPUT_FLAG_HW_AV_SYNC;
    else if (type == 2)
        flags = AUDIO_OUTPUT_FLAG_FAST|AUDIO_OUTPUT_FLAG_RAW;
    else if (type == 3)
        flags = AUDIO_OUTPUT_FLAG_DEEP_BUFFER;

    ALOGD("%s, flags %#x...\n", __FUNCTION__, flags);
    glpTracker = gmpAudioTracker.get();
    #if 0
    Status = glpTracker->set(stream, 48000, AUDIO_FORMAT_PCM_16_BIT,
            AUDIO_CHANNEL_OUT_STEREO, 0, (audio_output_flags_t)flags,
            AudioTrackCallback, url, 0, 0, false, (audio_session_t)0);
    #endif
    Status = glpTracker->set(stream, 48000, AUDIO_FORMAT_PCM_16_BIT,
            AUDIO_CHANNEL_OUT_STEREO, 8192, (audio_output_flags_t)flags,
            nullptr, nullptr, 0, 0, false, (audio_session_t)0);
//    glpTracker->setBufferSizeInFrames(8192);
    if (Status != NO_ERROR) {
        ALOGE("%s, AudioTrack set failed.\n", __FUNCTION__);
        if (gmpAudioTracker != NULL ) {
            gmpAudioTracker.clear();
            glpTracker = NULL;
        }
        return -1;
    }
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    Status = glpTracker->initCheck();
    if (Status != NO_ERROR) {
        ALOGE("%s, AudioTrack initCheck failed.\n", __FUNCTION__);
        AudioTrackRelease();
        return -1;
    }

    pthread_create(&writeThreadId, nullptr, outputThread, pFilePath);
    glpTracker->start();

    Status = glpTracker->setVolume(1.0, 1.0);
    if (Status != NO_ERROR) {
        ALOGE("%s, AudioTrack setVolume failed.\n", __FUNCTION__);
        AudioTrackRelease();
        return -1;
    }
    ALOGD("%s, exit...\n", __FUNCTION__);
    return 0;
}

int new_android_audiotrack(char *pFilePath, int flag, audio_stream_type_t enStreamType)
{
    ALOGI("<%s::%d>", __FUNCTION__, __LINE__);
    int ret = get_file_path(pFilePath);
    if (ret == 0) {
        ALOGI("OUTPUT media.dolbymix.sysfile PCM DATA");
    } else {
        ALOGI("OUTPUT ZERO DATA");
    }
    return AudioTrackInit(flag, pFilePath, enStreamType);
}

int release_android_audiotrack(void) {
    return AudioTrackRelease();
}

