#include "javahighlighter.h"

JavaLanguage::JavaLanguage()
{
    m_name = "Java";
    m_fileExtensions << "java";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    
    QTextCharFormat annotationFormat;
    annotationFormat.setForeground(QColor(128, 64, 0)); // Brown
    annotationFormat.setFontItalic(true);
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define Java keywords
    const QStringList keywordPatterns = {
        "\\babstract\\b", "\\bassert\\b", "\\bboolean\\b", "\\bbreak\\b", 
        "\\bbyte\\b", "\\bcase\\b", "\\bcatch\\b", "\\bchar\\b", "\\bclass\\b", 
        "\\bconst\\b", "\\bcontinue\\b", "\\bdefault\\b", "\\bdo\\b", "\\bdouble\\b", 
        "\\belse\\b", "\\benum\\b", "\\bextends\\b", "\\bfinal\\b", "\\bfinally\\b", 
        "\\bfloat\\b", "\\bfor\\b", "\\bif\\b", "\\bgoto\\b", "\\bimplements\\b", 
        "\\bimport\\b", "\\binstanceof\\b", "\\bint\\b", "\\binterface\\b", 
        "\\blong\\b", "\\bnative\\b", "\\bnew\\b", "\\bpackage\\b", "\\bprivate\\b", 
        "\\bprotected\\b", "\\bpublic\\b", "\\breturn\\b", "\\bshort\\b", "\\bstatic\\b", 
        "\\bstrictfp\\b", "\\bsuper\\b", "\\bswitch\\b", "\\bsynchronized\\b", 
        "\\bthis\\b", "\\bthrow\\b", "\\bthrows\\b", "\\btransient\\b", "\\btry\\b", 
        "\\bvoid\\b", "\\bvolatile\\b", "\\bwhile\\b", "\\btrue\\b", "\\bfalse\\b", 
        "\\bnull\\b", "\\bvar\\b", "\\byield\\b", "\\brecord\\b", "\\bsealed\\b",
        "\\bpermits\\b", "\\bnon-sealed\\b", "\\bmodule\\b", "\\bopen\\b", 
        "\\brequires\\b", "\\bexports\\b", "\\bopens\\b", "\\bto\\b", "\\buses\\b", 
        "\\bprovides\\b", "\\bwith\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Class names (starting with capital letter)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format = typeFormat;
    m_highlightingRules.append(rule);
    
    // Annotations (starting with @)
    rule.pattern = QRegularExpression("@[a-zA-Z0-9_]+\\b");
    rule.format = annotationFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // String literals
    rule.pattern = QRegularExpression("\"(?:\\\\\"|[^\"])*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Char literals
    rule.pattern = QRegularExpression("'(?:\\\\.|[^\\\\'])'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\b[a-zA-Z0-9_]+(?=\\s*\\()");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers (decimal, hex, and floating-point)
    rule.pattern = QRegularExpression("\\b\\d+\\.\\d*([eE][+-]?\\d+)?[fFdD]?\\b|\\b\\.\\d+([eE][+-]?\\d+)?[fFdD]?\\b|\\b\\d+[eE][+-]?\\d+[fFdD]?\\b|\\b0x[0-9a-fA-F]+\\b|\\b\\d+[lLfFdD]?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
