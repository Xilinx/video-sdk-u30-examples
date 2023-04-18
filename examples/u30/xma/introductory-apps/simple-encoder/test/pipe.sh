#!/bin/bash
#
# Copyright (C) 2023, Xilinx Inc - All rights reserved
# Xilinx Emcoder
#
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#


# Sample command run: pipe.sh 1920 1080 4000 30 60 2 60 H264

W=$1
H=$2
BitRate=$3
FrameRate=$4
GopSize=$5
NoBFrames=$6
IdrPer=$7
EncType=$8


Cmd=$(cat << EOS
gst-launch-1.0 --quiet videotestsrc pattern= zone-plate kx2=20 ky2=20 kxy=4 kt2=10 num-buffers=300 ! 
video/x-raw, framerate=${FrameRate}/1, width=${W}, height=${H} ! 
timeoverlay font-desc='Sans 15' !  
filesink location=/dev/stdout |
../build/u30_xma_encoder_simple - vid.es $W $H $BitRate $FrameRate $GopSize $NoBFrames $IdrPer $EncType
EOS
)

echo Executing $Cmd
eval $Cmd