#ifndef DEBUGUTIL_H
#define DEBUGUTIL_H

#include "common.h"
#include "context.h"

class DebugUtil : public QObject
{
    Q_OBJECT

public:
    DebugUtil();

    static QString toString(Context::Source source);
    static QString toString(QMultiMap<QString, int> multiMap);
    static QString toString(QMultiMap<int, QStringList> multiMap);
    static QString toString(QStringList stringList);
    static QString toString(QSet<QString> set);
};

#endif // DEBUGUTIL_H
