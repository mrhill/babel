TEMPLATE = app
DEPENDPATH += . ../include ../src
INCLUDEPATH += . ../include
DEFINES += bbQT

debug {
DEFINES += DEBUG
}

win32 {
    QMAKE_CXXFLAGS += /Zc:wchar_t /Zp4
    QMAKE_CXXFLAGS_DEBUG += /Zc:wchar_t /Zp4
    QMAKE_LFLAGS_WINDOWS += /NODEFAULTLIB:LIBCMT.lib,LIBCMTD.lib
}
unix {
    QMAKE_LIBDIR += ..
    LIBS += -lqbabel
    POST_TARGETDEPS += ../libqbabel.a
}
