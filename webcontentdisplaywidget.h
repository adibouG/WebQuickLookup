/***********************************************************************
 *
 * WebContentDisplayWidget:
 *
 * Mini WebBrowser loading and displaying preset of urls content
 *
 ***********************************************************************/

#ifndef WEBCONTENTDISPLAYWIDGET_H
#define WEBCONTENTDISPLAYWIDGET_H

#include "weblookupdialog.h"

#include <QWidget>
#include <QWebEngineView>

#include <QDomDocument>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class WebContentDisplayWidget; }
QT_END_NAMESPACE

// Forward class def
class QNetworkReply;
//class SearchRequest;
class WebContentDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WebContentDisplayWidget(QWidget *parent = nullptr);

    ~WebContentDisplayWidget();
    void startRequest(const SearchRequest &s);
    QWebEngineView* viewer() const { return _viewer; }
private:
    void startApiRequest();
    void displayContent();
    // void formatApiResponse(QNetworkReply*  res, const QStringList &keys);

    QDomElement json2Dom (const QJsonValue &val, QDomDocument doc = QDomDocument(), int depth = 1,
                        const QDomElement &base = QDomElement(), const bool &useTitleKey = false, const QStringList &selection = QStringList());

signals:

private slots:
    void formatApiResponse(QNetworkReply*  res);

private:

    Ui::WebContentDisplayWidget*    uiDisplay;

    QWebEngineView*                 _viewer;
    SearchRequest            _searchRequest;
    QList<QWebEnginePage*>          _pageList;


};

#endif // WEBCONTENTDISPLAYWIDGET_H
