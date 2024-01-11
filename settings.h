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




class Settings
{

public:
    Settings();
    Settings(const Settings &s)
        : _settingFile(s._settingFile)
        , _settingsList(s._settingsList)
    {}

    ~Settings(){}

public:
    struct LookRequestSetting {

        LookRequestSetting (const QString label, const QString url,
                           bool api=false, QStringList keys=QStringList())
            : label(label),url(url),api(api), keys(keys)
        {}
        ~LookRequestSetting ()
        {}

        QString label;
        QString url;
        bool api;
        QStringList  keys;
    };

    QMap<QString, QVariant> settingsList() { return _settingsList; }
    void setSettingsList(const QMap<QString, QVariant> &m) { _settingsList = m; }
    // void parseSettingFile();

private:
    QString                 _settingFile;
    QMap<QString, QVariant> _settingsList;

};
#endif // SETTINGS_H
