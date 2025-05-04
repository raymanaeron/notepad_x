#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>
#include <QVector>
#include "languagedata.h" // Include the full header instead of forward declaration

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    
public:
    SyntaxHighlighter(QTextDocument *parent = nullptr);
    SyntaxHighlighter(const LanguageData &langData, QTextDocument *parent = nullptr);
    
    void setLanguageData(const LanguageData &langData);
    QString languageName() const { return m_languageName; }
    void setDarkTheme(bool useDarkTheme);
    
protected:
    void highlightBlock(const QString &text) override;
    
private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
        QTextCharFormat darkThemeFormat; // Dark theme version
    };
    QVector<HighlightingRule> highlightingRules;
    
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat multiLineCommentDarkFormat; // Dark theme version
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    
    QString m_languageName;
    bool m_darkTheme;
    
    void setupFormatsForLanguage(const LanguageData &langData);
    void updateFormatsForTheme();
};

#endif // SYNTAXHIGHLIGHTER_H
