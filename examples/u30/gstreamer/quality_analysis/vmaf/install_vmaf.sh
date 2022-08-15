#! /bin/bash
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
#
# NOTICE:  BY INVOKING THIS SCRIPT AND USING THE SOFTWARE INSTALLED BY THE
# SCRIPT, YOU AGREE ON BEHALF OF YOURSELF AND YOUR EMPLOYER (IF APPLICABLE)
# TO BE BOUND TO THE LICENSE AGREEMENTS APPLICABLE TO THE SOFTWARE THAT YOU
# INSTALL BY RUNNING THE SCRIPT.

# BY ELECTING TO CONTINUE, YOU WILL CAUSE THIS SCRIPT FILE TO AUTOMATICALLY
# INSTALL A VARIETY OF SOFTWARE COPYRIGHTED
# BY XILINX AND THIRD PARTIES THAT IS SUBJECT TO VARIOUS LICENSE AGREEMENTS
# THAT APPEAR UPON INSTALLATION, ACCEPTANCE AND/OR ACTIVATION OF THE
# SOFTWARE AND/OR ARE CONTAINED OR DESCRIBED IN THE CORRESPONDING RELEASE
# NOTES OR OTHER DOCUMENTATION OR HEADER OR SOURCE FILES. XILINX DOES NOT
# GRANT TO LICENSEE ANY RIGHTS OR LICENSES TO SUCH THIRD-PARTY SOFTWARE.
# LICENSEE AGREES TO CAREFULLY REVIEW AND ABIDE BY THE TERMS AND CONDITIONS
# OF SUCH LICENSE AGREEMENTS TO THE EXTENT THAT THEY GOVERN SUCH SOFTWARE.

# BY ELECTING TO CONTINUE, YOU WILL CAUSE THE FOLLOWING SOFTWARE TO BE DOWNLOADED
# AND INSTALLED ON YOUR SYSTEM. BY ELECTING TO CONTINUE, YOU UNDERSTAND THAT THE
# INSTALLATION OF THE SOFTWARE LISTED BELOW MAY ALSO RESULT IN THE INSTALLATION
# ON YOUR SYSTEM OF ADDITIONAL SOFTWARE NOT LISTED BELOW IN ORDER TO OPERATE
# (SUCH SOFTWARE IS HEREAFTER REFERRED TO AS ‘DEPENDENCIES’)
# XILINX DOES NOT GRANT TO LICENSEE ANY RIGHTS OR LICENSES TO SUCH DEPENDENCIES
# LICENSEE AGREES TO CAREFULLY REVIEW AND ABIDE BY THE TERMS AND CONDITIONS
# OF ANY LICENSE AGREEMENTS TO THE EXTENT THAT THEY GOVERN SUCH DEPENDENCIES

# BY ELECTING TO CONTINUE, YOU WILL CAUSE THE FOLLOWING SOFTWARE PACKAGES
# (AND THEIR RESPECTIVE DEPENDENCIES, IF APPLICABLE) TO BE DOWNLOADED FROM
# https://gitlab.freedesktop.org/gstreamer/gst-plugins-base AND INSTALLED ON YOUR SYSTEM:

# - gst-plugins-base-1.16.2
# - gst-plugins-base-1.17.1

# BY ELECTING TO CONTINUE, YOU WILL CAUSE THE FOLLOWING SOFTWARE PACKAGES
# (AND THEIR RESPECTIVE DEPENDENCIES, IF APPLICABLE) TO BE DOWNLOADED FROM
# https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/tree/master/ext/iqa
# AND INSTALLED ON YOUR SYSTEM:

# - gst-plugins-bad 1.16.2 (iqa)

# BY ELECTING TO CONTINUE, YOU WILL CAUSE THE FOLLOWING SOFTWARE PACKAGES
# (AND THEIR RESPECTIVE DEPENDENCIES, IF APPLICABLE) TO BE DOWNLOADED FROM
# https://github.com/werti/vmaf.git revision=v1.3.14-gstreamer AND INSTALLED
# ON YOUR SYSTEM:
# - vmaf v1.3.14-gstreamer

# BY ELECTING TO CONTINUE, YOU ACKNOWLEDGE AND AGREE, FOR YOURSELF AND ON BEHALF
# OF YOUR EMPLOYER (IF APPLICABLE), THAT XILINX IS NOT DISTRIBUTING TO YOU IN
# THIS FILE ANY OF THE AFORMENTIONED SOFTWARE OR DEPENDENCIES, AND THAT YOU ARE
# SOLELY RESPONSIBLE FOR THE INSTALLATION OF SUCH SOFTWARE AND DEPENDENCIES ON
# YOUR SYSTEM AND FOR CAREFULLY REVIEWING AND ABIDING BY THE TERMS AND CONDITIONS
# OF ANY LICENSE AGREEMENTS TO THE EXTENT THAT THEY GOVERN SUCH SOFTWARE AND DEPENDENCIES
#


#####################################
###### Setup VVAS Environment #######
#####################################

if [[ $PATH == /opt/xilinx/vvas/bin* ]] && \
   [[ $LD_LIBRARY_PATH == /opt/xilinx/vvas/lib* ]] && \
   [[ $PKG_CONFIG_PATH == /opt/xilinx/vvas/lib/pkgconfig* ]] && \
   [[ $GST_PLUGIN_PATH == /opt/xilinx/vvas/lib/gstreamer-1.0* ]]
then
	echo "Already has VVAS environment variables set correctly"
else
	echo "Does not have VVAS environment paths. Setting using /opt/xilinx/vvas/setup.sh"
	source /opt/xilinx/vvas/setup.sh
fi


BASEDIR=$PWD

os_distr=`lsb_release -a | grep "Distributor ID:"`
os_version=`lsb_release -a | grep "Release:"`

echo $os_distr
echo $os_version

cpu_count=`cat /proc/cpuinfo | grep processor | wc -l`

echo CPU = $cpu_count



cp ./patches/* /tmp/
retval=$?
if [ $retval -ne 0 ]; then
	echo "Unable to copy patches"
        return 1
fi

# GStreamer base package installation with patch

cd /tmp/ && wget https://gstreamer.freedesktop.org/src/gst-plugins-base/gst-plugins-base-1.16.2.tar.xz && \
    tar -xvf gst-plugins-base-1.16.2.tar.xz && cd gst-plugins-base-1.16.2 && \
    cd common && patch -p1 < /tmp/0001-build-Adapt-to-backwards-incompatible-change-in-GNU-.patch && cd .. && \
    patch -p1 < /tmp/0001-Add-Xilinx-s-format-support.patch
    patch -p1 < /tmp/0001-Videoaggregator-cleanup-functions.patch
    patch -p1 < /tmp/0001-gst-plugins-base-Add-HDR10-support.patch
retval=$?
if [ $retval -ne 0 ]; then
	echo "Unable to apply patch"
	cd $BASEDIR
	return 1
fi
    ./autogen.sh --prefix=/opt/xilinx/vvas --disable-gtk-doc && \
    make -j$cpu_count && sudo make install && \
retval=$?
if [ $retval -ne 0 ]; then
	echo "Unable to install base gstreamer plugins ($retval)"
	cd $BASEDIR
	return 1
fi
cd $BASEDIR
rm -rf /tmp/gst-plugins-base-1.16.2*


# GStreamer bad package installation
cd /tmp/ && wget https://gstreamer.freedesktop.org/src/gst-plugins-bad/gst-plugins-bad-1.16.2.tar.xz && \
    tar -xvf gst-plugins-bad-1.16.2.tar.xz && cd gst-plugins-bad-1.16.2 && \
 cd common && patch -p1 < /tmp/0001-build-Adapt-to-backwards-incompatible-change-in-GNU-.patch && cd .. && \
 patch -p1 < /tmp/0001-VMAF-integration-in-gst-plugins-bad-1.16.2.patch && \
 patch -p1 < /tmp/0001-Update-Colorimetry-and-SEI-parsing-for-HDR10.patch && \
 patch -p1 < /tmp/0001-Derive-src-fps-from-vui_time_scale-vui_num_units_in_.patch && \
mkdir subprojects && cd subprojects && git clone https://github.com/werti/vmaf.git -b v1.3.14-gstreamer && \
cd vmaf &&  patch -p1 < /tmp/0001-Building-the-vmaf-as-dynamic-library.patch
cd /tmp/gst-plugins-bad-1.16.2 && \
meson --prefix=/opt/xilinx/vvas  --libdir=lib -Dyadif=disabled -Dmpegpsmux=disabled build && \
cd build && ninja && sudo ninja install

retval=$?
if [ $retval -ne 0 ]; then
        echo "Unable to install bad gstreamer plugins ($retval)"
        cd $BASEDIR
        return 1
fi
cd $BASEDIR
rm -rf /tmp/gst-plugins-bad-1.16.2*


#Remove GStreamer plugin cache
rm -rf ~/.cache/gstreamer-1.0/

echo "#######################################################################"
echo "########         VMAF plugin installed successfully          ########"
echo "#######################################################################"
