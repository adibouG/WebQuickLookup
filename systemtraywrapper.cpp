#include "systemtraywrapper.h"

#include <QMenu>

#define DEFAULT_ICON_PATH "qrc:/img/48x48/logviewer.png"

SystemTrayWrapper::SystemTrayWrapper(QWidget *parent)
    : QWidget(parent)
{

    QIcon icon = QIcon(QString(DEFAULT_ICON_PATH));
    trayIcon = new QSystemTrayIcon(icon);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayWrapper::iconActivated);
}

