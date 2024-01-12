#include "weblookupdialog.h"
#include "systemtraywrapper.h"

#include <QApplication>
#include <QMessageBox>

#include <QLabel>
#include <QDebug>

bool USE_SYSTRAY = true;

#ifdef QT_NO_SYSTEMTRAYICON
USE_SYSTRAY = false;
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool useSysTrayWrapper = true;

    if (!USE_SYSTRAY)
    {
        QString text("SystemTray not supported on this platform");
        QLabel *label = new QLabel(text);
        label->setWordWrap(true);
        label->show();
        useSysTrayWrapper = false;
        qDebug() << text;
    }

    else if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(nullptr, QObject::tr("Systray"),
                              QObject::tr("Cannot detect any system tray on this system."));
        useSysTrayWrapper = false;
    }

    WebLookupDialog * window = new WebLookupDialog();


    if (useSysTrayWrapper)
    {
        SystemTrayWrapper *  stw = new SystemTrayWrapper();
        QApplication::setQuitOnLastWindowClosed(false);
        SystemTrayWrapper::connect(stw, &QWidget::close, &app, &QApplication::quit);
        stw->setSysTrayedChildWidget(window);
        window->setParent(stw);
        stw->show();
    }
    else
    {
        window->show();
    }
    return app.exec();

}
