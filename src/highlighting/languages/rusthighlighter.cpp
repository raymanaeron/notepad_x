#include "rusthighlighter.h"

RustLanguage::RustLanguage()
{
    m_name = "Rust";
    m_fileExtensions << "rs";
    
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
    
    QTextCharFormat macroFormat;
    macroFormat.setForeground(QColor(180, 80, 0));  // Orange-brown
    
    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor(90, 90, 140));  // Slate blue
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(0, 128, 128));  // Teal
    
    QTextCharFormat lifetimeFormat;
    lifetimeFormat.setForeground(QColor(170, 0, 170));  // Purple
    lifetimeFormat.setFontItalic(true);
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define Rust keywords
    const QStringList keywordPatterns = {
        "\\bas\\b", "\\basync\\b", "\\bawait\\b", "\\bbreak\\b", "\\bconst\\b", 
        "\\bcontinue\\b", "\\bcrate\\b", "\\bdyn\\b", "\\belse\\b", "\\benum\\b", 
        "\\bextern\\b", "\\bfalse\\b", "\\bfn\\b", "\\bfor\\b", "\\bif\\b", 
        "\\bimpl\\b", "\\bin\\b", "\\blet\\b", "\\bloop\\b", "\\bmatch\\b", 
        "\\bmod\\b", "\\bmove\\b", "\\bmut\\b", "\\bpub\\b", "\\bref\\b", 
        "\\breturn\\b", "\\bself\\b", "\\bSelf\\b", "\\bstatic\\b", "\\bstruct\\b", 
        "\\bsuper\\b", "\\btrait\\b", "\\btrue\\b", "\\btype\\b", "\\bunion\\b", 
        "\\bunsafe\\b", "\\buse\\b", "\\bwhere\\b", "\\bwhile\\b", "\\byield\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Type names (start with capital letter)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format = typeFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    m_highlightingRules.append(rule);
    
    // String literals
    rule.pattern = QRegularExpression("\"(?:\\\\.|[^\\\\\"])*\"");
    rule.format = quotationFormat;
    m_highlightingRules.append(rule);
    
    // Char literals
    rule.pattern = QRegularExpression("'(?:\\\\.|[^\\\\'])'");
    rule.format = quotationFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\bfn\\s+([a-zA-Z0-9_]+)");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Macros
    rule.pattern = QRegularExpression("\\b[a-zA-Z0-9_]+!");
    rule.format = macroFormat;
    m_highlightingRules.append(rule);
    
    // Attributes
    rule.pattern = QRegularExpression("#\\[.*\\]");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = attributeFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?([eE][+-]?\\d+)?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Lifetime parameters
    rule.pattern = QRegularExpression("'[a-zA-Z_][a-zA-Z0-9_]*\\b");
    rule.format = lifetimeFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
