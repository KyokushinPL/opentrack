#ifndef __TRACKER_H__
#define __TRACKER_H__

#include <QThread>
#include <QMutex>
#include "plugin-support.h"
#include <ftnoir_tracker_base/ftnoir_tracker_types.h>

#include <qfunctionconfigurator/functionconfig.h>
#include "tracker_types.h"
#include "facetracknoir/main-settings.hpp"
#include "facetracknoir/options.h"
#include "facetracknoir/timer.hpp"
using namespace options;

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

class PoseState {
public:
    Mapping axes[6];
    PoseState(std::vector<axis_opts*> opts) :
        axes {
            Mapping("tx","tx_alt", 100, 100, 100, 100, opts[TX]),
            Mapping("ty","ty_alt", 100, 100, 100, 100, opts[TY]),
            Mapping("tz","tz_alt", 100, 100, 100, 100, opts[TZ]),
            Mapping("rx", "rx_alt", 180, 180, 180, 180, opts[Yaw]),
            Mapping("ry", "ry_alt", 90, 90, 90, 90, opts[Pitch]),
            Mapping("rz", "rz_alt", 180, 180, 180, 180, opts[Roll])
        }
    {}
};

class Work : protected QThread {
	Q_OBJECT

private:
    QMutex mtx;
    const main_settings& s;
    PoseState& pose;
    volatile bool should_quit;
    Timer t;
protected:
	void run();

public:
    Work(const PoseState& pose);
    ~Work();

    // XXX TODO make one function
    void getHeadPose(double *data);
    void getOutputHeadPose(double *data);
    volatile bool do_center;
    volatile bool enabled;
    
    Camera output_camera, raw_6dof;

    void start() { QThread::start(); }
};

#endif
