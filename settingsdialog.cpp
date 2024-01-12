#include "settingsdialog.h"

#include "ui_settingsdialog.h"

#include <QTableWidget>
#include <QTableWidgetItem>

#define DEFAULTSEARCH_1 "https://google.com/search?output=toolbar&q=%1"
#define DEFAULTSEARCH_2 "https://en.wikipedia.org/api/rest_v1/page/summary/%1"
#define SUPPORTED "text/plain"
#define DEBUG 1

typedef  QList<QTableWidgetItem>  TableWidgetRow;

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::SettingsDialog)
{


    QList<QTableWidgetItem> defaultUrlRow1;

    QTableWidgetItem item1_1 = QTableWidgetItem(QString("Google"));
    QTableWidgetItem item1_2 = QTableWidgetItem(QString(DEFAULTSEARCH_1));
    defaultUrlRow1.append(item1_1);
    defaultUrlRow1.append(item1_2);

    QList<QTableWidgetItem> defaultUrlRow2;
    QTableWidgetItem item2_1 = QTableWidgetItem(QString("WIKI"));
    QTableWidgetItem  item2_2 = QTableWidgetItem(QString(DEFAULTSEARCH_2));
    defaultUrlRow2.append(item2_1);
    defaultUrlRow2.append(item2_2);

    ui->setupUi(this);

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
