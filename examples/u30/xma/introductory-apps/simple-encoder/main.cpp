// © Copyright 2023 Xilinx, Inc. All rights reserved.
// This file contains confidential and proprietary information of Xilinx, Inc.
// and is protected under U.S. and international copyright and other
// intellectual property laws.
//
// DISCLAIMER
// This disclaimer is not a license and does not grant any rights to the
// materials distributed herewith. Except as otherwise provided in a valid
// license issued to you by Xilinx, and to the maximum extent permitted by
// applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
// FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
// IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
// MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort, including
// negligence, or under any other theory of liability) for any loss or damage of
// any kind or nature related to, arising under or in connection with these
// materials, including for any direct, or any indirect, special, incidental, or
// consequential loss or damage (including loss of data, profits, goodwill, or
// any type of loss or damage suffered as a result of any action brought by a
// third party) even if such damage or loss was reasonably foreseeable or Xilinx
// had been advised of the possibility of the same.
//
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-safe, or for use in
// any application requiring failsafe performance, such as life-support or
// safety devices or systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any other applications
// that could lead to death, personal injury, or severe property or
// environmental damage (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and liability of any use of
// Xilinx products in Critical Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
//
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT
// ALL TIMES


#include <boost/archive/basic_archive.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <xma.h>
#include <xrm.h>
#include <xvbm.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>

namespace pt = boost::property_tree;
using namespace std;
#define XCLBIN_PARAM_NAME    ((char *)"/opt/xilinx/xcdr/xclbins/transcode.xclbin")
#define XRM_PRECISION_1000000_BIT_MASK(load) ((load << 8))
#define XLNX_ENCODER_APP_NUM_DEVS 1
#define DEFAULT_DEVICE_ID    0
#define XLNX_ALIGN(x, align) (((x) + (align)-1) & ~((align)-1))
#define MAX_ENCODER_PARAMS 3
#define DEF_AVC_ENC_KEY \
    "[INPUT]\n" \
    "Width = %d\n" \
    "Height = %d\n" \
    "Format = NV12\n" \
    "[RATE_CONTROL]\n" \
    "RateCtrlMode = CBR\n" \
    "FrameRate = %d/1\n" \
    "BitRate = %ld\n" \
    "MaxBitRate = 12000\n" \
    "SliceQP = AUTO\n" \
    "MaxQP = 51\n" \
    "MinQP = 0\n" \
    "CPBSize = 2.000000\n" \
    "InitialDelay = 1.000000\n" \
    "[GOP]\n" \
    "GopCtrlMode = DEFAULT_GOP\n" \
    "Gop.GdrMode = DISABLE\n" \
    "Gop.Length = %d\n" \
    "Gop.NumB = %d\n" \
    "Gop.FreqIDR = %d\n" \
    "[SETTINGS]\n" \
    "Profile = %s\n" \
    "Level = 5\n" \
    "ChromaMode = CHROMA_4_2_0\n" \
    "BitDepth = 8\n" \
    "NumSlices = 1\n" \
    "QPCtrlMode = AUTO_QP\n" \
    "SliceSize = 0\n" \
    "EnableFillerData = DISABLE\n" \
    "AspectRatio = ASPECT_RATIO_AUTO\n" \
    "ColourDescription = COLOUR_DESC_UNSPECIFIED\n" \
    "ScalingList = FLAT\n" \
    "EntropyMode = MODE_CABAC\n" \
    "LoopFilter = ENABLE\n" \
    "ConstrainedIntraPred = DISABLE\n" \
    "LambdaCtrlMode = DEFAULT_LDA\n" \
    "CacheLevel2 = ENABLE\n" \
    "NumCore = 0\n"
#define DEF_HEVC_ENC_KEY \
    "[INPUT]\n" \
    "Width = %d\n" \
    "Height = %d\n" \
    "Format = NV12\n" \
    "[RATE_CONTROL]\n" \
    "RateCtrlMode = CBR\n" \
    "FrameRate = %d/1\n" \
    "BitRate = %ld\n" \
    "MaxBitRate = 12000\n" \
    "SliceQP = AUTO\n" \
    "MaxQP = 51\n" \
    "MinQP = 0\n" \
    "CPBSize = 2.000000\n" \
    "InitialDelay = 1.000000\n" \
    "[GOP]\n" \
    "GopCtrlMode = DEFAULT_GOP\n" \
    "Gop.GdrMode = DISABLE\n" \
    "Gop.Length = %d\n" \
    "Gop.NumB = %d\n" \
    "Gop.FreqIDR = %d\n" \
    "[SETTINGS]\n" \
    "Profile = %s\n" \
    "Level = 5\n" \
    "Tier = MAIN_TIER\n" \
    "ChromaMode = CHROMA_4_2_0\n" \
    "BitDepth = 8\n" \
    "NumSlices = 1\n" \
    "QPCtrlMode = AUTO_QP\n" \
    "SliceSize = 0\n" \
    "DependentSlice = FALSE\n" \
    "EnableFillerData = DISABLE\n" \
    "AspectRatio = ASPECT_RATIO_AUTO\n" \
    "ColourDescription = COLOUR_DESC_UNSPECIFIED\n" \
    "ScalingList = DEFAULT\n" \
    "LoopFilter = ENABLE\n" \
    "ConstrainedIntraPred = DISABLE\n" \
    "LambdaCtrlMode = DEFAULT_LDA\n" \
    "CacheLevel2 = ENABLE\n" \
    "NumCore = 0\n"
#define DEF_ENC_VAL \
    width, \
    height, \
    framerate, \
    bitrate, \
    gop_size, \
    num_bframes, \
    idr_period, \
    profile

/**
 * convertXmaPropsToJson: Converts encoder parameters to json string
 * @param enc_props: Encoder attribute structure
 * @param jsonJob: Stringified json output
 * @param format: Encoder format
 */
void convertXmaPropsToJson(XmaEncoderProperties *enc_props, char* jsonJob, char *format)
{
    pt::ptree enc_topTree, enc_resTree, enc_valTree;
    int channel_load = 0;
    int lookahead = 0;
    enc_valTree.put("function", "ENCODER");
    enc_valTree.put("format", format);
    enc_valTree.put("channel-load", channel_load);
    enc_valTree.put("lookahead-load", lookahead);
    enc_valTree.put("resolution.input.width", enc_props->width);
    enc_valTree.put("resolution.input.height", enc_props->height);
    enc_valTree.put("resolution.input.frame-rate.num", enc_props->framerate.numerator);
    enc_valTree.put("resolution.input.frame-rate.den", enc_props->framerate.denominator);

    enc_resTree.push_back(std::make_pair("",enc_valTree));

    enc_topTree.add_child("request.parameters.resources",enc_resTree);

    std::stringstream json_str;
    boost::property_tree::write_json(json_str, enc_topTree);
    strncpy(jsonJob, json_str.str().c_str(), XRM_MAX_PLUGIN_FUNC_PARAM_LEN - 1);

}


/**
 * encoderPlugin: Creates and sets up the encoder object
 * @param xrmContext: Encoder context
 * @param xma_encoder_props: Video encoder properties
 * @param adv_params_val: Video encoder parameters
 * @param params_val: Encoder management parameters
 * @param params: Storage for params_val
 * @param argv: User specified parameters
 * @param encoder_cu_list_res: Storage for encoder Compute Unit structure
 * @param cu_load: Number of required CUs
 */
int encoderPlugin(xrmContext xrm_ctx,
               XmaEncoderProperties *xma_encoder_props,
               char **adv_params_val,
               uint32_t *params_val,
               XmaParameter *params,
               char **argv,
               xrmCuListResourceV2 *encoder_cu_list_res,
               int32_t *cu_load)
{

    xrmPluginFuncParam   plg_param = {0};
    int32_t func_id = 0;
    char profile[64];
    uint32_t width, height, framerate, gop_size, num_bframes, idr_period;
    uint64_t bitrate;

    // Boilerplate setup
    xma_encoder_props->params = params;
    xma_encoder_props->param_cnt = MAX_ENCODER_PARAMS;
    width      = atoi(argv[3]);
    height     = atoi(argv[4]);
    bitrate    = atoi(argv[5]);
    framerate  = atoi(argv[6]);
    gop_size   = atoi(argv[7]);
    num_bframes = atoi(argv[8]);
    idr_period  = atoi(argv[9]);

    xma_encoder_props->params[0].name   = (char *)"enc_options";
    xma_encoder_props->params[0].type   = XMA_STRING;
    if(!strcmp(argv[10], "HEVC"))
    {
        strcpy(profile, "HEVC_MAIN");
        xma_encoder_props->params[0].length = sprintf(*adv_params_val, DEF_HEVC_ENC_KEY, DEF_ENC_VAL);
    }else
    {
        strcpy(profile, "AVC_MAIN");
        xma_encoder_props->params[0].length = sprintf(*adv_params_val, DEF_AVC_ENC_KEY, DEF_ENC_VAL);
    }
    xma_encoder_props->params[0].value = adv_params_val;

    for (int i=1; i < MAX_ENCODER_PARAMS; i++)
    {
        xma_encoder_props->params[i].type   = XMA_UINT32;
        xma_encoder_props->params[i].length = sizeof(uint32_t);
        params_val[i] = 0;
        xma_encoder_props->params[i].value = &params_val[i];
    }

    xma_encoder_props->params[1].name   = (char *)"latency_logging";
    xma_encoder_props->params[2].name   = (char *)"enable_hw_in_buf";

    strcpy((char *)xma_encoder_props->hwvendor_string, "MPSoC");
    xma_encoder_props->hwencoder_type        = XMA_MULTI_ENCODER_TYPE;
    xma_encoder_props->dev_index             = DEFAULT_DEVICE_ID;
    xma_encoder_props->bits_per_pixel  = 8;
    xma_encoder_props->width           = width;
    xma_encoder_props->height          = height;
    xma_encoder_props->bitrate       = bitrate;
    xma_encoder_props->framerate.numerator  = framerate;
    xma_encoder_props->framerate.denominator  = 1;
    xma_encoder_props->gop_size       = gop_size;
    xma_encoder_props->idr_interval     = idr_period;
    xma_encoder_props->format           = XMA_VCU_NV12_FMT_TYPE;

    convertXmaPropsToJson(xma_encoder_props, plg_param.input, argv[10]);

    //Calculate number of required CUs
    int32_t rc = xrmExecPluginFunc(xrm_ctx, (char *)"xrmU30EncPlugin",
                                func_id, &plg_param);

    *cu_load    = atoi((char*)(strtok(plg_param.output, " ")));
    int32_t skip_value = atoi((char*)(strtok(NULL, " ")));
    (void)skip_value; // To avoid warning

    if(*cu_load <= 0) {
        fprintf(stderr,
                   "Encoder load calculation failed %d\n", rc);
        return XMA_ERROR;
    }
    return XMA_SUCCESS;
}


/**
 * initXma: Globally initializes XMA. Takes no parameters
 */
int initXma()
{
    XmaXclbinParameter xclbin_param[XLNX_ENCODER_APP_NUM_DEVS];
    for(int i = 0; i < XLNX_ENCODER_APP_NUM_DEVS; i++) {
        xclbin_param[i].device_id   = DEFAULT_DEVICE_ID;
        xclbin_param[i].xclbin_name = XCLBIN_PARAM_NAME;
    }
    if( xma_initialize(xclbin_param, XLNX_ENCODER_APP_NUM_DEVS) != XMA_SUCCESS) {
        fprintf(stderr,
                   "XMA Initialization failed\n");
        return XMA_ERROR;
    }
    return XMA_SUCCESS;
}


/**
 * allocCu: Allocate CUs based on required number
 * @param cu_load: Number of required CUs
 * @param xrmContext: Encoder context
 * @param encoder_cu_list_res: Storage for encoder Compute Unit structure
 * @param xma_encoder_props: Video encoder properties
 */
int allocCu(int32_t cu_load, xrmContext xrm_ctx, xrmCuListResourceV2 *encoder_cu_list_res, XmaEncoderProperties *xma_encoder_props)
{
    xrmCuListPropertyV2 xrm_cu_list_prop = {};
    uint64_t dev_info = DEFAULT_DEVICE_ID;
    int32_t cu_reserve_id = 0;
    xrm_cu_list_prop.cuNum = 1;
    strcpy(xrm_cu_list_prop.cuProps[0].kernelName, "encoder");
    strcpy(xrm_cu_list_prop.cuProps[0].kernelAlias, "ENCODER_MPSOC");
    xrm_cu_list_prop.cuProps[0].devExcl = false;
    xrm_cu_list_prop.cuProps[0].requestLoad =
        XRM_PRECISION_1000000_BIT_MASK(cu_load);
    xrm_cu_list_prop.cuProps[0].deviceInfo = dev_info;
    xrm_cu_list_prop.cuProps[0].poolId     = cu_reserve_id;
    xrm_cu_list_prop.cuNum++;
    strcpy(xrm_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_encoder");
    xrm_cu_list_prop.cuProps[1].devExcl = false;
    xrm_cu_list_prop.cuProps[1].requestLoad =
        XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    xrm_cu_list_prop.cuProps[1].deviceInfo = dev_info;
    xrm_cu_list_prop.cuProps[1].poolId     = cu_reserve_id;

    //Allocate CUs
    int32_t rc = xrmCuListAllocV2(xrm_ctx, &xrm_cu_list_prop, encoder_cu_list_res);
    if(rc != XRM_SUCCESS){
        fprintf(stderr,
                   "xrm_allocation: fail to allocate cu list from reserve "
                   "id %d, error %d\n",
                   cu_reserve_id, rc);
        return XMA_ERROR;
    }
    // Set XMA plugin SO and device index */
    xma_encoder_props->plugin_lib = encoder_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_encoder_props->cu_index = encoder_cu_list_res->cuResources[1].cuId;
    xma_encoder_props->channel_id = encoder_cu_list_res->cuResources[1].channelId; // SW kernel always used 100%
    return XMA_SUCCESS;
}
/**
 * createSession: Creates an encoding session, based on the configured properties
 * @param xma_encoder_props: Video encoder properties
 * @param params_val: Encoder's active session
 */
int createSession(XmaEncoderProperties* xma_encoder_props, XmaEncoderSession** xma_encoder_session)
{

    /* XMA to select the ddr bank based on xclbin meta data */
    xma_encoder_props->ddr_bank_index = -1;
    xma_encoder_props->ooo_execution = false;
    fprintf(stderr, "Device ID: %d\n",
         xma_encoder_props->dev_index);
    *xma_encoder_session = xma_enc_session_create(xma_encoder_props);
    if(!*xma_encoder_session) {
      fprintf(stderr,
         "Failed to create encoder session\n");
      return XMA_ERROR;
    }
    return XMA_SUCCESS;
}


/**
 * initBufs: Creates input buffers for encoder
 * @param encoder_props: Video encoder properties
 * @param input_xframe: Encoder's input buffer
 */
int initBufs(XmaEncoderProperties* encoder_props, XmaFrame* input_xframe)
{
    XmaFrameProperties* frame_props = &input_xframe->frame_props;

    frame_props->format      = XMA_VCU_NV12_FMT_TYPE;
    frame_props->width          = encoder_props->width;
    frame_props->height         = encoder_props->height;
    frame_props->bits_per_pixel = encoder_props->bits_per_pixel;
    frame_props->linesize[0]       = encoder_props->width;
    frame_props->linesize[1]       = encoder_props->width;
    size_t plane_size[] = {
            (size_t) (encoder_props->width * encoder_props->height),
            (size_t) (encoder_props->width * encoder_props->height) >> 1,
            (size_t) (encoder_props->width * encoder_props->height) >> 1
    };
    for(int8_t plane_id = 0; plane_id < xma_frame_planes_get(frame_props); plane_id++) {
        input_xframe->data[plane_id].refcount    = 1;
        input_xframe->data[plane_id].buffer_type = XMA_HOST_BUFFER_TYPE;
        input_xframe->data[plane_id].is_clone    = false;
        if(posix_memalign(&input_xframe->data[plane_id].buffer, 4096, plane_size[plane_id]) != 0) {
            fprintf(stderr, "Unable to allocate xma frame buffer for encoding!\n");
            return XMA_ERROR;
        }
    }

    return XMA_SUCCESS;
}


/**
 * cleanup: Cleanup helper routine
 * @param xma_encoder_session: Encoder session
 * @param xrm_ctx: Encoder context
 * @param encoder_cu_list_res: Encoder reserved resources
 * @param input_xframe: Encoder's input buffer
 * @param output_xframe: Encoder's output buffer
 */
void cleanup(XmaEncoderSession* xma_encoder_session, xrmContext *xrm_ctx,
             xrmCuListResourceV2 *encoder_cu_list_res,
             XmaFrame* input_xframe,
             XmaDataBuffer *output_xframe)
{
    int num_planes = xma_frame_planes_get(&input_xframe->frame_props);
    for(int plane_id = 0; plane_id < num_planes; plane_id++) {
        if (input_xframe->data[plane_id].refcount > 0) { // Free memory if not already done so
            free(input_xframe->data[plane_id].buffer);
            input_xframe->data[plane_id].buffer = NULL;
        }
    }
    if (output_xframe->data.buffer) {
        free(output_xframe->data.buffer);
    }
    xma_enc_session_destroy(xma_encoder_session);
    xrmCuListReleaseV2(*xrm_ctx, encoder_cu_list_res);
    xrmCuPoolRelinquish(*xrm_ctx, 0);
    xrmDestroyContext(*xrm_ctx);
}


/**
 * prepFiles: Helper routine to read/write from/to file/stdin&out
 * @param argv: Read and write type
 * @param in_file: Input file descriptor
 * @param out_file: Output file descriptor
 */
void prepFiles(char** argv,  int *in_file, int *out_file)
{

    if ( strncmp ( argv[1],"-", 1) == 0 ){
      *in_file = STDIN_FILENO;
     }else{
      *in_file = open(argv[1], O_RDONLY);
     }

    if ( strncmp ( argv[2],"-", 1) == 0 ){
      *out_file = STDOUT_FILENO;
     }else{
      *out_file = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR |S_IWUSR | S_IRGRP | S_IROTH);
     }

}


/**
 * main: The main processing program. See ./test/pipe.sh for typical usage.
 * @param argv[1]: Input file (use '-' for stdin)
￼* @param argv[2]: Output file (use '-' for stdout)
 * @param argv[3]: Width [pixels]
 * @param argv[4]: Height [pixels]
 * @param argv[5]: Bit rate [kbps]
 * @param argv[6]: Frame rate [s]
 * @param argv[7]: GOP size [frames]
 * @param argv[8]: Number of B frames [frames]
 * @param argv[9]: IDR period [frames]
 * @param argv[10]: Encode type [A264|HEVC]
 */
int main(int argc, char* argv[])
{
    XmaEncoderProperties xma_encoder_props = {};
    XmaParameter         params[MAX_ENCODER_PARAMS] = {0};
    char                 adv_params_val_storage[4096]={0};
    char                 *adv_params_val = adv_params_val_storage;
    uint32_t             params_val[MAX_ENCODER_PARAMS] = {0};
    int32_t              cu_load    = 0;
    xrmContext           xrm_ctx;
    xrmCuListResourceV2  encoder_cu_list_res = {};
    XmaEncoderSession*   xma_encoder_session;
    XmaFrame             input_xframe = {};
    XmaDataBuffer        output_xframe = {};
    int32_t data_used, rc;
    int in_file;
    int out_file;

    // Create context
    xrm_ctx = xrmCreateContext(XRM_API_VERSION_1);
    if(xrm_ctx == NULL) {
        fprintf(stderr,
                   "Create local XRM context failed\n");
        exit(1);
    }

    prepFiles(argv, &in_file, &out_file);

    if (encoderPlugin(xrm_ctx, &xma_encoder_props, &adv_params_val, params_val, params, argv,
                      &encoder_cu_list_res, &cu_load) == XMA_ERROR)
    {
        xrmDestroyContext(xrm_ctx);
        exit(1);
    }

    if (initXma())
    {
        xrmDestroyContext(xrm_ctx);
        exit(1);
    }

    if (allocCu(cu_load, xrm_ctx, &encoder_cu_list_res, &xma_encoder_props))
    {
        xrmDestroyContext(xrm_ctx);
        exit(1);
    }

    if (createSession(&xma_encoder_props, &xma_encoder_session))
    {
        xrmCuListReleaseV2(xrm_ctx, &encoder_cu_list_res);
        xrmCuPoolRelinquish(xrm_ctx, 0);
        xrmDestroyContext(xrm_ctx);
        exit(1);
    }
    initBufs(&xma_encoder_props, &input_xframe);

    //Allocate, once, encoder's output frame
    output_xframe.alloc_size = (3 * xma_encoder_props.width * xma_encoder_props.height) >> 1;
    if(posix_memalign(&output_xframe.data.buffer, 4096, output_xframe.alloc_size) != 0) {
        fprintf(stderr,"Encoder failed to allocate data buffer for recv!\n");
        return XMA_ERROR;
    }

    int y_size = xma_encoder_props.width * xma_encoder_props.height;
    int uv_size = y_size >> 1;

    int rc_snd = XMA_SUCCESS, rc_rcv = XMA_SUCCESS;
    rc = 1;
    int dataread = y_size+uv_size;
    int enc_size=0;

    //Main processing loop
    while(rc_rcv != XMA_EOS && rc && dataread == (y_size+uv_size) ) {
        //Allocate more input buffer if needed
        if (input_xframe.data[0].refcount <=0){
            initBufs(&xma_encoder_props, &input_xframe);
        }
        //Read raw and send to encoder
        dataread = read(in_file, input_xframe.data[0].buffer, y_size);
        dataread += read(in_file, input_xframe.data[1].buffer, uv_size);
        rc_snd = xma_enc_session_send_frame(xma_encoder_session, &input_xframe);
        //Read more raw or pickup the encoded data
        if ( rc_snd == XMA_SEND_MORE_DATA) {
            continue;
        } else if (rc_snd == XMA_SUCCESS){
            rc_rcv = xma_enc_session_recv_data(xma_encoder_session, &output_xframe, &enc_size);
            if ( rc_rcv == XMA_SUCCESS ) {
                //Fully encoded frame, send it to out_file descriptor
                rc = (write(out_file, output_xframe.data.buffer, enc_size) == enc_size);
                fsync(out_file);
            }
            else if (rc_rcv == XMA_TRY_AGAIN){
                sched_yield(); //Too busy - let go and retry
                continue;
            } else {
                fprintf(stderr,"%d Error session_recv_frame_list --- %d\n",__LINE__,rc_rcv);
            }
        } else {
            fprintf(stderr,"%d Error session_send_data --- %d\n",__LINE__,rc_snd);
            break;
        }
    }
    //Process tail end
    if ( rc_snd != XMA_ERROR && rc_rcv != XMA_ERROR) {
        enc_size = 0;
        rc_snd = XMA_FLUSH_AGAIN;
        rc = 1;
        while(rc_rcv != XMA_EOS && rc && enc_size >= 0) {
            fprintf(stderr,"Flushing\n");
            if ( rc_snd == XMA_FLUSH_AGAIN ){
                XmaFrame flush_xframe = {};
                flush_xframe.is_last_frame = 1;
                flush_xframe.pts = -1;
                flush_xframe.data[0].buffer = NULL;
                rc_snd = xma_enc_session_send_frame(xma_encoder_session, &flush_xframe);
            }
            rc_rcv = xma_enc_session_recv_data(xma_encoder_session, &output_xframe, &enc_size);
            if ( rc_rcv == XMA_SUCCESS ) {
                rc = (write(out_file, output_xframe.data.buffer, enc_size) == enc_size);
                fsync(out_file);
            }
            else if (rc_rcv == XMA_TRY_AGAIN){
                sched_yield();
                continue;
            }
        }
    }
    //Cleanup
    cleanup(xma_encoder_session, &xrm_ctx, &encoder_cu_list_res, &input_xframe, &output_xframe);
    close(in_file);
    close(out_file);

}
