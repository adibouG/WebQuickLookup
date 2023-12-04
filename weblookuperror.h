/***********************************************************************
 *
 * WebLookupError:
 *
 * Error Object provide customized error message, handling and infos
 ***********************************************************************/

#ifndef WEBLOOKUPERROR_H
#define WEBLOOKUPERROR_H

#include <QString>

class WebLookupError
{

public:
    WebLookupError(const QString &e, const QString from = __FUNCTION__)
        : _mess(e), _ctx(from) { displayErrMess(); }

    void displayErrMess() { qDebug() << QString("Error: %1\nIn: %2").arg(_mess, _ctx); }

    QString  _mess;
    QString  _ctx;

};


#endif // WEBLOOKUPERROR_H
