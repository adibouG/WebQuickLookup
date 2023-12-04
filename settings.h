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

QT_BEGIN_NAMESPACE
namespace Settings { class WebLookupSettings; }
QT_END_NAMESPACE

class WebLookupSettings : public QObject
{
    Q_OBJECT

public:
    WebLookupSettings(QWidget *parent = nullptr);
    ~WebLookupSettings();

    QMap<QString, QVariant> getSettingsList() { return _settingsList; }

private:
    void parseSettingFile();

private:
    QString                 _settingFile;
    QMap<QString, QVariant> _settingsList;
    int                     _debugLevel = 0; //TODO: use an enum
};
#endif // SETTINGS_H
