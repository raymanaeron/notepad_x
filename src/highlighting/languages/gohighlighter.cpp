#include "gohighlighter.h"

GoLanguage::GoLanguage()
{
    m_name = "Go";
    m_fileExtensions << "go";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    
    QTextCharFormat builtinFormat;
    builtinFormat.setForeground(QColor(0, 134, 179)); // Teal blue
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define Go keywords
    const QStringList keywordPatterns = {
        "\\bbreak\\b", "\\bcase\\b", "\\bchan\\b", "\\bconst\\b", 
        "\\bcontinue\\b", "\\bdefault\\b", "\\bdefer\\b", "\\belse\\b", 
        "\\bfallthrough\\b", "\\bfor\\b", "\\bfunc\\b", "\\bgo\\b", 
        "\\bgoto\\b", "\\bif\\b", "\\bimport\\b", "\\binterface\\b", 
        "\\bmap\\b", "\\bpackage\\b", "\\brange\\b", "\\breturn\\b", 
        "\\bselect\\b", "\\bstruct\\b", "\\bswitch\\b", "\\btype\\b", 
        "\\bvar\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Go built-in types
    const QStringList typePatterns = {
        "\\bbool\\b", "\\bbyte\\b", "\\bcomplex64\\b", "\\bcomplex128\\b", 
        "\\berror\\b", "\\bfloat32\\b", "\\bfloat64\\b", "\\bint\\b", 
        "\\bint8\\b", "\\bint16\\b", "\\bint32\\b", "\\bint64\\b", 
        "\\brune\\b", "\\bstring\\b", "\\buint\\b", "\\buint8\\b", 
        "\\buint16\\b", "\\buint32\\b", "\\buint64\\b", "\\buintptr\\b"
    };
    
    for (const QString &pattern : typePatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        m_highlightingRules.append(rule);
    }
    
    // Go built-in functions
    const QStringList builtinPatterns = {
        "\\bappend\\b", "\\bcap\\b", "\\bclose\\b", "\\bcomplex\\b", 
        "\\bcopy\\b", "\\bdelete\\b", "\\bimag\\b", "\\blen\\b", 
        "\\bmake\\b", "\\bnew\\b", "\\bpanic\\b", "\\bprint\\b", 
        "\\bprintln\\b", "\\breal\\b", "\\brecover\\b"
    };
    
    for (const QString &pattern : builtinPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = builtinFormat;
        m_highlightingRules.append(rule);
    }
    
    // Single-line comments
    HighlightingRule rule;
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted strings
    rule.pattern = QRegularExpression("\"(?:\\\\.|[^\\\\\"])*\"");
    rule.format = quotationFormat;
    m_highlightingRules.append(rule);
    
    // Raw strings with backticks
    rule.pattern = QRegularExpression("`[^`]*`");
    rule.format = quotationFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\bfunc\\s+([a-zA-Z0-9_]+)");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?([eE][+-]?\\d+)?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
