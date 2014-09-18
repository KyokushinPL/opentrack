#pragma once

#include <QFrame>

#include "facetracknoir/pose-state.hpp"
#include "facetracknoir/plugin-support.h"
#include "facetracknoir/tracker.h"

#include "ftnoir_tracker_base/ftnoir_tracker_base.h"
#include "ftnoir_filter_base/ftnoir_filter_base.h"
#include "ftnoir_protocol_base/ftnoir_protocol_base.h"

class Runner {
public:
    Runner(PoseState& pose,
           Runner& runner,
           QFrame* video_container,
           Plugin& t1,
           Plugin& t2,
           Plugin& f,
           Plugin& p) :
        tracker1(nullptr),
        tracker2(nullptr),
        filter(nullptr),
        proto(nullptr),
        tracker1_dialog(nullptr),
        tracker2_dialog(nullptr),
        proto_dialog(nullptr),
        filter_dialog(nullptr),
        plugin_tracker1(t1),
        plugin_tracker2(t2),
        plugin_filter(f),
        plugin_proto(p),
        runner(runner),
        correct(false)
    {
        // invoke them all w/o short circuiting
        // easier debug if something goes wrong -sh

        if (t1.Constructor)
            tracker1 = (ITracker*) t1.Constructor();
        
        if (t2.Constructor)
            tracker2 = (ITracker*) t2.Constructor();
        
        if (p.Constructor)
            proto = (IProtocol*) p.Constructor();
        
        if (f.Constructor)
            filter = (IFilter*) f.Constructor();
        
        if (p.Constructor)
            if(!proto->checkServerInstallationOK())
                return;
        
        if (!(tracker1 && proto))
            return;
        
        tracker1->StartTracker(video_container);

        if (tracker2)
            tracker2->StartTracker(video_container);
        
        correct = true;
        
        work = std::make_shared<Work>(pose);
        work->start();
    }
    
    ~Runner()
    {   
        work.reset();
        
        if (tracker1_dialog)
            tracker1_dialog->unRegisterTracker();
        
        if (tracker2_dialog)
            tracker2_dialog->unRegisterTracker();
        
        if (proto_dialog)
            proto_dialog->unRegisterProtocol();
        
        if (filter_dialog)
            filter_dialog->unregisterFilter();
        
        // XXX TODO use pointer class deleting on outta scope
        // avoid boilerplate in ctor and dtor -sh 20140918
        delete tracker1;
        delete tracker2;
        delete proto;
        delete filter;
        delete tracker1_dialog;
        delete tracker2_dialog;
        delete proto_dialog;
        delete filter_dialog;
    }
    
    void query(double* raw_data, double* mapped_data)
    {
        work->query(raw_data, mapped_data);
    }
    
    bool is_correct() { return correct; }
    
public:
    ITracker* tracker1;
    ITracker* tracker2;
    IFilter* filter;
    IProtocol* proto;
    
    ITrackerDialog* tracker1_dialog;
    ITrackerDialog* tracker2_dialog;
    IProtocolDialog* proto_dialog;
    IFilterDialog* filter_dialog;
    
    Plugin plugin_tracker1, plugin_tracker2, plugin_filter, plugin_proto;
    
    ptr<Work> work;
private:
    Runner& runner;
    bool correct;
};