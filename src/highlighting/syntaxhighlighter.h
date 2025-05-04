#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include "languagedata.h"

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);
    explicit SyntaxHighlighter(const LanguageData &languageData, QTextDocument *parent = nullptr);
    
    void setLanguage(const LanguageData &languageData);
    QString languageName() const { return m_languageData.name(); }

protected:
    void highlightBlock(const QString &text) override;

private:
    LanguageData m_languageData;
};

#endif // SYNTAXHIGHLIGHTER_H
