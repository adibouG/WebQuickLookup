/***********************************************************************
 *
 * WebLookupSettings:
 *
 * Object with UI to Load, store and provide the settings
 *
 ***********************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QFile>
#include <QVariant>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialog; }
QT_END_NAMESPACE


struct LookRequestSetting {
    LookRequestSetting (const QString label, const QString url, bool api=false, QStringList keys=QStringList()):
        label(label),url(url),api(api) {}
    ~LookRequestSetting (){}
    QString label;
    QString url;
    bool api;
    QStringList  keys;
};



class SettingsDialog : public QWidget
{
    Q_OBJECT

public:


public:
    SettingsDialog(const QWidget *parent = nullptr);
    ~SettingsDialog();

    QMap<QString, QVariant> settingsList() { return _settingsList; }
    void setSettingsList(const QMap<QString, QVariant> &m) { _settingsList = m; }

private:
         // void parseSettingFile();


private:
    Ui::SettingsDialog*        ui;

    QString                 _settingFile;
    QMap<QString, QVariant> _settingsList;

    int                     _debugLevel = 0; //TODO: use an enum
};
#endif // SETTINGS_H
