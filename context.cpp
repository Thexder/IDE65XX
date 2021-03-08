#include "context.h"
#include "common.h"
#include "debugutil.h"

Context::Context()
    : QObject()
{
    scanPattern = QRegularExpression(QStringLiteral("(//|/\\*|\\*/|[\\.#]import|.const|.macro|.label|\\w+:)"));

    importPreprocessorPattern = QRegularExpression(QStringLiteral("#import(?:if)?\\s+([\"'])([^\\1]+)\\1"));
    importDirectivePattern = QRegularExpression(QStringLiteral("\\.import\\s+source[^\"']+([\"'])([^\\1]+?)\\1"));
    constantDirectivePattern = QRegularExpression(QStringLiteral("\\.const\\s+([\\w]+)[= ]"));
    labelDirectivePattern = QRegularExpression(QStringLiteral("(?:\\.label\\s+)?(\\w+)(?:\\:)?"));
    macroDirectivePattern = QRegularExpression(QStringLiteral("\\.macro\\s+(\\w+)[( ]"));

    singleLineCommentPattern = QRegularExpression(QStringLiteral("//[^\n]*(?#single-comment)"));
    multiLineCommentPattern = QRegularExpression(QStringLiteral("/\\*.*?\\*/(?#single-comment)"));
}


void Context::addMatches(QString line, int& lineNumber, int offset, QRegularExpression pattern, QMultiMap<QString, int>& definition) {
    // constant found
    QRegularExpressionMatchIterator matches = pattern.globalMatch(line, offset);
    if (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        definition.insert(match.captured(1), lineNumber);
        if (doLog) qDebug("    found=%s", qPrintable(match.captured(1)));
    }
}

void Context::scan(Source& source) {

    QFile file(source.file);

    if (doLog) qDebug("file=%s exists=%d", qPrintable(file.fileName()), file.exists());

    if(file.open(QIODevice::ReadOnly | QIODevice::Text )) {
        int counter = -1;
        QTextStream stream(&file);
        bool ignoreSection = false;
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            counter++;
            // sections of comments and no end in sight
            if (line.isEmpty() || (ignoreSection && line.indexOf("*/")==-1)) {
                // skip line
                continue;
            }
            bool ignore = ignoreSection;
            QRegularExpressionMatchIterator scans = scanPattern.globalMatch(line);
            while (scans.hasNext()) {
                QRegularExpressionMatch scan = scans.next();
                if (!ignore && scan.captured(1) == "//") {
                    // single line comment - skip line
                    break;
                } else if (!ignore && scan.captured(1) == "/*") {
                    // starting multi line comment
                    ignore = true;
                    ignoreSection = true;
                } else if (ignore && scan.captured(1) == "*/") {
                    // ending multi line comment
                    ignore = false;
                    ignoreSection = false;
                } else {
                    if (doLog) qDebug("  #%d [%s]", counter, qPrintable(line));
                    if (scan.captured(1).endsWith("import")) {
                        // import found
                        QRegularExpressionMatchIterator imports = QRegularExpressionMatchIterator();
                        if (scan.captured(1) == "#import") {
                            imports = importPreprocessorPattern.globalMatch(line, scan.capturedStart(1));
                        } else if (scan.captured(1) == ".import") {
                            imports = importDirectivePattern.globalMatch(line, scan.capturedStart(1));
                        }
                        if (imports.hasNext()) {
                            QRegularExpressionMatch import = imports.next();
                            QString relativeFile = import.captured(2).replace("./", "/");
                            if (doLog) qDebug("    import found=%s", qPrintable(relativeFile));
                            if (!context.contains(relativeFile) && !queue.contains(relativeFile)) {
                                source.imports.append(relativeFile);
                                queue.append(relativeFile);
                                if (doLog) qDebug("      added to queue file=%s", qPrintable(relativeFile));
                            }
                        }
                    } else if (scan.captured(1).endsWith("const")) {
                        // constant found
                        addMatches(line, counter, scan.capturedStart(1), constantDirectivePattern, source.global.consts);
                    } else if (scan.captured(1).endsWith("label") || scan.captured(1).endsWith(":")) {
                        // label found
                        addMatches(line, counter, scan.capturedStart(1), labelDirectivePattern, source.global.labels);
                    } else if (scan.captured(1).endsWith("macro")) {
                        // macro found
                        addMatches(line, counter, scan.capturedStart(1), macroDirectivePattern, source.global.macros);
                    }
                }
            }
        }
    }
}

void Context::startWith(QString workspacePath, QString relativeFile) {

    doLog = true;

    context.clear();

    qDebug("startWith=%s workspace=%s", qPrintable(relativeFile), qPrintable(workspacePath));

    queue.append(relativeFile);

    while (!queue.empty()) {
        Source source;
        QString relativeFile = queue.dequeue();
        QString filePath = (workspacePath + relativeFile).toLower();
        if (cache.contains(filePath)) {
            source = cache.value(filePath);
        } else {
            source = Source();
            source.relativeFile = relativeFile;
            source.file = filePath;
            scan(source);
            cache.insert(filePath, source);
        }
        context.insert(filePath, source);
    }
    qDebug("done");
    QMapIterator<QString, Source> i(context);
    while (i.hasNext()) {
        i.next();
        Source source = i.value();
        qDebug("context '%s' {", qPrintable(source.file));
        if (!source.imports.isEmpty()) qDebug("   %s=[%s]", "imports", qPrintable(DebugUtil::toString(source.imports)));
        if (!source.global.consts.isEmpty()) qDebug("   %s=[%s]", "constants", qPrintable(DebugUtil::toString(source.global.consts)));
        if (!source.global.labels.isEmpty()) qDebug("   %s=[%s]", "labels", qPrintable(DebugUtil::toString(source.global.labels)));
        if (!source.global.macros.isEmpty()) qDebug("   %s=[%s]", "macros", qPrintable(DebugUtil::toString(source.global.macros)));
        qDebug("}");
    }
}

Context::Source Context::getSource(QString filePath) {
    Source source;
    QString key = filePath.toLower();
    qDebug("lookup source=%s", qPrintable(key));
    if (context.contains(key)) {
        qDebug("success %s", qPrintable(key));
        QMultiMap<QString, Source>::iterator i = context.find(key);
        while (i != context.end() && i.key() == key) {
            qDebug("found source=%s", qPrintable(DebugUtil::toString(i.value())));
            return i.value();
        }
    }
    return source;
}
