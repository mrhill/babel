TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += . ../../include
DEFINES += bbQT

QMAKE_CXXFLAGS += /Zc:wchar_t /Zp4
QMAKE_CXXFLAGS_DEBUG += /Zc:wchar_t /Zp4

# Input
FORMS = 
HEADERS +=
SOURCES += babeltest.c strbuf.cpp
RESOURCES +=
