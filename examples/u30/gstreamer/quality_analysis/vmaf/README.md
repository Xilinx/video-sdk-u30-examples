
Integration of vmaf gstreamer plugin in gst-plugins-bad-1.16.2

VMAF gstreamer plugin can be compiled by following the steps mentioned in https://werti.github.io/GSoC2019/ .  Our requirement is to integrate https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/tree/master/ext/iqa  with gst-plugins-bad-1.16.2 . The following steps will simplify the integration of vmaf plugin in gst-plugins-bad-1.16.2.

1.	This directory contains the installation scripts and patches required for installation.

├── install_vmaf.sh
├── model
├── patches
│   ├── 0001-Add-Xilinx-s-format-support.patch
│   ├── 0001-Videoaggregator-cleanup-functions.patch
│   └── 0001-VMAF-integration-in-gst-plugins-bad-1.16.2.patch
└── setup.sh

2.	model is a directory which contains different models used by iqa-vmaf plugin. The discussion of this topic is out of scope.
3.	0001-Add-Xilinx-s-format-support.patch adds a patch to gst-plugins-base-1.16.2 which supports xilinx’s format buffers and discussion about this patch is out of scope for this topic .
4.	0001-Videoaggregator-cleanup-functions.patch  adds a patch to gst-plugins-base-1.16.2/gst-libs/gst/video/gstvideoaggregator.h . This patch contains the macros of some cleanup functions which were later introduced in gst-plugins-base-1.17.1. This patch is required to build the iqa-vmaf plugin.
5.	0001-VMAF-integration-in-gst-plugins-bad-1.16.2.patch adds a patch to gst-plugins-bad-1.16.2. i) This patch replaces gst-plugins-bad-1.16.2/ext/iqa directory with https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/tree/master/ext/iqa which contains the source code of iqa-vmaf plugin. 
ii) This patch also add new directory named subprojects with required wrap file. 
a)	https://gitlab.freedesktop.org/szve/gst-build/-/blob/master/subprojects/vmaf.wrap
iiI) This patch also updates meson.build in gst-plugins-bad-1.16.2 with the following line.
     
      subproject('vmaf')


6.	subproject('vmaf') in meson.build clones the vmaf library source code  pointed in vmaf.wrap file into the subprojects directory.

7.	setup.sh sets up environment variables useful for VVAS if not set before.

8.	By running install_vmaf.sh 

i)	it applies the required patches from patches directory .
ii)	it builds the dependent vmaf library which is already cloned in subprojects .
iii)	it builds and installs the iqa-vmaf plugin.

cd vmaf
source install_vmaf.sh

9.	By inspecting the plugin using gst-inspect-1.0 tells us that vmaf plugin is successfully installed.

gst-inspect-1.0 iqa-vmaf

10.	The description and usage  of the iqa-vmaf plugin can be found in https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/blob/master/ext/iqa/iqa-vmaf.c


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




