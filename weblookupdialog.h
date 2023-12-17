/***********************************************************************
 *
 * WebLookupDialog:
 *
 * Main Object with UI provide controls for the app
 *
 ***********************************************************************/
#ifndef WEBLOOKUPDIALOG_H
#define WEBLOOKUPDIALOG_H

#include "webcontentdisplaywidget.h"

//#include "xcb/xcb.h"
//#include <QAbstractNativeEventFilter>

#include <QWidget>
#include <QClipboard>

QT_BEGIN_NAMESPACE
namespace Ui { class WebLookupDialog; }
QT_END_NAMESPACE

class WebLookupDialog: public QWidget
{
    Q_OBJECT

public:
    struct LookRequestInit //TODO move in the setting class
    {
        QString     _label;
        QUrl        _url;
        bool        _isApi = false;
        QStringList _keys;
    };

    enum class LookStatus
    {
        UNKNOW,
        IDLE,
        SET,
        READY,
        LOADING,
        DONE
    };

public:
    WebLookupDialog(QWidget *parent = nullptr);
    ~WebLookupDialog();

    LookRequestInit requestSetup(const QString &label);

public slots:
    void prepareRequest(QClipboard::Mode m);
    void startNewRequest(const QUrl &url, const bool isApi = false);
    void requestEnded();
    void displayClosed();

private:
    void setState(const LookStatus &s) { _state = s; return;}
    LookStatus state() const { return _state; }

private:
    Ui::WebLookupDialog*        ui;
    WebContentDisplayWidget*    _display;    // ptr to the display ui
    LookStatus                  _state;      // hold and share the app and seacrh state
    QClipboard*                 _clipboard;  // ptr to the ciipboard
    QPair<QString, QStringList>                     _lastSearch; // last searched value

    QMap<QString, QString>     _urlList ;
};
#endif // WEBLOOKUPDIALOG_H
