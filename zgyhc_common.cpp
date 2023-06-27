

#define LOG_TAG "zgyhc_common"
#include <errno.h>
#include <system/audio.h>

#include "zgyhc_common.h"
#include <cutils/log.h>

#include <stdlib.h> //  size_t
#include <string.h> //strerror



void loopReadFile(char *pFilePath, void *pBuffer, unsigned int u32Size)
{
    static unsigned int u32CurPosition = 44;
//    if (aml_getprop_bool("media.audiohal.file")) {
         int s32Ret = 0;
         size_t u32ReadSize = 0;
         FILE *pFile = fopen(pFilePath, "r");
         if (pFile == NULL) {
             ALOGE("open file failed, %s", strerror(errno));
         }
         s32Ret = fseek(pFile, u32CurPosition, SEEK_SET);
         if (s32Ret != 0) {
             ALOGE("open fseek failed, s32Ret:%#x, %s", s32Ret, strerror(errno));
         }
         u32ReadSize = fread(pBuffer, 1, u32Size, pFile);
         if (u32ReadSize != u32Size) {
             ALOGI("restart read file");
             u32CurPosition = 44;
             fseek(pFile, 0, SEEK_SET);
             u32ReadSize = fread((char *)pBuffer+u32ReadSize, 1, u32Size-u32ReadSize, pFile);
          }
          u32CurPosition += u32ReadSize;
          //ALOGI("read file, u32CurPosition:%d, u32ReadSize:%d", u32CurPosition, u32ReadSize);
         fclose(pFile);
//     } else {
//         memset(pBuffer, 0, u32Size);
//     }
}

