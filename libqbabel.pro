TEMPLATE = lib
CONFIG += staticlib
TARGET = qbabel
DEPENDPATH += .
INCLUDEPATH += ./include/babel
DEFINES += bbQT

win32 {
    QMAKE_CXXFLAGS += /Zc:wchar_t /Zp4
    QMAKE_CXXFLAGS_DEBUG += /Zc:wchar_t /Zp4
}

debug {
DEFINES += DEBUG
}

# Input
HEADERS += \
    include/babel/Arr.h \
    include/babel/Registry.h \
    include/babel/strbuf.h \
    include/babel/algo.h \
    include/babel/babel.h \
    include/babel/bit.h \
    include/babel/cpg_unicode.h \
    include/babel/defs.h \
    include/babel/dir.h \
    include/babel/enc.h \
    include/babel/enc_dbc.h \
    include/babel/enc_sbc.h \
    include/babel/enc_utf16.h \
    include/babel/enc_utf8.h \
    include/babel/file.h \
    include/babel/fixmath.h \
    include/babel/heap.h \
    include/babel/log.h \
    include/babel/map.h \
    include/babel/mem.h \
    include/babel/str.h \
    include/babel/timedate.h \
    include/babel/json.h

SOURCES += \
    src/Arr.cpp \
    src/Registry.cpp \
    src/bbStrBuf.cpp \
    src/strbuf.c \
    src/algo.c \
    src/bit.c \
    src/cpg_unicode.c \
    src/dir.c \
    src/dir_qt.cpp \
    src/enc.c \
    src/enc_sbc.c \
    src/enc_utf16.c \
    src/enc_utf8.c \
    src/err.c \
    src/file.c \
    src/file_qt.cpp \
    src/fixmath.c \
    src/heap.c \
    src/log.c \
    src/map.c \
    src/mem.c \
    src/str.c \
    src/timedate.c \
    src/json.c
