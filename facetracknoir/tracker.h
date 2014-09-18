#ifndef __TRACKER_H__
#define __TRACKER_H__

#include <QThread>
#include <QMutex>
#include "plugin-support.h"
#include <ftnoir_tracker_base/ftnoir_tracker_types.h>

#include <qfunctionconfigurator/functionconfig.h>
#include "tracker_types.h"

#include "facetracknoir/pose-state.hpp"
#include "facetracknoir/main-settings.hpp"
#include "facetracknoir/options.h"
#include "facetracknoir/timer.hpp"
using namespace options;

class Work : protected QThread {
	Q_OBJECT

private:
    QMutex mtx;
    PoseState& pose;
    volatile bool should_quit;
    Timer t;
protected:
	void run();

public:
    Work(PoseState &pose);
    ~Work();

    // XXX TODO make one function
    void query(double *raw, double* mapped);
    volatile bool do_center;
    volatile bool enabled;
    
    Camera output_camera, raw_6dof;

    void start() { QThread::start(); }
};

#endif
