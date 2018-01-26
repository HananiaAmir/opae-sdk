## Copyright(c) 2017, 2018, Intel Corporation
##
## Redistribution  and  use  in source  and  binary  forms,  with  or  without
## modification, are permitted provided that the following conditions are met:
##
## * Redistributions of  source code  must retain the  above copyright notice,
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer in the documentation
##   and/or other materials provided with the distribution.
## * Neither the name  of Intel Corporation  nor the names of its contributors
##   may be used to  endorse or promote  products derived  from this  software
##   without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE
## IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT  SHALL THE COPYRIGHT OWNER  OR CONTRIBUTORS BE
## LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR
## CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT LIMITED  TO,  PROCUREMENT  OF
## SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS
## INTERRUPTION)  HOWEVER CAUSED  AND ON ANY THEORY  OF LIABILITY,  WHETHER IN
## CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE  OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 2.8.12)
project("opae-c-ase-platform")

# find afu_sim_setup
find_program(AFU_SIM_SETUP_EXECUTABLE
  NAMES
    afu_sim_setup
  PATHS
    /usr/bin
    /usr/local/bin
    /opt/local/bin
    DOC "AFU simulation setup utility")

# Directories
set(ASE_SERVER_RTL     ${PROJECT_SOURCE_DIR})
set(PLATFORM_SHARE_DIR ${CMAKE_SOURCE_DIR}/../platform)
set(PLATFORM_IF_DIR    ${PLATFORM_SHARE_DIR}/platform_if)
set(PLATFORM_IF_RTL    ${PLATFORM_IF_DIR}/rtl/device_if)
set(ASE_SCRIPTS_IN     ${PROJECT_SOURCE_DIR}/../in)

## Verilog compilation
set(VLOG_SRC_LIST
  vlog_files.list
  CACHE STRING "Project file listing all SystemVerilog source code for the AFU")

set(ASE_PKG_FILE ${ASE_SERVER_RTL}/ase_pkg.sv)
set(PLATFORM_PKG_FILE ${PLATFORM_IF_RTL}/ccip_if_pkg.sv)

if(ASE_SIMULATOR STREQUAL "QUESTA")
  find_package(Quartus)
  find_package(Questa)

  set(questa_flags "")
  list(APPEND questa_flags +define+VENDOR_ALTERA)
  list(APPEND questa_flags +define+TOOL_QUARTUS)
  list(APPEND questa_flags +define+${ASE_SIMULATOR})
  list(APPEND questa_flags +define+${ASE_PLATFORM})
  set(QUESTA_VLOG_DEFINES "${questa_flags}"
    CACHE STRING "Modelsim/Questa compiler define flags")

  set(questa_flags "")
  list(APPEND questa_flags +incdir+.+work+${ASE_SERVER_RTL}+${PLATFORM_IF_RTL})
  set(QUESTA_VLOG_INCLUDES "${questa_flags}"
    CACHE STRING "Modelsim/Questa compiler include flags")

  set(questa_flags "")
  list(APPEND questa_flags -nologo -sv +librescan)
  list(APPEND questa_flags -timescale ${ASE_TIMESCALE})
  list(APPEND questa_flags -work work)
  list(APPEND questa_flags -novopt)
  list(APPEND questa_flags ${QUESTA_VLOG_DEFINES})
  list(APPEND questa_flags ${QUESTA_VLOG_INCLUDES})
  list(APPEND questa_flags -f ${ASE_PROJECT_SOURCES})
  list(APPEND questa_flags -l vlog.log)
  set(QUESTA_VLOG_FLAGS "${questa_flags}"
    CACHE STRING "Modelsim/Questa compiler flags")

  set(questa_flags "")
  list(APPEND questa_flags -novopt)
  list(APPEND questa_flags -c)
  list(APPEND questa_flags -dpioutoftheblue 1)
  list(APPEND questa_flags -wlf ${questa_waveform})
  list(APPEND questa_flags -do vsim_run.tcl)
  list(APPEND questa_flags -sv_seed 1234)
  list(APPEND questa_flags -l vlog_run.log)
  set(QUESTA_VSIM_FLAGS "${questa_flags}"
    CACHE STRING "Modelsim/Questa simulator flags")

  # Define rule to generate platform_afu_top_config.vh
  add_custom_command(OUTPUT "questa/rtl/platform_afu_top_config.vh"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMAND cmake -E remove_directory ${PROJECT_BINARY_DIR}/questa
    COMMAND ${AFU_SIM_SETUP_EXECUTABLE} --sources=${CMAKE_BINARY_DIR}/${ASE_PROJECT_SOURCES} -p ${ASE_PLATFORM_ABBREV} -t ${ASE_SIMULATOR} questa)

  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/questa/rtl/include)
  add_custom_command(OUTPUT "include/platform_dpi.h"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/questa/rtl
    COMMAND ${QUESTA_VLIB_EXECUTABLE} work
    COMMAND ${QUESTA_VLOG_EXECUTABLE} -dpiheader ${PROJECT_BINARY_DIR}/questa/rtl/include/platform_dpi.h ${QUESTA_VLOG_FLAGS}
    DEPENDS "questa/rtl/platform_afu_top_config.vh")

  add_custom_command (OUTPUT vlog
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/questa/rtl
    COMMAND ${QUESTA_VLOG_EXECUTABLE} ${QUESTA_VLOG_FLAGS}
    DEPENDS "include/platform_dpi.h")

  # Simulation step
  add_custom_target (vlog_compile ALL
    DEPENDS vlog)

endif()
