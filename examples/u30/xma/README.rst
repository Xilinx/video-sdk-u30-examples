****************************************************
U30 XMA Apps
****************************************************

The ``examples/tutorials/03_xma`` folder contains a standalone XMA-based applications for U30.

XMA Apps Build Instructions
============================

.. rubric:: Environment

The xma apps has been built for and verified on Ubuntu 18.04, RHEL 7.8 and AWS Linux V2.

.. rubric:: Pre-requisites

- The U30 release packages should be installed on the server and the U30 cards needs to be flashed with the shell provided in the release package.

.. rubric:: Build

Export the XRT and XRM to build the xma apps::

    source /opt/xilinx/xrt/setup.sh
    source /opt/xilinx/xrm/setup.sh

Do make clean and make dev in the application directory::

    make clean
    make dev

To genearte DEB or RPM xma apps package, do make DEB or make RPM in the application directory::

    make DEB or make RPM

..
  ------------

  © Copyright 2020 Xilinx, Inc.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
