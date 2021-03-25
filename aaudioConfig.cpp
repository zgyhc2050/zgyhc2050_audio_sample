#define LOG_TAG "zgyhc_aaudioConfig"

#include <memory.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/Log.h>
#include <errno.h>
#include <stdlib.h>

#include <android-base/macros.h>
#include <aaudio/AAudio.h>
#include <csignal>



#include <cutils/properties.h>
#include "aaudioConfig.h"


#define MAX_FILE_PATH_BYTE  (1024)
static char g_astrFilePath[MAX_FILE_PATH_BYTE];
static AAudioStreamBuilder *aaudioBuilder = nullptr;
static AAudioStream *aaudioStream1 = nullptr;


void aaudioExitSignalHandler(int sig)
{
    if (sig == SIGINT)
    {
        if (aaudioStream1) {
            AAudioStream_close(aaudioStream1);
        }
        if (aaudioBuilder) {
            AAudioStreamBuilder_delete(aaudioBuilder);
        }
    }
}


static inline int32_t clamp32(int64_t sample)
{
    if ((sample >> 31) ^ (sample >> 63)) {
        sample = 0x7FFFFFFF ^ (sample >> 63);
    }
    return sample;
}

static inline int16_t clamp16(int32_t sample)
{
    if ((sample >> 15) ^ (sample >> 31)) {
        sample = 0x7FFF ^ (sample >> 31);
    }
    return sample;
}

void apply_volume(float volume, void *buf, int sample_size, int bytes)
{
    int16_t *input16 = (int16_t *)buf;
    int32_t *input32 = (int32_t *)buf;
    unsigned int i = 0;

    if (sample_size == 2) {
        for (i = 0; i < bytes / sizeof(int16_t); i++) {
            int32_t samp = (int32_t)(input16[i]);
            input16[i] = clamp16((int32_t)(volume * samp));
        }
    } else if (sample_size == 4) {
        for (i = 0; i < bytes / sizeof(int32_t); i++) {
            int64_t samp = (int64_t)(input32[i]);
            input32[i] = clamp32((int64_t)(volume * samp));
        }
    } else {
        ALOGE("%s, unsupported audio format: %d!\n", __FUNCTION__, sample_size);
    }
    return;
}


void apply_volume_16to32(float volume, int16_t *in_buf, int32_t *out_buf, int bytes)
{
    int16_t *input16 = (int16_t *)in_buf;
    int32_t *output32 = (int32_t *)out_buf;
    unsigned int i = 0;

    for (i = 0; i < bytes / sizeof(int16_t); i++) {
        int32_t samp = ((int32_t)input16[i]) << 16;
        output32[i] = clamp32((int64_t)(samp * (double)(volume)));
    }

    return;
}


// Callback function that fills the audio output buffer.
aaudio_data_callback_result_t aaudioDataCallBack(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        int32_t numFrames) {
    (void) userData;
    int32_t numSamples = AAudioStream_getChannelCount(stream) * numFrames;
    aaudio_format_t format = AAudioStream_getFormat(stream);
    char *pPath  = (char *)userData;

    void *pTempBuffer = NULL;
    pTempBuffer = realloc(pTempBuffer, numSamples * sizeof(int16_t));
    loopReadFile(pPath, pTempBuffer, numSamples * sizeof(int16_t));
    if (format == AAUDIO_FORMAT_PCM_I16) {
        memcpy(audioData, pTempBuffer, numSamples * sizeof(int16_t));
    } else if (format == AAUDIO_FORMAT_PCM_FLOAT) {
        int32_t * ps32AudioData = (int32_t *)audioData;
        int16_t * ps32TempBuf = (int16_t *)pTempBuffer;
        for (int i = 0; i < numSamples; i++) {
            ps32AudioData[2 * i]      = ps32TempBuf[2 * i] << 16;
            ps32AudioData[2 * i + 1]  = ps32TempBuf[2 * i + 1] << 16;
        }
    }

    free(pTempBuffer);
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}


int initAAudioConfig(zgyhc_audio_info_st *stCfg)
{
    if (stCfg->pFilePath == nullptr) {
        ALOGE("[%s:%d] pFilePath is null", __func__, __LINE__);
        return -1;
    }

    aaudio_result_t enRet = AAUDIO_OK;
    int patchLenth = strlen(stCfg->pFilePath);
    if (patchLenth >= MAX_FILE_PATH_BYTE || patchLenth <= 0) {
        ALOGE("[%s:%d] The leng:%d of the wrong", __func__, __LINE__, patchLenth);
        return -1;
    }
    strncpy(g_astrFilePath, stCfg->pFilePath, patchLenth);
    // Use an AAudioStreamBuilder to contain requested parameters.
    enRet = AAudio_createStreamBuilder(&aaudioBuilder);

    // Request stream properties.
    AAudioStreamBuilder_setSampleRate(aaudioBuilder, stCfg->sampleRate);
    AAudioStreamBuilder_setChannelCount(aaudioBuilder, stCfg->channelNum);
    AAudioStreamBuilder_setDataCallback(aaudioBuilder, aaudioDataCallBack, &g_astrFilePath);
    AAudioStreamBuilder_setPerformanceMode(aaudioBuilder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setFormat(aaudioBuilder, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setSharingMode(aaudioBuilder, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setBufferCapacityInFrames(aaudioBuilder, 768);

    // Create an AAudioStream using the Builder.
    enRet = AAudioStreamBuilder_openStream(aaudioBuilder, &aaudioStream1);
    enRet = AAudioStream_requestStart(aaudioStream1);
    ALOGI("[%s:%d] enRet:%#x", __func__, __LINE__, enRet);


    return 0;
}


