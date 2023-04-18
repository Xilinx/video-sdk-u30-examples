/*
 * Copyright (C) 2021, Xilinx Inc - All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef _XLNX_ENC_DYN_PARAMS_H_
#define _XLNX_ENC_DYN_PARAMS_H_

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <xma.h>

typedef struct XlnxLaDynParams {
    bool     is_spatial_gain_changed;
    uint32_t spatial_aq_gain;
    bool     is_temporal_mode_changed;
    bool     temporal_aq_mode;
    bool     is_spatial_mode_changed;
    bool     spatial_aq_mode;
} XlnxLaDynParams;

typedef struct XlnxEncDynParams {
    bool     is_bitrate_changed;
    uint32_t bit_rate;
    bool     is_bframes_changed;
    uint8_t  num_b_frames;
    bool     is_min_max_qp_changed;
    uint32_t min_qp;
    uint32_t max_qp;
} XlnxEncDynParams;

typedef struct XlnxDynParams {
    XlnxEncDynParams enc_dyn_param;
    XlnxLaDynParams  la_dyn_param;
} XlnxDynParams;

typedef struct XlnxDynParamFrames {
    uint32_t      frame_num;
    XlnxDynParams dyn_params;
} XlnxDynParamFrames;

#endif // _XLNX_ENC_DYN_PARAMS_H_
