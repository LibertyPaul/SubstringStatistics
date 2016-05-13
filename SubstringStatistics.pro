TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS +=

INCLUDEPATH += ./boost_1_60_0/

INCLUDEPATH += ./HybridLagreDataStorage/
INCLUDEPATH -= ./HybridLagreDataStorage/main.cpp

INCLUDEPATH += ./FASTQParser/
INCLUDEPATH -= ./FASTQParser/main.cpp
SOURCES += ./FASTQParser/Parser.cpp
