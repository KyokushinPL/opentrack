/********************************************************************************
* FaceTrackNoIR		This program is a private project of the some enthusiastic	*
*					gamers from Holland, who don't like to pay much for			*
*					head-tracking.												*
*																				*
* Copyright (C) 2010	Wim Vriend (Developing)									*
*						Ron Hendriks (Researching and Testing)					*
*																				*
* Homepage																		*
*																				*
* This program is free software; you can redistribute it and/or modify it		*
* under the terms of the GNU General Public License as published by the			*
* Free Software Foundation; either version 3 of the License, or (at your		*
* option) any later version.													*
*																				*
* This program is distributed in the hope that it will be useful, but			*
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY	*
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for	*
* more details.																	*
*																				*
* You should have received a copy of the GNU General Public License along		*
* with this program; if not, see <http://www.gnu.org/licenses/>.				*
*********************************************************************************/

#pragma once

#include <QMainWindow>
#include <QApplication>
#include <QWidget>
#include <QDialog>
#include <QUrl>
#include <QList>
#include <QKeySequence>
#include <QShortcut>
#if !defined(_WIN32)
#	include "qxt-mini/QxtGlobalShortcut"
#else
#	include <windows.h>
#endif
#include <QThread>
#include <QDebug>
#include <memory>

#include "ui_facetracknoir.h"

#include "facetracknoir/options.h"
using namespace options;

#include "facetracknoir/main-settings.hpp"

#include "facetracknoir/plugin-support.h"
#include "tracker.h"
#include "facetracknoir/shortcuts.h"
#include "facetracknoir/curve-config.h"

// XXX TODO remove these headers -sh 20140918
#include "ftnoir_protocol_base/ftnoir_protocol_base.h"
#include "ftnoir_tracker_base/ftnoir_tracker_base.h"
#include "ftnoir_filter_base/ftnoir_filter_base.h"

#include "opentrack-version.h"

class Work;				// pre-define class to avoid circular includes
class FaceTrackNoIR;
class KeybindingWorker;

class FaceTrackNoIR : public QMainWindow, IDynamicLibraryProvider
{
	Q_OBJECT

public:
    FaceTrackNoIR(QWidget *parent = 0);
	~FaceTrackNoIR();

    QFrame *get_video_widget();
    void bindKeyboardShortcuts();
    Plugin current_tracker1() {
        return dlopen_trackers.value(ui.iconcomboTrackerSource->currentIndex(), Plugin());
    }
    Plugin current_tracker2() {
        return dlopen_trackers.value(ui.cbxSecondTrackerSource->currentIndex() - 1, Plugin());
    }
    Plugin current_protocol() {
        return dlopen_protocols.value(ui.iconcomboProtocol->currentIndex(), Plugin());
    }
    Plugin current_filter() {
        return dlopen_filters.value(ui.iconcomboFilter->currentIndex(), Plugin());
    }
    
#if defined(_WIN32)
    Key keyCenter;
    Key keyToggle;
    ptr<KeybindingWorker> keybindingWorker;
#else 
    QxtGlobalShortcut keyCenter;
    QxtGlobalShortcut keyToggle;
#endif
    bundle b;
    main_settings s;
public slots:
    void shortcutRecentered();
    void shortcutToggled();

private:
    void createIconGroupBox();
	void loadSettings();
    void updateButtonState(bool);
    
    PoseState pose;
    Ui::OpentrackUI ui;
	QTimer timUpdateHeadPose;
    
    // XXX TODO move to new header file
    class Runner {
    public:
        typedef const Plugin& lib;
        
        Runner(PoseState& pose, QFrame* video_container, lib t1, lib t2, lib f, lib p) :
            tracker1(nullptr),
            tracker2(nullptr),
            filter(nullptr),
            proto(nullptr),
            tracker1_dialog(nullptr),
            tracker2_dialog(nullptr),
            proto_dialog(nullptr),
            filter_dialog(nullptr),
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
            
            t = std::make_shared<Work>(pose);
            
            t->start();
        }
        
        ~Runner()
        {   
            t.reset();
            
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
            t->getOutputHeadPose(mapped_data);
            t->getHeadPose(raw_data);
        }
        
        bool is_correct() { return correct; }
        
    private:
        ITracker* tracker1;
        ITracker* tracker2;
        IFilter* filter;
        IProtocol* proto;
        
        ITrackerDialog* tracker1_dialog;
        ITrackerDialog* tracker2_dialog;
        IProtocolDialog* proto_dialog;
        IFilterDialog* filter_dialog;
        
        ptr<Work> t;
        
        bool correct;
    };
    
	ptr<KeyboardShortcutDialog> _keyboard_shortcuts;
	ptr<CurveConfigurationDialog> _curve_config;
    
    ptr<Runner> state;

    QList<Plugin> dlopen_filters;
    QList<Plugin> dlopen_trackers;
    QList<Plugin> dlopen_protocols;
    QShortcut kbd_quit;

#ifndef _WIN32
    void bind_keyboard_shortcut(QxtGlobalShortcut&, key_opts& k);
#endif
    void fill_profile_cbx();
    int looping;
    
private slots:
    void open();
    void save();
    void saveAs();
    void exit();
    void profileSelected(int index);
    
    void showTrackerSettings();
    void showSecondTrackerSettings();
    
    void showServerControls();
    void showFilterControls();
    void showKeyboardShortcuts();
    void showCurveConfiguration();
    
    void showHeadPose();
    
    void startTracker();
    void stopTracker();
};
