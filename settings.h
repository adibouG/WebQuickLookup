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

class Settings : public QWidget
{
    Q_OBJECT

public:
    Settings(const QWidget *parent = nullptr);
    ~Settings();

    QMap<QString, QVariant> settingsList() { return _settingsList; }
    void setSettingsList(const QMap<QString, QVariant> &m) { _settingsList = m; }

private:
         // void parseSettingFile();

private:
    QString                 _settingFile;
    QMap<QString, QVariant> _settingsList;
    int                     _debugLevel = 0; //TODO: use an enum
};
#endif // SETTINGS_H
