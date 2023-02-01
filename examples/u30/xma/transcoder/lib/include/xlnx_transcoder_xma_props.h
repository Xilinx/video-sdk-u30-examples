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

#ifndef _XLNX_TRANSCODER_XMA_PROPS_H_
#define _XLNX_TRANSCODER_XMA_PROPS_H_

#include <dlfcn.h>
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xlnx_app_utils.h"
#include "xlnx_transcoder_constants.h"

typedef struct XlnxTranscoderProperties {

    XmaDecoderProperties xma_dec_props;
    XmaScalerProperties  xma_scal_props;
    XmaFilterProperties  xma_la_props[TRANSCODE_MAX_ABR_CHANNELS];
    XmaEncoderProperties xma_enc_props[TRANSCODE_MAX_ABR_CHANNELS];

} XlnxTranscoderProperties;

typedef struct XlnxDecoderProperties {
    int32_t dev_index;
    int32_t width;
    int32_t height;
    int32_t fps;
    int32_t codec_type; /* Set to 0 for H264, 1 for HEVC */
    int32_t bit_depth;
    int32_t low_latency;
    int32_t entropy_buffers_count;
    int32_t profile;
    int32_t level;
    int32_t chroma_mode;
    int32_t scan_type;
    int32_t zero_copy;
    int32_t latency_logging;
    int32_t splitbuff_mode;
} XlnxDecoderProperties;

typedef struct XlnxScalerProperties {
    uint64_t      p_mixrate_session;
    int32_t       dev_index;
    int32_t       in_width;
    int32_t       in_height;
    int32_t       bits_per_pixel;
    int32_t       fr_num;
    int32_t       fr_den;
    int32_t       nb_outputs;
    int32_t       out_width[SCAL_MAX_ABR_CHANNELS];
    int32_t       out_height[SCAL_MAX_ABR_CHANNELS];
    char          out_rate[SCAL_MAX_ABR_CHANNELS][SCAL_RATE_STRING_LEN];
    uint32_t      enable_pipeline;
    int32_t       log_level;
    int32_t       latency_logging;
    XmaFormatType xma_fmt_type;
} XlnxScalerProperties;

typedef struct {
    int32_t       dev_index;
    double        cpb_size;
    double        initial_delay;
    int64_t       max_bitrate;
    int64_t       bit_rate;
    int32_t       width;
    int32_t       height;
    int32_t       bits_per_pixel;
    int32_t       fps;
    int32_t       gop_size;
    int32_t       slice_qp;
    int32_t       min_qp;
    int32_t       max_qp;
    int32_t       codec_id;
    int32_t       control_rate;
    int32_t       custom_rc;
    int32_t       gop_mode;
    int32_t       gdr_mode;
    int32_t       num_bframes;
    uint32_t      dynamic_gop;
    int32_t       idr_period;
    int32_t       profile;
    int32_t       level;
    int32_t       tier;
    int32_t       num_slices;
    int32_t       dependent_slice;
    int32_t       slice_size;
    int32_t       lookahead_depth;
    int32_t       temporal_aq;
    int32_t       spatial_aq;
    int32_t       spatial_aq_gain;
    int32_t       qp_mode;
    int32_t       filler_data;
    int32_t       aspect_ratio;
    int32_t       scal_list;
    int32_t       entropy_mode;
    int32_t       loop_filter;
    int32_t       constrained_intra_pred;
    int32_t       prefetch_buffer;
    int32_t       tune_metrics;
    int32_t       num_cores;
    int32_t       latency_logging;
    uint32_t      enable_hw_buf;
    uint8_t       color_desc;
    uint8_t       tx_char;
    uint8_t       color_matrix;
    XmaFormatType xma_fmt_type;
    int32_t       disable_pipeline;
    int32_t       avc_lowlat;
    char*         enc_options;
} XlnxEncoderProperties;

typedef struct {
    XmaFormatType        xma_fmt_type;
    XmaFraction          framerate;
    XlnxLookaheadCodecID codec_type;
    int32_t              dev_index;
    int32_t              width;
    int32_t              height;
    int32_t              stride;
    int32_t              bits_per_pixel;
    int32_t              gop_size;
    uint32_t             lookahead_depth;
    uint32_t             spatial_aq_mode;
    uint32_t             temporal_aq_mode;
    uint32_t             rate_control_mode;
    uint32_t             spatial_aq_gain;
    uint32_t             num_bframes;
    uint32_t             dynamic_gop;
    int32_t              latency_logging;
    uint8_t              enable_hw_buf;
} XlnxLookaheadProperties;

void xlnx_dec_get_xma_props(XlnxDecoderProperties* dec_props,
                            XmaDecoderProperties*  xma_dec_props);

void xlnx_scal_get_xma_props(XlnxScalerProperties* scal_props,
                             XmaScalerProperties*  xma_scal_props);

XmaFrame* xlnx_enc_create_xma_frame(const XlnxEncoderProperties* enc_props);

int32_t xlnx_enc_get_xma_props(XlnxEncoderProperties* enc_props,
                               XmaEncoderProperties*  xma_enc_props);

void xlnx_la_get_xma_props(XlnxLookaheadProperties* la_props,
                           XmaFilterProperties*     xma_la_props);

void xlnx_dec_free_xma_props(XmaDecoderProperties* xma_dec_props);

XmaFrame* xlnx_scal_create_xma_frame(const XlnxScalerProperties* scal_props,
                                     bool should_alloc_buffer);

void xlnx_scal_free_xma_props(XmaScalerProperties* xma_scal_props);

void xlnx_enc_free_xma_props(XmaEncoderProperties* xma_enc_props);

void xlnx_la_free_xma_props(XmaFilterProperties* xma_la_props);

#endif // _XLNX_TRANSCODER_XMA_PROPS_H_
