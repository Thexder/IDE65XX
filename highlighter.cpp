#include "highlighter.h"
#include "common.h"
#include "debugutil.h"

bool logActive = true;
bool doLog = logActive;

Highlighter::Highlighter(Context *assemblyContext, QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QUrl url = parent->baseUrl();
    if (doLog) qDebug("file=%s", qPrintable(url.url()));

    context = assemblyContext;

    if (doLog) qDebug("seeking %s", qPrintable(url.url()));
    Context::Source source = context->getSource(url.url());
    if (doLog) qDebug("got %s", qPrintable(DebugUtil::toString(source)));

    QSettings settings(Common::appConfigDir()+"/settings.ini", QSettings::IniFormat);

    if (document()) {
        disconnect(document(), SIGNAL(contentsChanged()), this, SLOT(validateStructureChanges()));
    }
    if (parent) {
        // used as a trigger to make sure that after highlighting lines the check for full rehighlight happens
        connect(parent, SIGNAL(contentsChanged()), this, SLOT(validateStructureChanges()));
    }

    // application style
    bool styleIsCustom = settings.value("ApplicationStyle").toInt() == 5;
    References references;
    HighlightingRule rule;

    cmdFormat.setForeground((styleIsCustom)? settings.value("CustomOpcode", QColor(0x61, 0x61, 0xd0)).value<QColor>() : QColor(0x61, 0x61, 0xd0));
    cmdFormat.setFontWeight(QFont::Bold);
    const QString cmdPatterns[] = {
        // standart 6502 mnemonics
        QStringLiteral("\\badc\\b"), QStringLiteral("\\band\\b"), QStringLiteral("\\basl\\b"),
        QStringLiteral("\\bbcc\\b"), QStringLiteral("\\bbcs\\b"), QStringLiteral("\\bbeq\\b"),
        QStringLiteral("\\bbit\\b"), QStringLiteral("\\bbmi\\b"), QStringLiteral("\\bbne\\b"),
        QStringLiteral("\\bbpl\\b"), QStringLiteral("\\bbrk\\b"), QStringLiteral("\\bbvc\\b"),
        QStringLiteral("\\bbvs\\b"), QStringLiteral("\\bclc\\b"), QStringLiteral("\\bcld\\b"),
        QStringLiteral("\\bcli\\b"), QStringLiteral("\\bclv\\b"), QStringLiteral("\\bcmp\\b"),
        QStringLiteral("\\bcpx\\b"), QStringLiteral("\\bcpy\\b"), QStringLiteral("\\bdec\\b"),
        QStringLiteral("\\bdex\\b"), QStringLiteral("\\bdey\\b"), QStringLiteral("\\beor\\b"),
        QStringLiteral("\\binc\\b"), QStringLiteral("\\binx\\b"), QStringLiteral("\\biny\\b"),
        QStringLiteral("\\bjmp\\b"), QStringLiteral("\\bjsr\\b"), QStringLiteral("\\blda\\b"),
        QStringLiteral("\\bldx\\b"), QStringLiteral("\\bldy\\b"), QStringLiteral("\\blsr\\b"),
        QStringLiteral("\\bnop\\b"), QStringLiteral("\\bora\\b"), QStringLiteral("\\bpha\\b"),
        QStringLiteral("\\bphp\\b"), QStringLiteral("\\bpla\\b"), QStringLiteral("\\bplp\\b"),
        QStringLiteral("\\brol\\b"), QStringLiteral("\\bror\\b"), QStringLiteral("\\brti\\b"),
        QStringLiteral("\\brts\\b"), QStringLiteral("\\bsbc\\b"), QStringLiteral("\\bsec\\b"),
        QStringLiteral("\\bsed\\b"), QStringLiteral("\\bsei\\b"), QStringLiteral("\\bsta\\b"),
        QStringLiteral("\\bstx\\b"), QStringLiteral("\\bsty\\b"), QStringLiteral("\\btax\\b"),
        QStringLiteral("\\btay\\b"), QStringLiteral("\\btsx\\b"), QStringLiteral("\\btxa\\b"),
        QStringLiteral("\\btxs\\b"), QStringLiteral("\\btya\\b"),
        // illegal 6502 mnemonics
        QStringLiteral("\\bahx\\b"), QStringLiteral("\\balr\\b"), QStringLiteral("\\banc\\b"),
        QStringLiteral("\\banc2\\b"), QStringLiteral("\\barr\\b"), QStringLiteral("\\baxs\\b"),
        QStringLiteral("\\bdcp\\b"), QStringLiteral("\\bisc\\b"), QStringLiteral("\\blas\\b"),
        QStringLiteral("\\blax\\b"), QStringLiteral("\\brla\\b"), QStringLiteral("\\brra\\b"),
        QStringLiteral("\\bsax\\b"), QStringLiteral("\\bsbc2\\b"), QStringLiteral("\\bshx\\b"),
        QStringLiteral("\\bshy\\b"), QStringLiteral("\\bslo\\b"), QStringLiteral("\\bsre\\b"),
        QStringLiteral("\\btas\\b"), QStringLiteral("\\bxaa\\b"), QStringLiteral("\\bsbx\\b"),
        QStringLiteral("\\bsha\\b"),
        // dtv mnemonics
        QStringLiteral("\\bbra\\b"), QStringLiteral("\\bsac\\b"), QStringLiteral("\\bsir\\b"),
        // 65c02 mnemonics
        QStringLiteral("\\bbbr0\\b"), QStringLiteral("\\bbbr1\\b"), QStringLiteral("\\bbbr2\\b"),
        QStringLiteral("\\bbbr3\\b"), QStringLiteral("\\bbbr4\\b"), QStringLiteral("\\bbbr5\\b"),
        QStringLiteral("\\bbbr6\\b"), QStringLiteral("\\bbbr7\\b"), QStringLiteral("\\bbbs0\\b"),
        QStringLiteral("\\bbbs1\\b"), QStringLiteral("\\bbbs2\\b"), QStringLiteral("\\bbbs3\\b"),
        QStringLiteral("\\bbbs4\\b"), QStringLiteral("\\bbbs5\\b"), QStringLiteral("\\bbbs6\\b"),
        QStringLiteral("\\bbbs7\\b"), QStringLiteral("\\bphx\\b"), QStringLiteral("\\bphy\\b"),
        QStringLiteral("\\bplx\\b"), QStringLiteral("\\bply\\b"), QStringLiteral("\\brmb0\\b"),
        QStringLiteral("\\brmb1\\b"), QStringLiteral("\\brmb2\\b"), QStringLiteral("\\brmb3\\b"),
        QStringLiteral("\\brmb4\\b"), QStringLiteral("\\brmb5\\b"), QStringLiteral("\\brmb6\\b"),
        QStringLiteral("\\brmb7\\b"), QStringLiteral("\\bsmb0\\b"), QStringLiteral("\\bsmb1\\b"),
        QStringLiteral("\\bsmb2\\b"), QStringLiteral("\\bsmb3\\b"), QStringLiteral("\\bsmb4\\b"),
        QStringLiteral("\\bsmb5\\b"), QStringLiteral("\\bsmb6\\b"), QStringLiteral("\\bsmb7\\b"),
        QStringLiteral("\\bstp\\b"), QStringLiteral("\\bstz\\b"), QStringLiteral("\\btrb\\b"),
        QStringLiteral("\\btsb\\b"), QStringLiteral("\\bwai\\b")
    };

    for (const QString &patternCmd : cmdPatterns) {
        rule.pattern = QRegularExpression(patternCmd, QRegularExpression::CaseInsensitiveOption);
        rule.format = cmdFormat;
        strictHighlightingRules.append(rule);
    }

    // number
    numberFormat.setForeground((styleIsCustom)? settings.value("CustomNumber", QColor(0x00, 0x80, 0x80)).value<QColor>() : Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral("\\$[0-9a-fA-F]+|\\%[01]+|(?<=\\W)[0-9]+"));
//    rule.pattern = QRegularExpression(QStringLiteral("(?:\\$[0-9a-fA-F]*)|(?:\\#\\$[0-9a-fA-F]*)|(?:\\#\\%[01]*)|(?:\\%[01]*)|(?:\\#[0-9]*)|(?:\\ [0-9]*)|(?:\\([0-9]*\\))"));
    rule.format = numberFormat;
    strictHighlightingRules.append(rule);
    // end of number

    // function/macro
    functionFormat.setFontItalic(true);
    functionFormat.setForeground((styleIsCustom)? settings.value("CustomFunction", QColor(0xaf, 0x64, 0x00)).value<QColor>() : QColor(0xaf, 0x64, 0x00));
    functionFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    strictHighlightingRules.append(rule);
    // end of function

    // label references
    references.definitionFormat.setFontWeight(QFont::ExtraBold);
    references.definitionFormat.setFontItalic(true);
    references.definitionFormat.setForeground((styleIsCustom)? settings.value("CustomLabel", QColor(0xc3, 0x34, 0x34)).value<QColor>() : QColor(0xc3, 0x34, 0x34));
    references.definitions.append(QRegularExpression(QStringLiteral("(\\w+):(?#label)")));
    references.definitions.append(QRegularExpression(QStringLiteral("\\.label\\s+([\\w]+)[= ](?#label)")));
    references.referenceFormat.setFontWeight(QFont::Normal);
    references.referenceFormat.setFontItalic(true);
    references.referenceFormat.setForeground((styleIsCustom)? settings.value("CustomReference", QColor(0xc3, 0x34, 0x34)).value<QColor>() : QColor(0xc3, 0x34, 0x34));
    references.nominator = QRegularExpression(QStringLiteral("([\\w]+)(?#ref)"));
    references.names = source.global.labels;
    labelReferences = references;
    // adds the label definitions to the user data for each specific line
    QMapIterator<QString, int> i(references.names);
    while (i.hasNext()) {
        i.next();
        QTextBlock textBlock = parent->findBlockByLineNumber(i.value());
        BlockData* blockData = static_cast<BlockData*>(textBlock.userData());
        if (!blockData) {
            blockData = new BlockData;
            textBlock.setUserData(blockData);
        }
        blockData->definitions.append(i.key());
    }
    // end of label reference

    // assembler Directives
    assemblerDirectiveFormat.setForeground((styleIsCustom)? settings.value("CustomAssemblerDir", QColor(0xaf, 0x64, 0x00)).value<QColor>() : QColor(0xaf, 0x64, 0x00));
    assemblerDirectiveFormat.setFontItalic(true);
    assemblerDirectiveFormat.setFontWeight(QFont::Bold);

    QStringList assemblerDirectivePatterns;
    assemblerDirectivePatterns
            <<"\\.function"
            <<"\\.return"
            <<"\\.for"
            <<"\\.print"
            <<"\\.var"
            <<"\\.fill"
            <<"\\.byte"
            <<"\\.text"
            <<"\\.label"
            <<"\\.eval"
            <<"\\.align"
            <<"\\.assert"
            <<"\\.asserterror"
            <<"\\.break"
            <<"\\.by"
            <<"\\.const"
            <<"\\.cpu"
            <<"\\.define"
            <<"\\.disk"
            <<"\\.dw"
            <<"\\.dword"
            <<"\\.encoding"
            <<"\\.enum"
            <<"\\.error"
            <<"\\.errorif"
            <<"\\.file"
            <<"\\.filemodify"
            <<"\\.filenamespace"
            <<"\\.fillword"
            <<"\\.if"
            <<"\\.import binary"
            <<"\\.import c64"
            <<"\\.import source"
            <<"\\.import text"
            <<"\\.importonce"
            <<"\\.lohifill"
            <<"\\.macro"
            <<"\\.memblock"
            <<"\\.modify"
            <<"\\.namespace"
            <<"\\.pc"
            <<"\\.plugin"
            <<"\\.printnow"
            <<"\\.pseudopc"
            <<"\\.pseudocommand"
            <<"\\.segment"
            <<"\\.segmentdef"
            <<"\\.segmentout"
            <<"\\.struct"
            <<"\\.te"
            <<"\\.watch"
            <<"\\.while"
            <<"\\.wo"
            <<"\\.word"
            <<"\\.zp";

    for (const QString &patternassemblerDirective : assemblerDirectivePatterns) {
        rule.pattern = QRegularExpression(patternassemblerDirective);
        rule.format = assemblerDirectiveFormat;
        strictHighlightingRules.append(rule);
    }
    // end of assembler Directives

    // preprocessor Directives
    preprocessorDirectiveFormat.setForeground((styleIsCustom)? settings.value("CustomPreprocessorDir", QColor(0x00, 0x80, 0x00)).value<QColor>() : Qt::darkGreen);
    preprocessorDirectiveFormat.setFontWeight(QFont::Bold);
    const QString preprocessorDirectivePatterns[] = {
        QStringLiteral("#if [^\n]*"),
        QStringLiteral("#endif"),
        QStringLiteral("#undef [^\n]*"),
        QStringLiteral("#else"),
        QStringLiteral("#elif [^\n]*"),
        QStringLiteral("#import"),
        QStringLiteral("#importif [^\n]*"),
        QStringLiteral("#importonce"),
        QStringLiteral("#define [^\n]*")
    };

    for (const QString &patternPreprocessorDirectiveFormat : preprocessorDirectivePatterns) {
      rule.pattern = QRegularExpression(patternPreprocessorDirectiveFormat, QRegularExpression::CaseInsensitiveOption);
      rule.format = preprocessorDirectiveFormat;
      strictHighlightingRules.append(rule);
    }
    // end of preprocessor Directives

    // quotation
    quotationFormat.setForeground((styleIsCustom)? settings.value("CustomQuotation", QColor(0x00, 0x80, 0x00)).value<QColor>() : Qt::darkGreen);
    // (\"|\') ............ first quote (=group 1 referenced \1 aka ") starts string
    // (\\\1|[^\1]) ....... and contains either escaped quotes (\\\1 aka \") OR any but the same quote
    // *?\g1 .............. until the next same quote (\1 aka ") ends the string
    rule.pattern = QRegularExpression(QStringLiteral("(\"|\')(\\\\1|[^\\1])*?($|\\1)(?#quote)"));
    rule.partialMatch = true;
    rule.format = quotationFormat;
    strictHighlightingRules.append(rule);
    quoteExpression = rule.pattern;
    // end of quotation

    // comments
    singleLineCommentFormat.setForeground((styleIsCustom)? settings.value("CustomComment", QColor(0x80, 0x80, 0x80)).value<QColor>() : Qt::darkGray);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*(?#comment)"));
    rule.format = singleLineCommentFormat;
    // single line comment gets NOT added to the rules -
    // it will applied at the end so that comments win over any other rule
    singleLineCommentRule = rule;
    multiLineCommentFormat.setForeground((styleIsCustom)? settings.value("CustomComment", QColor(0x80, 0x80, 0x80)).value<QColor>() : Qt::darkGray);
    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
    // end of comments

    // go live
    detectStructureChanges = true;

    if (doLog) qDebug("references size=%d content={%s}", labelReferences.names.size(), qPrintable(DebugUtil::toString(labelReferences.names)));
}

void Highlighter::formatMatches(QString text, QRegularExpression pattern, bool partialMatch, int groupToFormat, QTextCharFormat format) {

    QRegularExpression::MatchType matchType = (partialMatch ?
        QRegularExpression::PartialPreferCompleteMatch : QRegularExpression::NormalMatch);
    QRegularExpressionMatchIterator matchIterator = pattern.globalMatch(text, 0, matchType);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(groupToFormat), match.capturedLength(groupToFormat), format);

        if (doLog) {
            qDebug("  %d=[%s] partial=%d", groupToFormat,
                   qPrintable(match.captured(groupToFormat)), match.hasPartialMatch());
        }
    }
}

void Highlighter::extractDefinitions(QString text, References& references) {

    QStringList sourcePerLine;
    for (const QRegularExpression &extractor : qAsConst(references.definitions)) {
        QRegularExpressionMatchIterator matchIterator = extractor.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            QString value = match.captured(1);
            sourcePerLine.append(value);
        }
    }
    // fetch current block data
    BlockData* blockData = static_cast<BlockData*>(this->currentBlockUserData());
    if (!blockData) {
        blockData = new BlockData;
        this->setCurrentBlockUserData(blockData);
    }
    if (doLog) qDebug("  BLOCK old=[%s]", qPrintable(DebugUtil::toString(blockData->definitions)));
    if (blockData->definitions.isEmpty() && sourcePerLine.isEmpty()) {
        // ignore empty definitions
    } else {
        int currentBlockNumber = currentBlock().blockNumber();
        QStringListIterator o(blockData->definitions);
        while (o.hasNext()) {
            QString name = o.next();
            if (!sourcePerLine.contains(name, Qt::CaseSensitive)) {
                int lineNumber = -99;
                if (references.names.contains(name, currentBlockNumber)) {
                    lineNumber = currentBlockNumber;
                } else if (references.names.contains(name, -1)) {
                    lineNumber = -1;
                }
                if (lineNumber >= -1) {
                    if (doLog) qDebug("    -[%s]", qPrintable(name));
                    references.names.remove(name, lineNumber);
                    hasStructureChanges = true;
                }
            }
        }
        QStringListIterator n(sourcePerLine);
        while (n.hasNext()) {
            QString name = n.next();
            if (!blockData->definitions.contains(name, Qt::CaseSensitive)) {
                if (references.names.contains(name, -1)) {
                    if (doLog) qDebug("    =[%s]:%d", qPrintable(name), currentBlockNumber);
                    references.names.replace(name, currentBlockNumber);
                } else {
                    if (doLog) qDebug("    +[%s]", qPrintable(name));
                    references.names.insert(name, currentBlockNumber);
                    hasStructureChanges = true;
                }
            }
        }
        blockData->definitions = sourcePerLine;
    }
    if (doLog) qDebug("  BLOCK new=[%s]", qPrintable(DebugUtil::toString(blockData->definitions)));
    if (doLog) qDebug("  hasStructureChanges=%d", hasStructureChanges);
}

void Highlighter::formatReferences(QString text, References& references) {

    QRegularExpressionMatchIterator matchIterator = references.nominator.globalMatch(text);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        QString candidate = match.captured(1);
        bool confirmed = references.names.contains(candidate);
        if (confirmed) {
            setFormat(match.capturedStart(1), match.capturedLength(1), references.referenceFormat);
        }

        if (doLog) {
            qDebug("  REF CHECK=[%s] success=%d", qPrintable(candidate), confirmed);
        }
    }
}

void Highlighter::formatDefinitions(QString text, References& references) {

    for (const QRegularExpression &definition : qAsConst(references.definitions)) {
        formatMatches(text, definition, false, 1, references.definitionFormat);
    }
}

void Highlighter::validateStructureChanges() {
    if (doLog) qDebug("\nVALIDATE hasStructureChanges=%d", hasStructureChanges);
    if (hasStructureChanges) {
        hasStructureChanges = false;
        detectStructureChanges = false;
        rehighlight();
        detectStructureChanges = true;
    }
}

void Highlighter::highlightBlock(const QString &text)
{
    //QRegExp choice = QRegExp("(\\.label|#comment|#quote|#ref)");
    QRegExp choice = QRegExp("(#ref)");
    bool candidate = (logActive && true);

    // apply reference rule
    doLog = (logActive && candidate);

    // determine fragemented multiline comments

    QVector<QPair<int, int>> commentFragments;
    QPair<int, int> fragment;
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {

        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        fragment.first = startIndex;
        fragment.second = commentLength;
        commentFragments.append(fragment);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }

    if (candidate) qDebug("#%d text=[%s]", currentBlock().blockNumber(), qPrintable(text));

    // scratch comments from text
    QString textToExtractFrom = "" + text;
    int singleLineCommentIndex = text.indexOf(singleLineCommentRule.pattern);
    if (singleLineCommentIndex > -1) {
        textToExtractFrom = textToExtractFrom.remove(singleLineCommentIndex, textToExtractFrom.size());
        if (doLog) qDebug("     //=[%s]", qPrintable(textToExtractFrom));
    }
    if (commentFragments.size() > 0) {
        for (const QPair<int, int> &fragment : qAsConst(commentFragments)) {
            textToExtractFrom = textToExtractFrom.replace(fragment.first, fragment.second, QString("^").repeated(fragment.second));
        }
        if (doLog) qDebug("    /**/=[%s]", qPrintable(textToExtractFrom));
    }
    if (textToExtractFrom.indexOf(QRegularExpression("[\"\']")) > -1) {
        QRegularExpressionMatchIterator matchIterator = quoteExpression.globalMatch(textToExtractFrom);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            textToExtractFrom = textToExtractFrom.replace(match.capturedStart(), match.capturedLength(), QString("^").repeated(match.capturedLength()));
        }
        if (doLog) qDebug("     \"\"=[%s]", qPrintable(textToExtractFrom));
    }

    if (detectStructureChanges) {
        if (doLog) qDebug("  EXTRACT old size=%d names={%s}", labelReferences.names.size(), qPrintable(DebugUtil::toString(labelReferences.names)));
        extractDefinitions(textToExtractFrom, labelReferences);
        if (doLog) qDebug("  EXTRACT new size=%d names={%s}", labelReferences.names.size(), qPrintable(DebugUtil::toString(labelReferences.names)));
    }

    formatReferences(textToExtractFrom, labelReferences);
    formatDefinitions(textToExtractFrom, labelReferences);

    // apply strict rules
    for (const HighlightingRule &rule : qAsConst(strictHighlightingRules)) {

        doLog = (logActive && candidate && rule.pattern.pattern().contains(choice));
        formatMatches(text, rule.pattern, rule.partialMatch, 0, rule.format);
    }

    // the single line comment is dominant and overrides other formats
    formatMatches(text, singleLineCommentRule.pattern, singleLineCommentRule.partialMatch, 0, singleLineCommentRule.format);

    if (commentFragments.size() > 0) {
        for (const QPair<int, int> &fragment : qAsConst(commentFragments)) {
            setFormat(fragment.first, fragment.second, multiLineCommentFormat);
        }
    }
}
