#include "facetracknoir/facetracknoir.h"
#include "facetracknoir/curve-config.h"
CurveConfigurationDialog::CurveConfigurationDialog(FaceTrackNoIR *ftnoir, QWidget *parent) :
    QWidget( parent, Qt::Dialog ), mainApp(ftnoir)
{
	ui.setupUi( this );
    setFont(qApp->font());

    QPoint offsetpos(120, 30);
	this->move(parent->pos() + offsetpos);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(doOK()));
    connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(doCancel()));

    tie_setting(mainApp->s.a_x.altp, ui.tx_altp);
    tie_setting(mainApp->s.a_y.altp, ui.ty_altp);
    tie_setting(mainApp->s.a_z.altp, ui.tz_altp);
    tie_setting(mainApp->s.a_yaw.altp, ui.rx_altp);
    tie_setting(mainApp->s.a_pitch.altp, ui.ry_altp);
    tie_setting(mainApp->s.a_roll.altp, ui.rz_altp);

    tie_setting(mainApp->s.tcomp_p, ui.tcomp_enable);
    tie_setting(mainApp->s.tcomp_tz, ui.tcomp_rz);

    tie_setting(mainApp->s.a_x.zero, ui.pos_tx);
    tie_setting(mainApp->s.a_y.zero, ui.pos_ty);
    tie_setting(mainApp->s.a_z.zero, ui.pos_tz);
    tie_setting(mainApp->s.a_yaw.zero, ui.pos_rx);
    tie_setting(mainApp->s.a_pitch.zero, ui.pos_ry);
    tie_setting(mainApp->s.a_roll.zero, ui.pos_rz);

    tie_setting(mainApp->s.a_yaw.invert, ui.invert_yaw);
    tie_setting(mainApp->s.a_pitch.invert, ui.invert_pitch);
    tie_setting(mainApp->s.a_roll.invert, ui.invert_roll);
    tie_setting(mainApp->s.a_x.invert, ui.invert_x);
    tie_setting(mainApp->s.a_y.invert, ui.invert_y);
    tie_setting(mainApp->s.a_z.invert, ui.invert_z);
    
    tie_setting(mainApp->s.a_yaw.src, ui.src_yaw);
    tie_setting(mainApp->s.a_pitch.src, ui.src_pitch);
    tie_setting(mainApp->s.a_roll.src, ui.src_roll);
    tie_setting(mainApp->s.a_x.src, ui.src_x);
    tie_setting(mainApp->s.a_y.src, ui.src_y);
    tie_setting(mainApp->s.a_z.src, ui.src_z);

	loadSettings();
}

void CurveConfigurationDialog::doOK() {
	save();
	this->close();
}

void CurveConfigurationDialog::doCancel() {
    mainApp->b->revert();
    loadSettings();
    close();
}

void CurveConfigurationDialog::loadSettings() {
    QFunctionConfigurator* configs[6] = {
        ui.txconfig,
        ui.tyconfig,
        ui.tzconfig,
        ui.rxconfig,
        ui.ryconfig,
        ui.rzconfig
    };

    QFunctionConfigurator* alt_configs[6] = {
        ui.txconfig_alt,
        ui.tyconfig_alt,
        ui.tzconfig_alt,
        ui.rxconfig_alt,
        ui.ryconfig_alt,
        ui.rzconfig_alt
    };

    for (int i = 0; i < 6; i++)
    {
        configs[i]->setConfig(&mainApp->axis(i).curve, mainApp->axis(i).name1);
        alt_configs[i]->setConfig(&mainApp->axis(i).curveAlt, mainApp->axis(i).name2);
    }
}

void CurveConfigurationDialog::save() {
    QSettings settings("opentrack");
    QString currentFile =
            settings.value("SettingsFile",
                           QCoreApplication::applicationDirPath() + "/settings/default.ini" )
            .toString();
    
    struct {
        QFunctionConfigurator* qfc;
        Axis axis;
        bool altp;
    } qfcs[] =
    {
        { ui.rxconfig, Yaw, false },
        { ui.ryconfig, Pitch, false},
        { ui.rzconfig, Roll, false },
        { ui.txconfig, TX, false },
        { ui.tyconfig, TY, false },
        { ui.tzconfig, TZ, false },
        
        { ui.rxconfig_alt, Yaw, true },
        { ui.ryconfig_alt, Pitch, true},
        { ui.rzconfig_alt, Roll, true },
        { ui.txconfig_alt, TX, true },
        { ui.tyconfig_alt, TY, true },
        { ui.tzconfig_alt, TZ, true },
        { nullptr, Yaw, false }
    };
    
    for (int i = 0; qfcs[i].qfc; i++)
    {
        Mapping& axis = mainApp->axis(qfcs[i].axis);
        qfcs[i].qfc->saveSettings(currentFile, qfcs[i].altp ? axis.name2 : axis.name1);
    }
}
