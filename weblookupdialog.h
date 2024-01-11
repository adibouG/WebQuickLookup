/***********************************************************************
 *
 * WebLookupDialog:
 *
 * Main Object with UI provide controls for the app
 *
 ***********************************************************************/
#ifndef WEBLOOKUPDIALOG_H
#define WEBLOOKUPDIALOG_H

#include "settings.h"

//#include "xcb/xcb.h"
//#include <QAbstractNativeEventFilter>

#include <QWidget>
#include <QClipboard>

#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class WebLookupDialog; }
QT_END_NAMESPACE


struct SearchRequest {
    QString word;
    QUrl    url;
    QString  label;
    bool api = false;
    QStringList  keys = QStringList();
};

class WebContentDisplayWidget;


class WebLookupDialog: public QWidget
{
    Q_OBJECT
public:
    friend class WebContentDisplayWidget;

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

    SearchRequest requestSetup(const QString &search, Settings::LookRequestSetting set);
    const QList<SearchRequest> lastSearch() const { return _lastSearch; }

public slots:
    void prepareRequest(QClipboard::Mode m);
    void requestEnded();
    void displayClosed();

private slots:
    void startNewRequest(bool append);
    void appendSearch(int index);

private:
    void setState(const LookStatus &s) { _state = s; }
    LookStatus state() const { return _state; }

private:
    Ui::WebLookupDialog*        ui;
    WebContentDisplayWidget*    _display;    // ptr to the display ui
    LookStatus                  _state;      // hold and share the app and seacrh state
    QClipboard*                 _clipboard;  // ptr to the ciipboard

    QList<SearchRequest>  _lastSearch; // last search

    QList<Settings::LookRequestSetting>     settingTest;  //will come from settings but added here for dev ease
};
#endif // WEBLOOKUPDIALOG_H
