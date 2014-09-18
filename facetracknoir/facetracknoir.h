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
#include <utility>

#include "ui_facetracknoir.h"

#include "facetracknoir/options.h"
using namespace options;

#include "facetracknoir/main-settings.hpp"

#include "facetracknoir/plugin-support.h"
#include "tracker.h"
#include "facetracknoir/shortcuts.h"
#include "facetracknoir/curve-config.h"
#include "facetracknoir/runner.hpp"

// XXX TODO remove these headers -sh 20140918
#include "ftnoir_protocol_base/ftnoir_protocol_base.h"
#include "ftnoir_tracker_base/ftnoir_tracker_base.h"
#include "ftnoir_filter_base/ftnoir_filter_base.h"

#include "opentrack-version.h"

class Work;				// pre-define class to avoid circular includes
class FaceTrackNoIR;
class KeybindingWorker;

class FaceTrackNoIR : public QMainWindow
{
	Q_OBJECT

public:
    FaceTrackNoIR(QWidget *parent = 0);
	~FaceTrackNoIR();

    void bindKeyboardShortcuts();
    
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
    
    bool is_running() { return state != nullptr; }

private:
    void createIconGroupBox();
	void loadSettings();
    void updateButtonState(bool);
    
    PoseState pose;
    Ui::OpentrackUI ui;
	QTimer timUpdateHeadPose;
    
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
