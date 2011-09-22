PROJECT  := babeltest
TARGET   := $(PROJECT).exe
TOPDIR   := $(PWD)/../..
INCDIR   := $(PWD)
SRCDIR   := $(PWD)
CPPFLAGS += -I$(TOPDIR)/include
CXXFLAGS := -DbbSIZEOF_INT=4 -DbbSIZEOF_UPTR=8
LIBS     := $(TOPDIR)/babel.a
include $(TOPDIR)/common.mk
