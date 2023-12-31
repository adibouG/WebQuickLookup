/***********************************************************************
 *
 * WebContentDisplayWidget:
 *
 * Mini Simplified WebBrowser to load and display an url content
 *
 ***********************************************************************/

#include "webcontentdisplaywidget.h"
#include "ui_webcontentdisplaywidget.h"

#include "weblookuperror.h"

#include <QDomDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMenu>

#define DEBUG 1


WebContentDisplayWidget::WebContentDisplayWidget(QWidget *parent) :
  QWidget(parent),
    uiDisplay(new Ui::WebContentDisplayWidget),
    _viewer(new QWebEngineView(this))
{
    uiDisplay->setupUi(this);
}

WebContentDisplayWidget::~WebContentDisplayWidget()
{
    for (const auto &i : _pageList) delete i ;
    delete _viewer ;
    delete uiDisplay;
}

/*
 * startRequest
 * @params:
 *  - url: the url to load provided to the constructor by the parent caller object
 */
void WebContentDisplayWidget::startRequest(const SearchRequest &s)
{

    if (s.url.isEmpty())
    {
        //WebLookupDialog* p = qobject_cast<WebLookupDialog*>(parent());
        WebLookupError e("url is missing", objectName());
        return;
    }
    _searchRequest = s;

    if (s.api)
    {
        startApiRequest();
        return;
    }
    else
    {
        connect (_viewer, &QWebEngineView::loadFinished, this, &WebContentDisplayWidget::displayContent);
        _viewer->load(_searchRequest.url);
        return;
    }
}

/*
 * startApiStyleRequest: called automaticcaly once "this" object is constructed
 * @params:
 *  - url: the url to load provided to the constructor by the parent caller object
 */

void WebContentDisplayWidget::startApiRequest()
{

    auto* nam = new QNetworkAccessManager(this);
    // As we won't access the networkAccessManager at later stages,
    // we don't need to store its pointer-ref
    // we leave its destruction to be handled by Qt
    if (DEBUG)
        connect(nam, &QNetworkAccessManager::destroyed, [this] () { qDebug() << "NetworkAccessManager::destroyed"; }) ;

    connect(nam, &QNetworkAccessManager::finished, this, &WebContentDisplayWidget::formatApiResponse);
    nam->get(QNetworkRequest(_searchRequest.url));
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
    _viewer->setHtml(hdoc.toString());//, "text/html" );
    emit _viewer->loadFinished(true);
    //displayContent();
    return;
}

void WebContentDisplayWidget::displayContent()
{
    _pageList.append(_viewer->page());
    //searchDial->lastSearch()
    uiDisplay->verticalLayout_2->addWidget(_viewer);


    qDebug() << "this->size vs sizeHint : " << this->size() << ":::" << sizeHint();
    //resize (250, this->sizeHint().height());
 //   uiDisplay->tabWidget->resize(_viewer->sizeHint()); //pageList.last()->contentsSize());
    qDebug() << "this->size after reSize: " << this->size() ;
    this->show();
    return;
}
