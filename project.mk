###############################################################################
 #
 # Copyright (C) 2024 Analog Devices, Inc.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
 #
 ##############################################################################
# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analogdevicesinc.github.io/msdk/USERGUIDE/#build-system

MXC_OPTIMIZE_CFLAGS = -Og
# ^ For example, you can uncomment this line to 
# optimize the project for debugging

# ****************** eCTF Bootloader *******************
# DO NOT REMOVE
LINKERFILE  = firmware.ld
STARTUPFILE = startup_firmware.S
ENTRY       = firmware_startup
 
BINS += secrets.S
 
# Set assembler flag if development build
# - Uses different global secrets location
ifeq ($(DEV_BUILD),1) 
	PROJ_AFLAGS+=-DDEV_BUILD
endif

# **********************************************************
# Custom config goes here.

# Where to find source files for this project
VPATH += src
VPATH += lib

# Where to find header files for this project
IPATH += include
IPATH += lib

MAXIM_PATH          = /home/rhys/msdk
DEBUG               = 1
LIB_FREERTOS        = 1
RTOS_CONFIG_DIR     = include
FREERTOS_HEAP_TYPE := 4
export FREERTOS_HEAP_TYPE
