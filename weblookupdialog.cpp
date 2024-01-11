#include "weblookupdialog.h"
#include "ui_weblookupdialog.h"

#include "webcontentdisplaywidget.h"
#include <QMimeData>
#include <QEvent>

#define DEBUG 1
#define DEFAULTSEARCH_1 "https://google.com/search?output=toolbar&q=%1"
#define DEFAULTSEARCH_2 "https://en.wikipedia.org/api/rest_v1/page/summary/%1"
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
    Settings::LookRequestSetting s1("Google", QString(DEFAULTSEARCH_1));
    settingTest << s1;
    Settings::LookRequestSetting s2("Wikipedia", QString(DEFAULTSEARCH_2), true);
    settingTest << s2;

    // /* fill the combo box with the list of label/url to use */
    for (const auto &s : settingTest)
    {
        ui->UrlSelectorField->addItem(s.label);
    }

    // We want to catch when content of the clipboard changed (text selection or copy)
    // and trigger an update to the query in the widget to prepare a search

    connect(_clipboard, &QClipboard::changed, this, &WebLookupDialog::prepareRequest);
    ui->StartRequestButton->setEnabled(false);
    setState(LookStatus::IDLE);
}

WebLookupDialog::~WebLookupDialog()
{
    disconnect(_clipboard);
    disconnect(ui->StartRequestButton);

    _clipboard = nullptr;
    if (_display)  delete _display;
    _display = nullptr;
    delete ui;
}


SearchRequest WebLookupDialog::requestSetup(const QString &search, Settings::LookRequestSetting set)
{
    SearchRequest req;
    req.label = set.label;
    req.api = set.api;
    req.word = search;
    req.url = QUrl(set.url.arg(search));
    req.keys = set.keys;
    return req;
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

    qDebug() << "MimeData to text : ";
    qDebug() << "MimeData formats contained/valid for the clipboard content :\n" <<  _clipboard->mimeData()->formats();
    qDebug() << "Text/plain conversion of these  Types " << _clipboard->text(m);

#endif

    /* TODOWebContentDisplayWidget
    image suport replace textEdit with a label to display image,
    else if (mimeData->hasImage()) { getPixmap(qvariant_cast<QPixmap>(mimeData->imageData())); }
    else if (mimeData->hasUrls()) or just else ...>?    */
    ui->QueryTextEditField->clear();
    ui->QueryTextEditField->setText(searchText);
    ui->QueryTextEditField->setClearButtonEnabled(true);
    setState(LookStatus::SET);



    connect(ui->StartRequestButton, &QAbstractButton::clicked, this, &WebLookupDialog::startNewRequest);

    setState(LookStatus::READY);
    ui->StartRequestButton->setEnabled(true);

    /* TODO else block ui->QueryAllCheckBox->isChecked()) == true:
     loop throught all urls , qquery each, appending or tabbing the result in display */

    return;
}

void WebLookupDialog::startNewRequest(bool append = false )
{
    Settings::LookRequestSetting keySetting = settingTest.at(ui->UrlSelectorField->currentIndex());
    qDebug() << "selected index: " << ui->UrlSelectorField->currentIndex();
    qDebug() << "key to use: " << keySetting.label;
    qDebug() << "Url to use: " << keySetting.url;

    SearchRequest s = requestSetup(ui->QueryTextEditField->text(), keySetting);



    qDebug() << "url :" << s.url.toString() ;

//    ui->QueryTextEditField->setEnabled(false);
    if (!_display)
    {
        //_display->viewer()->createWindow(QWebEnginePage::WebBrowserTab);
        _display = new WebContentDisplayWidget(nullptr);
    }
    if (!append)
    {
        _lastSearch.clear();
    }

    connect(_display->viewer(), &QWebEngineView::loadFinished, this, &WebLookupDialog::requestEnded);
    connect(_display, &WebContentDisplayWidget::destroyed, this, &WebLookupDialog::displayClosed);
//    connect(_display, &WebContentDisplayWidget::error, this, &WebLookupDialog::displayClosed);

    setState(LookStatus::LOADING);
    ui->QueryTextEditField->setReadOnly(true);
    ui->StartRequestButton->setEnabled(false);

    _lastSearch << s;

    _display->setAppendResult(append);
    _display->startRequest(s);

    return;
}

void WebLookupDialog::requestEnded()
{
    setState(LookStatus::DONE);
    ui->QueryTextEditField->setReadOnly(false);

    connect(ui->UrlSelectorField, &QComboBox::currentIndexChanged, this, &WebLookupDialog::appendSearch);
    return;
}

void WebLookupDialog::appendSearch(int index)
{
    setState(LookStatus::SET);
    auto prev = _lastSearch.last();
    auto word = ui->QueryTextEditField->text();
    auto reqSet  = requestSetup (word,  settingTest.at(index));

    if (prev.word != word)
    {
        startNewRequest(false);
    }
    else if (reqSet.url != prev.url)
    {
        startNewRequest(true); // TODO: the append result in the same page or in a tab
    }
    return;
}



void WebLookupDialog::displayClosed()
{

    _display = nullptr;
}
