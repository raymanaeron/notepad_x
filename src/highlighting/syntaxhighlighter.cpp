#include "syntaxhighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Default constructor, creates a plain text highlighter
}

SyntaxHighlighter::SyntaxHighlighter(const LanguageData &languageData, QTextDocument *parent)
    : QSyntaxHighlighter(parent), m_languageData(languageData)
{
    // Constructor with specific language data
}

void SyntaxHighlighter::setLanguage(const LanguageData &languageData)
{
    m_languageData = languageData;
    rehighlight(); // Reapply highlighting rules to the entire document
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply regular expression-based highlighting rules
    for (const HighlightingRule &rule : m_languageData.highlightingRules()) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    
    // Handle multi-line comments
    setCurrentBlockState(0);
    
    // If the language has multi-line comment support
    if (!m_languageData.commentStartExpression().pattern().isEmpty()) {
        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = text.indexOf(m_languageData.commentStartExpression());
        
        while (startIndex >= 0) {
            QRegularExpressionMatch match = m_languageData.commentEndExpression().match(text, startIndex + 2);
            int endIndex = match.capturedStart();
            int commentLength = 0;
            
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex + match.capturedLength();
            }
            
            setFormat(startIndex, commentLength, m_languageData.multiLineCommentFormat());
            startIndex = text.indexOf(m_languageData.commentStartExpression(), startIndex + commentLength);
        }
    }
}
