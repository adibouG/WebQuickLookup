#ifndef WEBPAGEVIEW_H
#define WEBPAGEVIEW_H

#include <QWebEngineView>

class WebPageView: public QWebEngineView
{
    Q_GADGET

public:

    WebPageView();
    void appendPageContent(const QWebEnginePage* _page);


private:

    QWebEngineView _viewer;
};

#endif // WEBPAGEVIEW_H
