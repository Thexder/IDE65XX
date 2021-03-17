#ifndef CONTEXT_H
#define CONTEXT_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QQueue>


class Context : public QObject
{
    Q_OBJECT

public:
    Context();

    // follows the sourcefile-imports starting with given file
    void startWith(QString workspacePath, QString filePath);

    /* definitions for constants, labels and macros */
    struct Definition {
        QMultiMap<QString, int> consts;
        QMultiMap<QString, int> labels;
        QMultiMap<QString, int> macros;
    };
    /* a source file with its imports and definitions */
    struct Source
    {
        // like it is related to in source (i.e. ./lib/mag.asm)
        QString relativeFile;
        // ordinary file (i.e. D:/intro/lib/mag.asm)
        QString file;

        // list of imports (relativeFile)
        QStringList imports;

        Definition global;
        Definition local;
    };

    Source getSource(QString relativeFile);
    Source scanLine(QString line, int lineNumber, bool blockCommentActive);


private:

    void scan(Source& source);
    bool scanLine(QString line, int lineNumber, bool blockCommentActive, Source& source);
    void addMatches(QString line, int& lineNumber, int offset, QRegularExpression pattern, QMultiMap<QString, int>& definition);

    QRegularExpression scanPattern;

    QRegularExpression importPreprocessorPattern;
    QRegularExpression importDirectivePattern;
    QRegularExpression constantDirectivePattern;
    QRegularExpression labelDirectivePattern;
    QRegularExpression macroDirectivePattern;
    QRegularExpression singleLineCommentPattern;
    QRegularExpression multiLineCommentPattern;

    // cache of sources (key = filePath)
    QMultiMap<QString, Source> cache;
    // project context (key = filePath)
    QMultiMap<QString, Source> context;
    QQueue<QString> queue;

    bool doLog = false;
};

#endif // CONTEXT_H
