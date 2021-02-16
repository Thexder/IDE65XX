#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QQueue>
#include <QRegularExpression>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private slots:
    void validateStructureChanges();

private:
    /* static rules */
    struct BlockData : public QTextBlockUserData
    {
        QStringList definitions;
    };

    /* static rules */
    struct HighlightingRule
    {
        // detects a specific pattern to highlight
        QRegularExpression pattern;
        // defines open or fixed ending
        bool partialMatch = false;
        // the format to apply to the matching text
        QTextCharFormat format;
    };

    /* references and definitions */
    struct References
    {
        // detects specific definitions
        QVector<QRegularExpression> definitions;
        // detects a candidate referencing a definition
        QRegularExpression nominator;
        // format for a definition
        QTextCharFormat definitionFormat;
        // format for a valid reference
        QTextCharFormat referenceFormat;
        // collection of valid references
        QMultiMap<QString, int> names;
    };

    // formats pattern matches in text with the given format
    void formatMatches(QString text, QRegularExpression pattern, bool partialMatch, int groupToFormat, QTextCharFormat format);

    // extracts definitions for given references
    void extractDefinitions(QString text, References& references);

    bool detectStructureChanges = false;
    bool hasStructureChanges = false;

    void formatReferences(QString text, References& references);
    void formatDefinitions(QString text, References& references);


    QVector<HighlightingRule> strictHighlightingRules;
    QVector<HighlightingRule> fuzzyHighlightingRules;

    References labelReferences;

    HighlightingRule singleLineCommentRule;

    QRegularExpression quoteExpression;
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat cmdFormat;
    QTextCharFormat assemblerDirectiveFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat labelFormat;
    QTextCharFormat preprocessorDirectiveFormat;
    QTextCharFormat numberFormat;
};

#endif // HIGHLIGHTER_H
