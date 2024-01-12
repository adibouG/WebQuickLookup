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

SystemTrayWrapper::~SystemTrayWrapper()
{
    disconnect(trayIcon, &QSystemTrayIcon::activated, this, &SystemTrayWrapper::iconActivated);
    if (trayIcon) delete trayIcon;
    if (trayIconMenu) delete trayIconMenu;
    if (_sysTrayedChildWidget) delete _sysTrayedChildWidget;
}

void SystemTrayWrapper::iconActivated(QSystemTrayIcon::ActivationReason /*reason*/)
{


}

void SystemTrayWrapper::showSearchDialog()
{


}


void SystemTrayWrapper::showSettingsDialog()
{


}

void SystemTrayWrapper::setVisible(bool)
{


}


void SystemTrayWrapper::closeEvent(QCloseEvent*)
{


}
