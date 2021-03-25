#ifndef __ZGYHC_COMMON_H__
#define __ZGYHC_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <system/audio-base.h>

typedef struct {
    char                    *pFilePath;
    audio_output_flags_t    enFlags;
    audio_stream_type_t     enStreamType;
    unsigned int            channelNum;
    int                     channelMask;
    unsigned int            sampleRate;
    bool                    bAaudio;
} zgyhc_audio_info_st;


void loopReadFile(char *pFilePath, void *pBuffer, unsigned int u32Size);

#ifdef __cplusplus
}
#endif

#endif //__ZGYHC_COMMON_H__

