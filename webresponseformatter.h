#ifndef WEBRESPONSEFORMATTER_H
#define WEBRESPONSEFORMATTER_H

#include <QObject>

class WebResponseFormatter : public QObject
{
    Q_OBJECT

    enum class Error
    {
        UNKNOWN,
        INPUTTYPE,
        PROCESSING,
    };

public:
    explicit WebResponseFormatter(QObject *parent = nullptr) : QObject(parent) {}
    ~WebResponseFormatter() {}

    static const QString format(const QVariant &d, const QString &mimeType, const QStringList &keyList);

signals:
    void formattingEnded(const QString &s);
    void formattingError(const QString &s);

private:
    QString variantParser(const QVariant &d);
};

#endif // WEBRESPONSEFORMATTER_H
