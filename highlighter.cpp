#include "highlighter.h"
#include "common.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QSettings settings(Common::appConfigDir()+"/settings.ini", QSettings::IniFormat);

    // application style
    bool styleIsCustom = settings.value("ApplicationStyle").toInt() == 5;
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
        highlightingRules.append(rule);
    }

    // number
    numberFormat.setForeground((styleIsCustom)? settings.value("CustomNumber", QColor(0x00, 0x80, 0x80)).value<QColor>() : Qt::darkCyan);
    rule.pattern = QRegularExpression(QStringLiteral("(?:\\$[0-9a-fA-F]*)|(?:\\#\\$[0-9a-fA-F]*)|(?:\\#\\%[01]*)|(?:\\%[01]*)|(?:\\#[0-9]*)|(?:\\ [0-9]*)|(?:\\([0-9]*\\))"));
    rule.format = numberFormat;
    highlightingRules.append(rule);
    // end of number

    // function
    functionFormat.setFontItalic(true);
    functionFormat.setForeground((styleIsCustom)? settings.value("CustomFunction", QColor(0xaf, 0x64, 0x00)).value<QColor>() : QColor(0xaf, 0x64, 0x00));
    functionFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);
    // end of function

    // label
    labelFormat.setFontWeight(QFont::ExtraBold);
    labelFormat.setFontItalic(true);
    labelFormat.setForeground((styleIsCustom)? settings.value("CustomLabel", QColor(0xc3, 0x34, 0x34)).value<QColor>() : QColor(0xc3, 0x34, 0x34));
    rule.pattern = QRegularExpression(QStringLiteral("\\S+:"));
    rule.format = labelFormat;
    highlightingRules.append(rule);
    // end of label

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
        highlightingRules.append(rule);
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
      highlightingRules.append(rule);
    }
    // end of preprocessor Directives

    // comments
    singleLineCommentFormat.setForeground((styleIsCustom)? settings.value("CustomComment", QColor(0x80, 0x80, 0x80)).value<QColor>() : Qt::darkGray);
    rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground((styleIsCustom)? settings.value("CustomComment", QColor(0x80, 0x80, 0x80)).value<QColor>() : Qt::darkGray);

    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
    // end of comments

    // quotation
    quotationFormat.setForeground((styleIsCustom)? settings.value("CustomQuotation", QColor(0x00, 0x80, 0x00)).value<QColor>() : Qt::darkGreen);
    // (\"|\') ............ first quote (=group 1 referenced \1 aka ") starts string
    // (\\\1|[^\1]) ....... and contains either escaped quotes (\\\1 aka \") OR any but the same quote
    // *?\g1 .............. until the next same quote (\1 aka ") ends the string
    rule.pattern = QRegularExpression(QStringLiteral("(\"|\')(\\\\1|[^\\1])*?($|\\1)"));
    rule.partialMatch = true;
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    // end of quotation
}

void Highlighter::highlightBlock(const QString &text)
{
//    bool doLog = text.contains(".const") || text.contains("#import");

//    if (doLog) qDebug("text=%s", qPrintable(text));

//    if (doLog) {
//        QRegularExpression regex = QRegularExpression("(\"|\')(\\\\1|[^\\1])*($|\\1)");
//        QRegularExpressionMatchIterator matchIterator = regex.globalMatch(text);
//        if (matchIterator.hasNext()) {
//         QRegularExpressionMatch match = matchIterator.next();
//            qDebug(" x=%s p=%d", qPrintable(match.captured(0)), match.capturedStart(0));
//        }
//    }

    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {

//        bool thisRule = rule.pattern.pattern().contains("\\1");
        QRegularExpression::MatchType matchType = (rule.partialMatch ?
            QRegularExpression::PartialPreferCompleteMatch : QRegularExpression::NormalMatch);
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text, 0, matchType);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);

//            if (thisRule && doLog) qDebug("match=%s partial=%d", qPrintable(match.captured()), match.hasPartialMatch());
        }
    }

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
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
