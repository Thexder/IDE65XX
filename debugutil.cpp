#include "debugutil.h"
#include <QMapIterator>
#include <QSetIterator>


DebugUtil::DebugUtil()
    : QObject()
{
}

QString DebugUtil::toString(Context::Source source) {
    QByteArray toString;
    toString.append("source file=");
    toString.append(qPrintable(source.file));
    toString.append(" {\n");
    if (!source.imports.isEmpty()) {
        toString.append("   imports=[");
        toString.append(qPrintable(DebugUtil::toString(source.imports)));
        toString.append("]\n");
    }
    if (!source.global.consts.isEmpty()) {
        toString.append("   constants=[");
        toString.append(qPrintable(DebugUtil::toString(source.global.consts)));
        toString.append("]\n");
    }
    if (!source.global.labels.isEmpty()) {
        toString.append("   labels=[");
        toString.append(qPrintable(DebugUtil::toString(source.global.labels)));
        toString.append("]\n");
    }
    if (!source.global.macros.isEmpty()) {
        toString.append("   macros=[");
        toString.append(qPrintable(DebugUtil::toString(source.global.macros)));
        toString.append("]\n");
    }
    toString.append("}");
    return toString;
}

QString DebugUtil::toString(QMultiMap<QString, int> multiMap) {

    QByteArray toString;
    QMapIterator<QString, int> i(multiMap);
    while (i.hasNext()) {
        QMapIterator<QString, int>::Item item = i.next();
        toString.append(qPrintable(item.key()));
        toString.append("=");
        toString.append(QString::number(item.value()).toUtf8());
        toString.append(",");
    }
    toString.truncate(toString.size()-1);
    return toString;
}

QString DebugUtil::toString(QMultiMap<int, QStringList> multiMap) {

    QByteArray toString;
    QMapIterator<int, QStringList> i(multiMap);
    while (i.hasNext()) {
        QMapIterator<int, QStringList>::Item item = i.next();
        if (item.value().size() > 0) {
            toString.append(QString::number(item.key()).toUtf8());
            toString.append("={");
            QStringListIterator l(item.value());
            while (l.hasNext()) {
                toString.append(qPrintable(l.next()));
                toString.append(",");
            }
            toString.truncate(toString.size()-1);
            toString.append("},");
        }
    }
    if (toString.size() >= 1) {
        toString.truncate(toString.size()-1);
    }
    return toString;
}

QString DebugUtil::toString(QStringList stringList) {

    QByteArray toString;
    for (QStringList::Iterator S =  stringList.begin(); S != stringList.end(); S++) {
        toString.append(qPrintable(*S));
        toString.append(",");
    }
    toString.truncate(toString.size()-1);
    return toString;
}

QString DebugUtil::toString(QSet<QString> set) {

    QByteArray toString;
    QSetIterator<QString> i(set);
    while (i.hasNext()) {
        QString item = i.next();
        toString.append(qPrintable(item));
        toString.append(",");
    }
    toString.truncate(toString.size()-1);
    return toString;
}
