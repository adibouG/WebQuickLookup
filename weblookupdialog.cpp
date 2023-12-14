#include "weblookupdialog.h"
#include "ui_weblookupdialog.h"

#include <QMimeData>
#include <QEvent>

#define DEBUG 1
#define DEFAULTSEARCH_1 "http://google.com/complete/search?output=toolbar&q=%1"
#define DEFAULTSEARCH_2 "http://google.com/complete/search?output=toolbar&q=%1"
#define SUPPORTED "text/plain"

WebLookupDialog::WebLookupDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebLookupDialog),
    _display(nullptr),
    _state(UNKNOW),
    _clipboard(QApplication::clipboard())
{
    ui->setupUi(this);


    _urlList.insert("Google", DEFAULTSEARCH_1);
    _urlList.insert("Google2", DEFAULTSEARCH_2);

    // We want to catch when content of the clipboard changed (text selection or copy)
    // and trigger an update to the query in the widget to prepare a search
    //connect(_clipboard, &QClipboard::dataChanged, this, &WebLookupDialog::prepareRequest);
    //connect(_clipboard, &QClipboard::selectionChanged, this, &WebLookupDialog::prepareRequest);
    connect(_clipboard, &QClipboard::changed, this, &WebLookupDialof og::prepareRequest);

    ui->StartRequestButton->setEnabled(false);
    setState(IDLE);
}

WebLookupDialog::~WebLookupDialog()
{
    disconnect(_clipboard);
    disconnect(ui->StartRequestButton);

    _clipboard = nullptr;
    delete _display;
    delete ui;
}

void WebLookupDialog::prepareRequest(QClipboard::Mode m)
{
    const QRegularExpression supportedTypes(QString(SUPPORTED)
                                                    .replace(QChar(', '), QChar('|'))
                                                    , QRegularExpression::CaseInsensitiveOption);
    // "text/plain, text/html" => QRegExp("text/plain|text/html")

    if (!_clipboard->mimeData(m)->formats().contains(supportedTypes))
        return;

    /* We only accept text mime type so far, so no need to check forr and to add image mime type logics */
    if (!_clipboard->mimeData(m)->hasText() || !_clipboard->text(m).trimmed().length())
        return;

    QString searchText = _clipboard->text(m).trimmed();/*
    not used simplified() too keep the "pattern/struct" of the string */

    QString::NormalizationForm nf(QString::NormalizationForm_C);    /*
    to compare string from different sources/origine in UTF8
    normalized can help when using the correct form of course to get better results
    no change no need to search ,                                   */
    if (ui->QueryTextEditField->text().trimmed().normalized(nf).toLower() == searchText.normalized(nf).toLower())
        return;

    if (DEBUG)
    {
        qDebug() << "MimeData formats contained/valid for the clipboard content :\n"<<  _clipboard->mimeData()->formats();
        qDebug() << "Text/plain conversion of these  Types " << _clipboard->text(m);
    }
    /*
    else if (mimeData->hasImage()) { //TODO ... replace textEdit with a label to display image, getPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    else if (mimeData->hasUrls()) or just else ...>?
    */
    ui->QueryTextEditField->setText(searchText);
    ui->QueryTextEditField->setClearButtonEnabled(true);
    setState(SET);

    if (!ui->QueryAllCheckBox->isChecked())
    {
        QString url = ui->UrlSelectorField->currentText();
        QUrl reqUrl= QUrl(url);
        reqUrl.setQuery(searchText);

        bool isApi = ui->UrlSelectorField->currentText().contains("api");
        // TODO this is not consistent trigger API hint, need better stuff

        connect(ui->StartRequestButton, &QAbstractButton::clicked,
            this, [this, reqUrl, isApi] () {
                startNewRequest(reqUrl, isApi);
                return  ;
            }
        );

        setState(READY);
        ui->StartRequestButton->setEnabled(true);    
    }
    /* TODO else block ui->QueryAllCheckBox->isChecked()) == true:
     loop throught all urls , qquery each, appending or tabbing the result in display */

    return;
}

void WebLookupDialog::startNewRequest(const QUrl &url, const bool isApi)
{
    setState(LOADING);
    ui->StartRequestButton->setEnabled(false);

    _lastSearch.setValue(url.toString());

    _display = new WebContentDisplayWidget(this);
    connect(&_display->_view, &QWebEngineView::loadFinished, this, &WebLookupDialog::requestEnded);
    connect(_display, &WebContentDisplayWidget::destroyed, this, &WebLookupDialog::displayClosed);
    _display->startRequest(url, isApi);

    return;
}

void WebLookupDialog::requestEnded()
{
    setState(DONE);
    return;
}


void WebLookupDialog::displayClosed()
{
    _display = nullptr;
}
