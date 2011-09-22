# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := babel

LOCAL_CFLAGS	:=  -DgrUSE_OGR=0					\
		  			-DFIXME=0						\
		  			-DptUSE_FONTWIN=0				\
					-DbbNO_STRICMP=1				\
					-DptUSE_FONTWIN=0				\
					-DbbNO_SPRINTF					\
					-DbbCFGENC=0					\

LOCAL_SRC_FILES := \
					algo.c	\
					Arr.cpp	\
					bit.c	\
					cpg_unicode.c	\
					enc.c	\
					enc_sbc.c	\
					enc_utf16.c	\
					enc_utf8.c	\
					err.c	\
					file.c	\
					fixmath.c	\
					heap.c	\
					log.c	\
					map.c	\
					mem.c	\
					str.c	\
					StrBuf.cpp	\
					timedate.c	\
					Tree.cpp	\
					dir.c	\


LOCAL_C_INCLUDES :=	.	\
					$(LOCAL_PATH)/../include/babel	\

LOCAL_LDLIBS :=	\
				-llog -lc -lgcc -lstdc++ -ldl -lm	\

include $(BUILD_STATIC_LIBRARY)
