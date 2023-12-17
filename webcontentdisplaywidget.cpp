/***********************************************************************
 *
 * WebContentDisplayWidget:
 *
 * Mini Simplified WebBrowser to load and display an url content
 *
 ***********************************************************************/

#include "webcontentdisplaywidget.h"
#include "ui_webcontentdisplaywidget.h"

#include "weblookupdialog.h"
#include "weblookuperror.h"

#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMenu>

#define DEBUG 1


WebContentDisplayWidget::WebContentDisplayWidget(QWidget *parent) :
    QWebEngineView(parent),
    uiDisplay(new Ui::WebContentDisplayWidget)
{
    uiDisplay->setupUi(this);
}


WebContentDisplayWidget::WebContentDisplayWidget(const QUrl &url, const bool api, QWidget *parent) :
    QWebEngineView(parent),
    uiDisplay(new Ui::WebContentDisplayWidget)
{
    uiDisplay->setupUi(this);
    if (!url.isEmpty())
        startRequest(url, api);
}

WebContentDisplayWidget::~WebContentDisplayWidget()
{
    delete uiDisplay;
}

/*
 * StartNewRequest: called automaticcaly once "this" object is constructed with an url parameter
 * @params:
 *  - url: the url to load provided to the constructor by the parent caller object
 */
void WebContentDisplayWidget::startRequest(const QUrl &url, const bool api)
{

    if (url.isEmpty())
    {
        //WebLookupDialog* p = qobject_cast<WebLookupDialog*>(parent());
        WebLookupError e("url is missing", objectName());
        return;
    }
    connect (this, &QWebEngineView::loadFinished, this, &WebContentDisplayWidget::displayContent);
    if (api)
    {
        startApiRequest(url, QStringList());
        return;
    }
    load(url);
    return;
}

/*
 * startApiStyleRequest: called automaticcaly once "this" object is constructed
 * @params:
 *  - url: the url to load provided to the constructor by the parent caller object
 */

void WebContentDisplayWidget::startApiRequest(const QUrl &url, const QStringList &keys)
{
    auto* nam = new QNetworkAccessManager(this) ;
    // As we won't access the networkAccessManager at later stages,
    // we don't need to store its pointer-ref
    // we leave its destruction to be handled by Qt
    if (DEBUG)
        connect(nam, &QNetworkAccessManager::destroyed, [] () { qDebug() << "NetworkAccessManager::destroyed"; }) ;

    connect(nam, &QNetworkAccessManager::finished, this, &WebContentDisplayWidget::formatApiResponse);
    nam->get(QNetworkRequest(url));
    return;
}



void WebContentDisplayWidget::formatApiResponse(QNetworkReply*  res)
{

    if (res->isRunning()) return;

    if (DEBUG)
    {
        connect(res, &QNetworkReply::finished, [] () { qDebug() << "QNetworkReply::finished"; }) ;
        connect(res, &QNetworkReply::destroyed, [] () { qDebug() << "QNetworkReply::destroyed"; }) ;
    }

    if (res->error())
        qDebug() << " received error " << res->errorString();

    // response data processing
    // TODO look header for erwsponse type and format
    QByteArray data = res->readAll();

    if (data.isEmpty() )
    {
        WebLookupError e("No data in response body");
        return;
    }

    // Debug log the http response body
    if (DEBUG) qDebug() << " data  \n" <<  data;


    QDomDocument hdoc; // the data returned (json usually) will be parsed and used to build a dom / html document

    //TODO  check body data mime type
    if (res->url().toString().contains("wiki"))
    {
        QJsonDocument jdoc = QJsonDocument::fromJson(data);
        QString title;
        QString description;
        QString image;
        QString textExtract;
        QString linkToHtml;
        if (!jdoc.isEmpty())
        {
            title = jdoc["title"].toString();
            description = jdoc["description"].toString();
            image = jdoc["thumbnail"].toObject().value("source").toString();
            textExtract = jdoc["extract_html"].toString();
            linkToHtml = jdoc["content_urls"].toObject()
                             .value("desktop").toObject()
                             .value("page").toString();

        }

        QDomElement html = hdoc.createElement("html");
        hdoc.appendChild(html);
        QDomElement root = hdoc.createElement("body");
        html.appendChild(root);
        QDomElement titlePart = hdoc.createElement("div");
        root.appendChild(titlePart);
        QDomElement tH2 = hdoc.createElement("h2");
        titlePart.appendChild(tH2);
        QDomText titleContent = hdoc.createTextNode(title);
        tH2.appendChild(titleContent);

        QDomElement descPart = hdoc.createElement("div");
        QDomElement dP = hdoc.createElement("p");
        QDomText descContent = hdoc.createTextNode(description);

        root.appendChild(descPart);
        descPart.appendChild(dP);
        dP.appendChild(descContent);

        QDomElement br = hdoc.createElement("br");
        descPart.appendChild(br);

        QDomElement img = hdoc.createElement("image");
        img.setAttribute("src", image);
        img.setAttribute("width", "20%") ;
        descPart.appendChild(img);


        QDomElement link = hdoc.createElement("a");
        link.setAttribute("href", linkToHtml);
        QDomText linkContent = hdoc.createTextNode("link to article...");
        link.appendChild(linkContent);
        descPart.appendChild(link);


        QDomElement extractPart = hdoc.createElement("div");
        root.appendChild(extractPart);

        QDomDocument eContent ;

        eContent.setContent(textExtract) ;
        qDebug() << eContent.toString();
        extractPart.appendChild (eContent);
        qDebug() << hdoc.toString();
    }
    setHtml(hdoc.toString());//, "text/html" );
    emit loadFinished(true);
    //displayContent();
    return;
}

void WebContentDisplayWidget::displayContent()
{
   // WebLookupDialog* searchDial = qobject_cast<WebLookupDialog*>(parent());
    QWebEngineView * pageView = new QWebEngineView(this);
   QWebEnginePage* webPage = new QWebEnginePage(this->page());
    _pageList.append(webPage);
   pageView->setPage(webPage);
    //searchDial->lastSearch()    //uiDisplay->gridLayout->addWidget(&_view); // >->itemAtPosition(1,0)->setAlignment(Qt::AlignBottom);
    uiDisplay->gridLayout_2->addWidget(pageView);  //, this->page()->url().toString());
        //-gridLayout->addWidget (, 0, 0, Qt::AlignTop);
    qDebug() << "this->size vs sizeHint : " << this->size() << ":::" << sizeHint();
   // qDebug() << "web->size vs sizeHint : " << page->size() << ":::" << page.sizeHint();
    resize (250, this->sizeHint().height());
    qDebug() << "this->size after reSize: " << this->size() ;
    this->show();
//    WebLookupDialog* searchUI = qobject_cast<WebLookupDialog*>(parent());
//    searchUI->requestEnded();
    return;
}
