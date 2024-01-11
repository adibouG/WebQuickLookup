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

#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMenu>

#define DEBUG 1


WebContentDisplayWidget::WebContentDisplayWidget(QWidget *parent) :
  QWidget(parent),
    ui(new Ui::WebContentDisplayWidget),
    _viewer(new QWebEngineView(this))
{
    ui->setupUi(this);
}

WebContentDisplayWidget::~WebContentDisplayWidget()
{
    for (const auto &i : _pageList) delete i ;
    delete _viewer ;
    delete ui;
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

    connect(nam, &QNetworkAccessManager::sslErrors, this, [](QNetworkReply* res, const QList<QSslError> errors) {
            for (const auto &s : errors)
            {
                qDebug() << "ssl error : " << s .error() << " " <<  s.errorString() ;
            }
            return;// TODO: message box with  continue / cancel option
        }
    );

    connect(nam, &QNetworkAccessManager::finished, this, &WebContentDisplayWidget::formatApiResponse);
    nam->get(QNetworkRequest(_searchRequest.url));
    return;
}



void WebContentDisplayWidget::formatApiResponse(QNetworkReply*  res)
{

    if (res->isRunning()) return;

    if (DEBUG)
    {
         connect(res, &QNetworkReply::destroyed, [] () { qDebug() << "QNetworkReply::destroyed"; }) ;
    }


    QVariant statusCode = res->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (!statusCode.isNull())
    {
        qDebug() << " NetworkRequest HttpStatusCodeAttribute :" << statusCode.toInt() << " " << res->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();

        if (statusCode.toInt() >= 400)
        {

            if (res->error() != QNetworkReply::NoError) qDebug() << " received error " << res->errorString();
            return ; // TODO : handle the issue

        }
        else if (statusCode.toInt() >= 300)
        {

            return;

        }
        // we have a 200
    }

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

    QDomDocument hdoc;// the data returned (json usually) will be parsed and used to build a dom / html document
    QDomElement html = hdoc.createElement("html");
    hdoc.appendChild(html);
    QDomElement root = hdoc.createElement("body");
    html.appendChild(root);
    QDomElement searchTitlePart = hdoc.createElement("div");
    root.appendChild(searchTitlePart);
    QDomElement tH1 = hdoc.createElement("h1");
    searchTitlePart.appendChild(tH1);
    QDomText searchTitle = hdoc.createTextNode(QString("Result for %1 from  %2").arg(_searchRequest.word, _searchRequest.label));
    tH1.appendChild(searchTitle);

    QVariant contentType = res->header(QNetworkRequest::ContentTypeHeader) ;
    qDebug() << "contentType : " << contentType.toString();

    if (!contentType.isNull() && contentType.toString().contains("json"))
    {
        QJsonParseError* p = new QJsonParseError();
 qDebug() << "contentType : " << contentType.toString();
        QJsonDocument jdoc = QJsonDocument::fromJson(data, p);

        if (p->error != QJsonParseError::NoError)
        {
            qDebug() << "Json parse error : " << p->errorString();
        }

        if (this->_searchRequest.label.contains("wiki", Qt::CaseInsensitive))
        {
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
        }
        else
        {
            auto el = json2Dom(QJsonValue::fromVariant(jdoc.toVariant()), hdoc);
            root.appendChild(el);
        }
    }
    qDebug() << hdoc.toString();
    _viewer->setHtml(hdoc.toString());//, "text/html" );
    emit _viewer->loadFinished(true);
    //displayContent();
    return;
}


QDomElement WebContentDisplayWidget::json2Dom (const QJsonValue &val, QDomDocument doc, int depth,
                                            const QDomElement &base, const bool &useTitleKey, const QStringList &selection)
{
    if (val.isNull() || val.isUndefined())
        return base;

    QDomElement baseElement = base;
    if (base.isNull())
        baseElement = doc.createElement("div");

    if (val.isArray())
    {
        QJsonArray arr = val.toArray();

        if (!arr.size())
            return base;


        for (const auto &i : arr)
        {
            auto el = json2Dom(i, doc, depth, baseElement);
            if (!el.isNull())
                baseElement.appendChild(el);
        } // for
    } //
    else if (val.isString() && val.toString().length())
    {
        QString text = val.toString();
        if (text.contains(QRegularExpression("(<[a-z]{1,9}[1-9]{0,1}>)"))) // check if text contains html style tags
        {
            QDomDocument content ;
            content.setContent(text) ;
            qDebug() << content.toString();
            baseElement.appendChild (content);
        }
        else if (text.startsWith("https://")) // check if this is an image url
        {
            auto mimetype = QStringList() << ".tiff" << ".webp" << ".svg"<< ".jpeg" << ".jpg" << ".png";
            for (const auto &t : mimetype) // for
            {
                if (text.endsWith(t) && QUrl(text).isValid()) // if
                {
                    QDomElement img = doc.createElement("image");
                    img.setAttribute("src", text);
                    img.setAttribute("width", "20%") ;
                    baseElement.appendChild(img);
                    break;
                } // __________________// if
            } // ____________________________// for
        }
        else
        {
            QDomElement textElement = doc.createElement("p");
            QDomText textContent = doc.createTextNode(text);
            textElement.appendChild(textContent);
            baseElement.appendChild(textElement);
        }
    } // else if
    else if (val.isObject())
    {
        QJsonObject obj = val.toObject();

        bool addElement = false;
        bool addTitle = false;
        bool addSection = false;

        for (const auto &k : obj.keys())
        {
            QJsonValue v = obj.value(k);

            if (v.isNull() || v.isUndefined())
                continue;

            if (selection.length() && !selection.contains(k))
                continue;

            QString text;
            QString titleValue = k;
            QString titleElementValue = QString("h%1").arg(++depth);
            //check that this key value can be used as title
            if (useTitleKey && k.contains("title", Qt::CaseInsensitive) && obj.value(k).isString())
            {
                addTitle = true;
                addElement = true;
                titleValue = QString("[%1] %2").arg(k, obj.value(k).toString());
                QDomElement titleElement = doc.createElement(titleElementValue);
                QDomText titleContent = doc.createTextNode(titleValue);
                titleElement.appendChild(titleContent);
                baseElement.appendChild(titleElement);
            }
            else
            {
                QDomElement titleElement = doc.createElement(titleElementValue);
                QDomText titleContent = doc.createTextNode(titleValue);
                titleElement.appendChild(titleContent);
                baseElement.appendChild(titleElement);
                auto el = json2Dom(v, doc, depth, baseElement);
                if (!el.isNull())
                    baseElement.appendChild(el);

            }
        }
    }
    return baseElement;
}


void WebContentDisplayWidget::displayContent()
{
    _pageList.append(_viewer->page());
    //searchDial->lastSearch()
    ui->verticalLayout_2->addWidget(_viewer);


    qDebug() << "this->size vs sizeHint : " << this->size() << ":::" << sizeHint();
    //resize (250, this->sizeHint().height());
 //   uiDisplay->tabWidget->resize(_viewer->sizeHint()); //pageList.last()->contentsSize());
    qDebug() << "this->size after reSize: " << this->size() ;
    this->show();
    return;
}
