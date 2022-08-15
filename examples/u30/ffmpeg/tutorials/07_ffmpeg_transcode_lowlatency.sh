#!/bin/bash
#
# Copyright 2020-2022, Xilinx, Inc.
# Copyright 2022, Advanced Micro Device, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
#
#!/bin/bash

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <h264 clip>"
    exit 1
fi
ffprobe -hide_banner -loglevel fatal -show_streams -select_streams v $1 -hide_banner -show_entries frame=pict_type -of csv=p=0 | \
grep -w B > /dev/null && echo "Input file cannot contain B frames" && exit 1

DecoderOpt="-low_latency 1 -splitbuff_mode 1"
EncoderOpt="-disable-pipeline 1 -avc-lowlat 0"

ffmpeg -re -y -hide_banner -loglevel info -c:v mpsoc_vcu_h264 ${DecoderOpt} -latency_logging 1 -i $1 -c:v mpsoc_vcu_h264 ${EncoderOpt} -bf 0 -latency_logging 1 -f mp4 /tmp/low_latency.mp4


