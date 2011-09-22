PROJECT  := babel
TARGET   := $(PROJECT).a
TOPDIR   := $(PWD)/..
INCDIR   := $(PWD)/include/babel
SRCDIR   := $(PWD)/src
CXXFLAGS := -DbbSIZEOF_INT=4 -DbbSIZEOF_UPTR=8
include common.mk
