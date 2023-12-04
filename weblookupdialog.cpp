#include "weblookupdialog.h"
#include "ui_weblookupdialog.h"


#include <QMimeData>
#include <QEvent>

WebLookupDialog::WebLookupDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebLookupDialog),
    _display(nullptr),
    _state(UNKNOW),
    _clipboard(QApplication::clipboard())
{
    ui->setupUi(this);

    connect(_clipboard, &QClipboard::changed, this, &WebLookupDialog::prepareRequest);

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

void WebLookupDialog::prepareRequest(QClipboard::Mode)
{
    if (_clipboard->text().isEmpty())
        return;

    //if (ui->QueryTextEditField->text().length()
    //    && _clipboard->text() == ui->QueryTextEditField->text())

    QString searchText;
    const QMimeData *mimeData = _clipboard->mimeData();
    if (mimeData->hasHtml())
    {
        searchText= mimeData->html(); //_clipboard->text();
    }
    else if (mimeData->hasText())
    {
        searchText = mimeData->text();
    }
    else if (mimeData->hasImage())
    {
        //TODO
        //replace textEdit with a label to display image
        //ui->QueryTextEditField->
        //getPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
        //check  search api for image
    }
    else
    {
        return;
    }

    //if (searchText != _lastSearch.value())
    {
        ui->QueryTextEditField->setText(searchText);
        ui->QueryTextEditField->setClearButtonEnabled(true);
        setState(SET);
    }

    if (ui->QueryAllCheckBox->isChecked())
    {
        // TODO loop throught all urls , qquery each, appending or tabbing the result in display
    }
    else
    {
        QString url = ui->UrlSelectorField->currentText();
        QUrl reqUrl= QUrl(url);
        reqUrl.setQuery(searchText);

        bool isApi = ui->UrlSelectorField->currentText().contains("api");


        connect(ui->StartRequestButton, &QAbstractButton::clicked,
            this, [this, reqUrl, isApi] () {
                startNewRequest(reqUrl, isApi);
            }
        );

        setState(READY);
        ui->StartRequestButton->setEnabled(true);
    }
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
