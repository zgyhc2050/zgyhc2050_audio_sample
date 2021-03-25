/*
 * Copyright (C) 2021 Amlogic Corporation.
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



#ifndef __ZGYHC_TRACK_CONFIG_H__
#define __ZGYHC_TRACK_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "zgyhc_common.h"

enum AUDIO_TRACK_CH_TYPE_E {
    AUDIO_TRACK_TYPE_CH_STEREO          = 2,
    AUDIO_TRACK_TYPE_CH_5POINT1         = 6,
    AUDIO_TRACK_TYPE_CH_7POINT1         = 8,
};



int initAudioTrackConfig(zgyhc_audio_info_st *stCfg);
int deinitAudioTrackConfig(void);


#ifdef __cplusplus
}
#endif

#endif //__ZGYHC_TRACK_CONFIG_H__
