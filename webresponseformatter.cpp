/***********************************************************************
 *
 * WebResponseFormatter:
 *
 * Mostly to dtructure and format an object returned by an API
 * as API response body most often are JSON object or arrays ,
 * but xml or othernot display friendly data format supports can be implemented
 *
 *
 ***********************************************************************/

#include "webresponseformatter.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDomDocument>

#define DEFAULTKEYS "title, description, thumbnail, extract_html, content_urls.desktop.page"

const QString WebResponseFormatter::format(const QVariant &data, const QString &mimeType = QString(), const QStringList &keyList = QString(DEFAULTKEYS).split(',', Qt::SkipEmptyParts))
{
    if (data.isNull())
        return QString();

    // we start building a Dom tree
    QDomDocument hdoc;
    QDomElement html = hdoc.createElement("html");
    hdoc.appendChild(html);
    QDomElement bodyroot = hdoc.createElement("body");
    html.appendChild(bodyroot);

    //TODO use or make a more generic parser from this
    if (keyList.isEmpty())
    {
        // TODO: convert all the object entries into a Dom tree struct
        //QJsonDocument jdoc = QJsonDocument::fromJson (data.toJsonDocument());
    }


        QJsonDocument jdoc = data.toJsonDocument();

        auto jval = data.toJsonValue();

        if (jval.isNull() || jval.isUndefined())
            return QString();

/*
        if (jval.isObject())
        {
           //auto jobj = jval.toObject();
      //      (jobj.keys().contains()
            for (const QString &k : )
            {
                if (jobj.contains(k))
                {
                    if (job)
                    QDomElement part = hdoc.createElement("div");

            QJsonValue v = jdoc.value(k);
            if (v.isString())
                QString t = v.toString();

        }
*/
    QString title = jdoc["title"].toString();
    QString description = jdoc["description"].toString();
    QString image = jdoc["thumbnail"].toObject().value("source").toString();
    QString textExtract = jdoc["extract_html"].toString();
    QString linkToHtml = jdoc["content_urls"].toObject()
                                                .value("desktop").toObject()
                                                    .value("page").toString();

    QDomElement titlePart = hdoc.createElement("div");
    bodyroot.appendChild(titlePart);
    QDomElement tH2 = hdoc.createElement("h2");
    titlePart.appendChild(tH2);
    QDomText titleContent = hdoc.createTextNode(title);
    tH2.appendChild(titleContent) ;

    QDomElement descPart = hdoc.createElement("div");
    QDomElement dP = hdoc.createElement("p");
    QDomText descContent = hdoc.createTextNode(description);

    bodyroot.appendChild(descPart);
    descPart.appendChild(dP);
    dP.appendChild(descContent);

    QDomElement br = hdoc.createElement("br");
    descPart.appendChild(br);

    QDomElement img = hdoc.createElement("image");
    img.setAttribute("src", image);
    img.setAttribute("width", "20%") ;
    descPart.appendChild(img);    //res->deleteLater();


    QDomElement link = hdoc.createElement("a");
    link.setAttribute("href", linkToHtml);
    QDomText linkContent = hdoc.createTextNode("link to article...");
    link.appendChild(linkContent);
    descPart.appendChild(link);


    QDomElement extractPart = hdoc.createElement("div");
    bodyroot.appendChild(extractPart);

    QDomDocument eContent ;

    eContent.setContent(textExtract) ;
    qDebug() << eContent.toString();
    extractPart.appendChild (eContent);
    qDebug() << hdoc.toString();

    return  hdoc.toString();
}
