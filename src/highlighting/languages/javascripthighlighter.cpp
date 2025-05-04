#include "javascripthighlighter.h"

JavaScriptLanguage::JavaScriptLanguage()
{
    m_name = "JavaScript";
    m_fileExtensions << "js" << "jsx" << "mjs";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat globalObjectsFormat;
    globalObjectsFormat.setForeground(QColor(90, 90, 180)); // Light blue-purple
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat jsxFormat;
    jsxFormat.setForeground(QColor(0, 110, 110)); // Teal
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define JavaScript keywords
    const QStringList keywordPatterns = {
        "\\bbreak\\b", "\\bcase\\b", "\\bcatch\\b", "\\bclass\\b", "\\bconst\\b",
        "\\bcontinue\\b", "\\bdebugger\\b", "\\bdefault\\b", "\\bdelete\\b", "\\bdo\\b",
        "\\belse\\b", "\\benum\\b", "\\bexport\\b", "\\bextends\\b", "\\bfalse\\b",
        "\\bfinally\\b", "\\bfor\\b", "\\bfunction\\b", "\\bif\\b", "\\bimport\\b",
        "\\bin\\b", "\\binstanceof\\b", "\\bnew\\b", "\\bnull\\b", "\\breturn\\b",
        "\\bsuper\\b", "\\bswitch\\b", "\\bthis\\b", "\\bthrow\\b", "\\btrue\\b",
        "\\btry\\b", "\\btypeof\\b", "\\bvar\\b", "\\bvoid\\b", "\\bwhile\\b",
        "\\bwith\\b", "\\byield\\b", "\\blet\\b", "\\bstatic\\b", "\\bawait\\b",
        "\\basync\\b", "\\bget\\b", "\\bset\\b", "\\bof\\b", "\\bfrom\\b"
    };
    
    // JavaScript global objects and properties
    const QStringList globalObjectPatterns = {
        "\\bArray\\b", "\\bBoolean\\b", "\\bDate\\b", "\\bError\\b", "\\bJSON\\b",
        "\\bMath\\b", "\\bNumber\\b", "\\bObject\\b", "\\bPromise\\b", "\\bRegExp\\b",
        "\\bString\\b", "\\bconsole\\b", "\\bdocument\\b", "\\bwindow\\b", "\\bglobal\\b",
        "\\bMap\\b", "\\bSet\\b", "\\bSymbol\\b", "\\bWeakMap\\b", "\\bWeakSet\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for global objects
    for (const QString &pattern : globalObjectPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = globalObjectsFormat;
        m_highlightingRules.append(rule);
    }
    
    // Single-line comments
    HighlightingRule rule;
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted strings
    rule.pattern = QRegularExpression("\"(?:\\\\\"|[^\"])*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted strings
    rule.pattern = QRegularExpression("'(?:\\\\'|[^'])*'");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Template strings (backticks)
    rule.pattern = QRegularExpression("`(?:\\\\`|[^`])*`");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\bfunction\\s+([a-zA-Z0-9_]+)");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Method declarations
    rule.pattern = QRegularExpression("\\b([a-zA-Z0-9_]+)\\s*\\([^)]*\\)\\s*\\{");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Arrow functions
    rule.pattern = QRegularExpression("\\([^)]*\\)\\s*=>|[a-zA-Z0-9_]+\\s*=>");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers (decimal, hex, binary and floating-point)
    rule.pattern = QRegularExpression("\\b\\d+\\.\\d*([eE][+-]?\\d+)?\\b|\\b\\.\\d+([eE][+-]?\\d+)?\\b|\\b\\d+[eE][+-]?\\d+\\b|\\b0x[0-9a-fA-F]+\\b|\\b0b[01]+\\b|\\b\\d+\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // JSX tags for .jsx files
    rule.pattern = QRegularExpression("</?[a-zA-Z][a-zA-Z0-9.:-]*");
    rule.format = jsxFormat;
    m_highlightingRules.append(rule);
    
    // JSX attributes
    rule.pattern = QRegularExpression("\\b[a-zA-Z][a-zA-Z0-9_-]*(?==)");
    rule.format = jsxFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
