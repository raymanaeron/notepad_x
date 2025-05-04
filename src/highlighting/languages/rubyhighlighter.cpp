#include "rubyhighlighter.h"

RubyLanguage::RubyLanguage()
{
    m_name = "Ruby";
    m_fileExtensions << "rb" << "rbw" << "rake" << "gemspec";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat classFormat;
    classFormat.setForeground(Qt::darkMagenta);
    classFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat symbolFormat;
    symbolFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);
    
    QTextCharFormat methodFormat;
    methodFormat.setForeground(QColor(0, 128, 128)); // Teal
    methodFormat.setFontItalic(true);
    
    QTextCharFormat instanceVarFormat;
    instanceVarFormat.setForeground(QColor(170, 0, 170)); // Purple
    
    QTextCharFormat globalVarFormat;
    globalVarFormat.setForeground(QColor(170, 0, 0)); // Dark Red
    
    QTextCharFormat constantFormat;
    constantFormat.setForeground(QColor(120, 20, 80)); // Dark purple
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat regexpFormat;
    regexpFormat.setForeground(QColor(210, 65, 30)); // Orange-red
    
    // Ruby keywords
    const QStringList keywordPatterns = {
        "\\balias\\b", "\\band\\b", "\\bbegin\\b", "\\bbreak\\b", "\\bcase\\b", 
        "\\bclass\\b", "\\bdef\\b", "\\bdefined\\b", "\\bdo\\b", "\\belse\\b", 
        "\\belsif\\b", "\\bend\\b", "\\bensure\\b", "\\bfalse\\b", "\\bfor\\b", 
        "\\bif\\b", "\\bin\\b", "\\bmodule\\b", "\\bnext\\b", "\\bnil\\b", 
        "\\bnot\\b", "\\bor\\b", "\\bredo\\b", "\\brescue\\b", "\\bretry\\b", 
        "\\breturn\\b", "\\bself\\b", "\\bsuper\\b", "\\bthen\\b", "\\btrue\\b", 
        "\\bundef\\b", "\\bunless\\b", "\\buntil\\b", "\\bwhen\\b", "\\bwhile\\b", 
        "\\byield\\b", "\\brequire\\b", "\\binclude\\b", "\\bextend\\b", "\\battr_reader\\b", 
        "\\battr_writer\\b", "\\battr_accessor\\b", "\\bprivate\\b", "\\bprotected\\b", 
        "\\bpublic\\b", "\\braise\\b", "\\bcatch\\b", "\\bthrow\\b", "\\bproc\\b", 
        "\\blambda\\b"
    };
    
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Class and module names
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\bclass\\s+([A-Z][A-Za-z0-9_]*)\\b|\\bmodule\\s+([A-Z][A-Za-z0-9_]*)\\b");
    rule.format = classFormat;
    m_highlightingRules.append(rule);
    
    // Constants (starting with capital letter)
    rule.pattern = QRegularExpression("\\b[A-Z][A-Za-z0-9_]*\\b");
    rule.format = constantFormat;
    m_highlightingRules.append(rule);
    
    // Method definitions
    rule.pattern = QRegularExpression("\\bdef\\s+([a-zA-Z_][a-zA-Z0-9_]*)\\b");
    rule.format = methodFormat;
    m_highlightingRules.append(rule);
    
    // Symbols
    rule.pattern = QRegularExpression(":[a-zA-Z_][a-zA-Z0-9_]*\\b|:\"[^\"]*\"|:'[^']*'");
    rule.format = symbolFormat;
    m_highlightingRules.append(rule);
    
    // Instance variables
    rule.pattern = QRegularExpression("@[a-zA-Z_][a-zA-Z0-9_]*\\b");
    rule.format = instanceVarFormat;
    m_highlightingRules.append(rule);
    
    // Class variables
    rule.pattern = QRegularExpression("@@[a-zA-Z_][a-zA-Z0-9_]*\\b");
    rule.format = instanceVarFormat;
    m_highlightingRules.append(rule);
    
    // Global variables
    rule.pattern = QRegularExpression("\\$[a-zA-Z_][a-zA-Z0-9_]*\\b|\\$\\d+|\\$[!@&`'+~=/\\\\,;.<>*$?:\"]");
    rule.format = globalVarFormat;
    m_highlightingRules.append(rule);
    
    // Regular expressions
    rule.pattern = QRegularExpression("\\/%[^%]*%\\/|\\/{1}[^\\n\\/]*\\/{1}[iomxneus]*");
    rule.format = regexpFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted strings with interpolation
    rule.pattern = QRegularExpression("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted strings
    rule.pattern = QRegularExpression("'[^'\\\\]*(\\\\.[^'\\\\]*)*'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comment
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Heredoc
    rule.pattern = QRegularExpression("<<[\\-~]?(['\"]?)([a-zA-Z_][a-zA-Z0-9_]*)\\1");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\b|\\b0[xX][0-9a-fA-F]+\\b|\\b[0-9]+\\.[0-9]+([eE][+-]?[0-9]+)?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Ruby has no multiline comments
    m_commentStartExpression = QRegularExpression("=begin");
    m_commentEndExpression = QRegularExpression("=end");
    m_multiLineCommentFormat = commentFormat;
}
