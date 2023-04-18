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

#ifndef _XLNX_ENCODER_APP_H_
#define _XLNX_ENCODER_APP_H_

#include "xlnx_enc_arg_parse.h"
#include "xlnx_encoder.h"
#include "xlnx_lookahead.h"

typedef struct XlnxEncoderAppCtx {
    XmaFrame           la_input_xframe;
    XmaFrame*          enc_input_xframe; // enc_input_xframe == la output
    uint32_t           enc_state;
    XlnxEncoderCtx     enc_ctx;
    XlnxLookaheadCtx   la_ctx;
    FILE*              in_file;
    XlnxFormatType     pix_fmt;
    FILE*              out_file;
    size_t             num_frames_to_encode;
    int32_t            loop_count;
    bool               should_convert_input;
    XlnxAppTimeTracker timer;
} XlnxEncoderAppCtx;

#endif // _XLNX_ENCODER_APP_H_
