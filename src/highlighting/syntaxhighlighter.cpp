#include "syntaxhighlighter.h"
#include "languagedata.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent), m_languageName("Plain Text"), m_darkTheme(false)
{
    // Default constructor - no language rules
}

SyntaxHighlighter::SyntaxHighlighter(const LanguageData &langData, QTextDocument *parent)
    : QSyntaxHighlighter(parent), m_darkTheme(false)
{
    setupFormatsForLanguage(langData);
}

void SyntaxHighlighter::setLanguageData(const LanguageData &langData)
{
    setupFormatsForLanguage(langData);
    rehighlight(); // Force redraw with new rules
}

void SyntaxHighlighter::setDarkTheme(bool useDarkTheme)
{
    if (m_darkTheme == useDarkTheme)
        return;
        
    m_darkTheme = useDarkTheme;
    updateFormatsForTheme();
    rehighlight(); // Force redraw with new theme colors
}

void SyntaxHighlighter::setupFormatsForLanguage(const LanguageData &langData)
{
    // Clear any existing rules
    highlightingRules.clear();
    
    // Save the language name
    m_languageName = langData.name();
    
    // Copy all highlighting rules from the language data
    for (const auto &rule : langData.highlightingRules()) {
        HighlightingRule newRule;
        newRule.pattern = rule.pattern;
        newRule.format = rule.format;
        
        // Create dark theme version of the format with much more vibrant colors
        newRule.darkThemeFormat = rule.format; // Start with the same format
        
        // Adjust colors for dark theme - use significantly more vibrant colors
        QColor color = rule.format.foreground().color();
        
        if (color == Qt::darkBlue) 
            newRule.darkThemeFormat.setForeground(QColor(100, 180, 255));    // Much brighter blue for keywords
        else if (color == Qt::blue)
            newRule.darkThemeFormat.setForeground(QColor(100, 180, 255));    // Bright blue for keywords
        else if (color == Qt::darkRed) 
            newRule.darkThemeFormat.setForeground(QColor(235, 160, 120));   // Much brighter orange for strings
        else if (color == Qt::darkGreen) 
            newRule.darkThemeFormat.setForeground(QColor(120, 180, 100));   // Significantly brighter green for comments
        else if (color == Qt::darkYellow) 
            newRule.darkThemeFormat.setForeground(QColor(248, 248, 170));   // Much brighter yellow
        else if (color == Qt::darkMagenta) 
            newRule.darkThemeFormat.setForeground(QColor(227, 154, 235));   // Vibrant purple for keywords/tags
        else if (color == Qt::darkCyan) 
            newRule.darkThemeFormat.setForeground(QColor(98, 240, 220));    // Bright teal for identifiers
        else if (color == Qt::black) 
            newRule.darkThemeFormat.setForeground(QColor(240, 240, 240));   // Almost white text for better contrast
        else if (color == QColor(0, 128, 128)) // Typical teal color
            newRule.darkThemeFormat.setForeground(QColor(98, 240, 220));    // Brighter teal
        else if (color == QColor(128, 0, 128)) // Typical purple
            newRule.darkThemeFormat.setForeground(QColor(227, 154, 235));   // Much brighter purple
        else if (color == QColor(128, 64, 0)) // Brown
            newRule.darkThemeFormat.setForeground(QColor(235, 160, 100));   // Brighter brown
        // Special case for any remaining dark colors that might be hard to see
        else if (color.lightness() < 128)
            newRule.darkThemeFormat.setForeground(QColor(240, 240, 240));   // Ensure all text is visible
        
        highlightingRules.append(newRule);
    }
    
    // Set up multi-line comment handling
    commentStartExpression = langData.commentStartExpression();
    commentEndExpression = langData.commentEndExpression();
    multiLineCommentFormat = langData.multiLineCommentFormat();
    
    // Create dark theme version of multi-line comment format
    multiLineCommentDarkFormat = langData.multiLineCommentFormat();
    if (multiLineCommentFormat.foreground().color() == Qt::darkGreen) {
        multiLineCommentDarkFormat.setForeground(QColor(120, 180, 100)); // Much brighter green for comments
    }
}

void SyntaxHighlighter::updateFormatsForTheme()
{
    // This method would adjust format rules based on theme
    // But we're now handling this directly in the highlightBlock method
    // by using the appropriate format for the active theme
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply syntax highlighting rules
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            
            // Use the appropriate format based on the theme
            QTextCharFormat format = m_darkTheme ? rule.darkThemeFormat : rule.format;
            setFormat(match.capturedStart(), match.capturedLength(), format);
        }
    }
    
    // Handle multi-line comments if the language supports them
    if (!commentStartExpression.pattern().isEmpty() && commentStartExpression.isValid() && commentEndExpression.isValid()) {
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
                commentLength = endIndex - startIndex + match.capturedLength();
            }
            
            // Use the appropriate comment format based on the theme
            if (m_darkTheme) {
                setFormat(startIndex, commentLength, multiLineCommentDarkFormat);
            } else {
                setFormat(startIndex, commentLength, multiLineCommentFormat);
            }
            
            startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
        }
    }
}
