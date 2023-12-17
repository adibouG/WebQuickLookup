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
        LookRequestInit(){}
        LookRequestInit(const QString &label, const QString &word,
                        const bool &isApi = false, const QStringList &keys = QStringList())
                    : _label(label), _search(word), _isApi(isApi),_keys(keys)
        {
            //_url = QUrl();
        }

        QString     _label;
        QString     _search;
        QUrl        _url;
        bool        _isApi = false;
        QStringList _keys;
    };

    struct LookRequestSetting //TODO move in the setting class
    {
        LookRequestSetting(){}
        LookRequestSetting(const QString &label, const QString &url,
                        const bool &isApi = false, const QStringList &keys = QStringList())
            : _label(label), _url(url), _isApi(isApi),_keys(keys)
        {}

        QString     _label;
        QString        _url;
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
    const QPair<QString, QList<LookRequestInit>> lastSearch() const { return _lastSearch; }

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

    QPair<QString, QList<LookRequestInit>>  _lastSearch; // last searched value
    QMap<QString, LookRequestSetting>     settingTest ;
};
#endif // WEBLOOKUPDIALOG_H
