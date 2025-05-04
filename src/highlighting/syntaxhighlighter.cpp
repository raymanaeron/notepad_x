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
    // Save the language name
    m_languageName = langData.name();
    
    // Clear any existing rules
    highlightingRules.clear();
    
    // Copy all highlighting rules from the language data
    for (const auto &rule : langData.highlightingRules()) {
        HighlightingRule newRule;
        newRule.pattern = rule.pattern;
        newRule.format = rule.format;
        
        // Also create a dark theme version of the format
        newRule.darkThemeFormat = rule.format; // Start with the same format
        
        // Adjust colors for dark theme
        QColor color = rule.format.foreground().color();
        
        if (color == Qt::darkBlue) 
            newRule.darkThemeFormat.setForeground(QColor(120, 120, 255)); // Lighter blue
        else if (color == Qt::darkRed) 
            newRule.darkThemeFormat.setForeground(QColor(255, 100, 100)); // Lighter red
        else if (color == Qt::darkGreen) 
            newRule.darkThemeFormat.setForeground(QColor(100, 200, 100)); // Lighter green
        else if (color == Qt::darkYellow) 
            newRule.darkThemeFormat.setForeground(QColor(240, 240, 100)); // Lighter yellow
        else if (color == Qt::darkMagenta) 
            newRule.darkThemeFormat.setForeground(QColor(230, 120, 230)); // Lighter magenta
        else if (color == Qt::darkCyan) 
            newRule.darkThemeFormat.setForeground(QColor(100, 220, 220)); // Lighter cyan
        else if (color == Qt::black) 
            newRule.darkThemeFormat.setForeground(Qt::white); // White for black
            
        highlightingRules.append(newRule);
    }
    
    // Set up multi-line comment handling
    commentStartExpression = langData.commentStartExpression();
    commentEndExpression = langData.commentEndExpression();
    multiLineCommentFormat = langData.multiLineCommentFormat();
    
    // Create dark theme version of multi-line comment format
    multiLineCommentDarkFormat = langData.multiLineCommentFormat();
    if (multiLineCommentFormat.foreground().color() == Qt::darkGreen) {
        multiLineCommentDarkFormat.setForeground(QColor(100, 200, 100)); // Lighter green
    }
    
    // Apply current theme settings
    if (m_darkTheme) {
        updateFormatsForTheme();
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
    for (const auto &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            // Apply the appropriate format based on the theme
            if (m_darkTheme) {
                setFormat(match.capturedStart(), match.capturedLength(), rule.darkThemeFormat);
            } else {
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
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
