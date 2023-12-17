/***********************************************************************
 *
 * WebContentDisplayWidget:
 *
 * Mini Simplified WebBrowser to load and display an url content
 *
 ***********************************************************************/

#ifndef WEBCONTENTDISPLAYWIDGET_H
#define WEBCONTENTDISPLAYWIDGET_H

#include <QWidget>
#include <QWebEngineView>


QT_BEGIN_NAMESPACE
namespace Ui { class WebContentDisplayWidget; }
QT_END_NAMESPACE


// Forward class def
class QNetworkReply;

class WebContentDisplayWidget : public QWidget
{
    Q_OBJECT
    friend class WebLookupDialog;
public:
    explicit WebContentDisplayWidget(QWidget *parent = nullptr);
    WebContentDisplayWidget(const QUrl &url, const bool api = false, QWidget *parent = nullptr);
    ~WebContentDisplayWidget();

    void startRequest(const QUrl &url, const bool api);

private:
    void startApiRequest(const QUrl &url);
    void displayContent();

signals:

private slots:
    void formatApiResponse(QNetworkReply*  res);

private:

    Ui::WebContentDisplayWidget*    uiDisplay;

    QWebEngineView                  _view;

};

#endif // WEBCONTENTDISPLAYWIDGET_H
