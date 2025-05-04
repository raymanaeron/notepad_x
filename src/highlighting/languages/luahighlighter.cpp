#include "luahighlighter.h"

LuaLanguage::LuaLanguage()
{
    m_name = "Lua";
    m_fileExtensions << "lua";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat builtInFormat;
    builtInFormat.setForeground(QColor(0, 110, 140)); // Teal/Blue
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    // Lua keywords
    const QStringList keywordPatterns = {
        "\\band\\b", "\\bbreak\\b", "\\bdo\\b", "\\belse\\b", "\\belseif\\b",
        "\\bend\\b", "\\bfalse\\b", "\\bfor\\b", "\\bfunction\\b", "\\bgoto\\b",
        "\\bif\\b", "\\bin\\b", "\\blocal\\b", "\\bnil\\b", "\\bnot\\b",
        "\\bor\\b", "\\brepeat\\b", "\\breturn\\b", "\\bthen\\b", "\\btrue\\b",
        "\\buntil\\b", "\\bwhile\\b"
    };
    
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Lua built-in functions and libraries
    const QStringList builtInPatterns = {
        // Standard library
        "\\bprint\\b", "\\btonumber\\b", "\\btostring\\b", "\\btype\\b", "\\bselect\\b",
        "\\bnext\\b", "\\bpairs\\b", "\\bipairs\\b", "\\bpcall\\b", "\\bxpcall\\b",
        "\\brequire\\b", "\\bdofile\\b", "\\bloadfile\\b", "\\bloadstring\\b", "\\bsetmetatable\\b",
        "\\bgetmetatable\\b", "\\brawequal\\b", "\\brawget\\b", "\\brawset\\b", "\\bcollectgarbage\\b",
        
        // Libraries
        "\\bstring\\.\\w+\\b", "\\btable\\.\\w+\\b", "\\bmath\\.\\w+\\b", 
        "\\bio\\.\\w+\\b", "\\bos\\.\\w+\\b", "\\bcoroutine\\.\\w+\\b", 
        "\\bdebug\\.\\w+\\b", "\\bpackage\\.\\w+\\b", "\\bbit32\\.\\w+\\b", "\\butf8\\.\\w+\\b"
    };
    
    for (const QString &pattern : builtInPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = builtInFormat;
        m_highlightingRules.append(rule);
    }
    
    // Function declarations
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\bfunction\\s+([a-zA-Z0-9_.:]+)\\s*\\(");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Strings with double quotes
    rule.pattern = QRegularExpression("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Strings with single quotes
    rule.pattern = QRegularExpression("'[^'\\\\]*(\\\\.[^'\\\\]*)*'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Multiline strings with [[
    rule.pattern = QRegularExpression("\\[\\[.*?\\]\\]");
    rule.pattern.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("--[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Numbers (integer, hex, float)
    rule.pattern = QRegularExpression("\\b[0-9]+\\b|\\b0x[0-9a-fA-F]+\\b|\\b[0-9]+\\.[0-9]+\\b|\\b[0-9]+[eE][+-]?[0-9]+\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multiline comments
    m_commentStartExpression = QRegularExpression("--\\[\\[");
    m_commentEndExpression = QRegularExpression("\\]\\]");
    m_multiLineCommentFormat = commentFormat;
}
