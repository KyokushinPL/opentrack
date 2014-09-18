#pragma once

#include <QString>
#include <QSettings>
#include "facetracknoir/main-settings.hpp"
#include "qfunctionconfigurator/functionconfig.h"

class Mapping {
public:
    Mapping(QString primary,
            QString secondary,
            int max_input,
            int max_output,
            int max_input_alt,
            int max_output_alt,
            axis_opts* opts) :
        headPos(0),
        curve(max_input, max_output),
        curveAlt(max_input_alt, max_output_alt),
        opts(*opts),
        name1(primary),
        name2(secondary)
    {
        QSettings settings("opentrack");
        QString currentFile = settings.value ( "SettingsFile", QCoreApplication::applicationDirPath() + "/settings/default.ini" ).toString();
        QSettings iniFile( currentFile, QSettings::IniFormat );
        curve.loadSettings(iniFile, primary);
        curveAlt.loadSettings(iniFile, secondary);
    }
    volatile double headPos;
    FunctionConfig curve;
    FunctionConfig curveAlt;
    axis_opts const& opts;
    const QString name1, name2;
};