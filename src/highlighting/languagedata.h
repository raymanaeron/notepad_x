#ifndef LANGUAGEDATA_H
#define LANGUAGEDATA_H

#include <QVector>
#include <QString>
#include <QTextCharFormat>
#include <QRegularExpression>

struct HighlightingRule {
    QRegularExpression pattern;
    QTextCharFormat format;
    QTextCharFormat darkThemeFormat; // Add storage for dark theme format
};

class LanguageData {
public:
    LanguageData();
    virtual ~LanguageData() = default;
    
    QString name() const { return m_name; }
    QVector<HighlightingRule> highlightingRules() const { return m_highlightingRules; }
    
    // For multi-line comment handling
    QRegularExpression commentStartExpression() const { return m_commentStartExpression; }
    QRegularExpression commentEndExpression() const { return m_commentEndExpression; }
    QTextCharFormat multiLineCommentFormat() const { return m_multiLineCommentFormat; }
    QTextCharFormat multiLineCommentDarkFormat() const { return m_multiLineCommentDarkFormat; }
    
    // File extensions supported by this language
    QStringList fileExtensions() const { return m_fileExtensions; }
    
protected:
    QString m_name;
    QVector<HighlightingRule> m_highlightingRules;
    QRegularExpression m_commentStartExpression;
    QRegularExpression m_commentEndExpression;
    QTextCharFormat m_multiLineCommentFormat;
    QTextCharFormat m_multiLineCommentDarkFormat; // Dark theme version
    QStringList m_fileExtensions;
};

#endif // LANGUAGEDATA_H
