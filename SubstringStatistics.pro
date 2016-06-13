TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

INCLUDEPATH += ../HybridLagreDataStorage/
INCLUDEPATH -= ../HybridLagreDataStorage/main.cpp

INCLUDEPATH += ../FASTQParser/
INCLUDEPATH -= ../FASTQParser/main.cpp

SOURCES += ../FASTQParser/Parser.cpp

HEADERS += \
    HLDSFeeder.hpp \
    FASTQExtractor.hpp
