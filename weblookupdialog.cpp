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
    _state(LookStatus::UNKNOW),
    _clipboard(QApplication::clipboard())
{
    ui->setupUi(this);

    // TODO replace with Settings class
    _urlList.insert("Google", DEFAULTSEARCH_1);
    _urlList.insert("Google2", DEFAULTSEARCH_2);
    /* fill the combo box with the list of label/url to use */
    for (const auto k : _urlList.keys())
    {
        ui->UrlSelectorField->addItem(k);
    }

    // We want to catch when content of the clipboard changed (text selection or copy)
    // and trigger an update to the query in the widget to prepare a search
    //connect(_clipboard, &QClipboard::dataChanged, this, &WebLookupDialog::prepareRequest);
    //connect(_clipboard, &QClipboard::selectionChanged, this, &WebLookupDialog::prepareRequest);
    connect(_clipboard, &QClipboard::changed, this, &WebLookupDialog::prepareRequest);

    ui->StartRequestButton->setEnabled(false);
    setState(LookStatus::IDLE);
}

WebLookupDialog::~WebLookupDialog()
{
    disconnect(_clipboard);
    disconnect(ui->StartRequestButton);

    _clipboard = nullptr;
    delete _display;
    delete ui;
}


WebLookupDialog::RequestSetup WebLookupDialog::requestSetup(const QString &label)
{

}

void WebLookupDialog::prepareRequest(QClipboard::Mode m)
{
    const QRegularExpression supportedTypes(QString(SUPPORTED)
                                                        .replace(QString(", "), QChar('|'))
                                                , QRegularExpression::CaseInsensitiveOption); // "text/plain, text/html" => QRegExp("text/plain|text/html")

    if (!_clipboard->mimeData(m)->formats().contains(supportedTypes))
        return;

    /* We only accept text mime type so far, so no need to check forr and to add image mime type logics */
    if (!_clipboard->mimeData(m)->hasText() || !_clipboard->text(m).trimmed().length())
        return;

    QString searchText = _clipboard->text(m).trimmed();/*
    not used simplified() too keep the "pattern/struct" of the string */

    /* UTF8 strings from different/unknow origin should be normalized for compared */
    QString::NormalizationForm  nf(QString::NormalizationForm_C);
    if (ui->QueryTextEditField->text().trimmed().toLower() == searchText.toLower()
        || ui->QueryTextEditField->text().trimmed().toLower().normalized(nf) == searchText.toLower().normalized(nf))
        return;     // no change no search ,        reqUrl.setQuery(searchText);


#if (DEBUG)

    qDebug() << "previous text :" << ui->QueryTextEditField->text().trimmed().toLower();
    qDebug() << "new text :" << searchText.toLower();
    qDebug() << "normalized form :" << nf;
    qDebug() << "previous text normalized :" << ui->QueryTextEditField->text().trimmed().toLower().normalized(nf);
    qDebug() << "new text normalized form :" << searchText.toLower().normalized(nf);

    qDebug() << "MimeData to text : ";
    qDebug() << "MimeData formats contained/valid for the clipboard content :\n"<<  _clipboard->mimeData()->formats();
    qDebug() << "Text/plain conversion of these  Types " << _clipboard->text(m);

#endif

    /* TODO
    image suport replace textEdit with a label to display image,
    else if (mimeData->hasImage()) { getPixmap(qvariant_cast<QPixmap>(mimeData->imageData())); }
    else if (mimeData->hasUrls()) or just else ...>?    */

    ui->QueryTextEditField->setText(searchText);
    ui->QueryTextEditField->setClearButtonEnabled(true);
    setState(LookStatus::SET);

    if (!ui->QueryAllCheckBox->isChecked())
    {
        QString key = ui->UrlSelectorField->currentText();
        QUrl reqUrl= QUrl(_urlList.value(key).arg(searchText));

        bool isApi = ui->UrlSelectorField->currentText().contains("api");
        // TODO this is not consistent API hint, need better stuff, might come from settings

        connect(ui->StartRequestButton, &QAbstractButton::clicked,
            this, [this, searchText, key, reqUrl, isApi] () {

                _lastSearch.first = searchText;
            _lastSearch.second = QStringList({key});
                startNewRequest(reqUrl, isApi);
                return  ;
            }
        );

        setState(LookStatus::READY);
        ui->StartRequestButton->setEnabled(true);    
    }
    /* TODO else block ui->QueryAllCheckBox->isChecked()) == true:
     loop throught all urls , qquery each, appending or tabbing the result in display */

    return;
}

void WebLookupDialog::startNewRequest(const QUrl &url, const bool isApi)
{
    setState(LookStatus::LOADING);
    ui->StartRequestButton->setEnabled(false);


    _display = new WebContentDisplayWidget(this);
    connect(&_display->_view, &QWebEngineView::loadFinished, this, &WebLookupDialog::requestEnded);
    connect(_display, &WebContentDisplayWidget::destroyed, this, &WebLookupDialog::displayClosed);
    _display->startRequest(url, isApi);

    return;
}

void WebLookupDialog::requestEnded()
{
    setState(LookStatus::DONE);
    return;
}


void WebLookupDialog::displayClosed()
{
    _display = nullptr;
}
