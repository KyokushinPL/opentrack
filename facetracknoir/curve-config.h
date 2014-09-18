#pragma once
#include <QWidget>
#include <QPalette>
#include "ui_ftnoir_curves.h"

class FaceTrackNoIR;

class CurveConfigurationDialog: public QWidget
{
    Q_OBJECT
public:
    CurveConfigurationDialog( FaceTrackNoIR *ftnoir, QWidget *parent );
    void loadSettings();
private:
    Ui::UICCurveConfigurationDialog ui;
    void save();
    // WARN avoid shared_ptr, else circular reference count
    FaceTrackNoIR *mainApp;

private slots:
    void doOK();
    void doCancel();
};
