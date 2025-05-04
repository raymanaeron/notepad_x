#include "typescripthighlighter.h"

TypeScriptLanguage::TypeScriptLanguage()
{
    m_name = "TypeScript";
    m_fileExtensions << "ts" << "tsx";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    
    QTextCharFormat decoratorFormat;
    decoratorFormat.setForeground(QColor(128, 64, 0)); // Brown
    decoratorFormat.setFontItalic(true);
    
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
    
    // Define TypeScript keywords
    const QStringList keywordPatterns = {
        "\\babstract\\b", "\\bas\\b", "\\basync\\b", "\\bawait\\b", "\\bboolean\\b",
        "\\bbreak\\b", "\\bcase\\b", "\\bcatch\\b", "\\bclass\\b", "\\bconst\\b",
        "\\bconstructor\\b", "\\bcontinue\\b", "\\bdebugger\\b", "\\bdeclare\\b", "\\bdefault\\b",
        "\\bdelete\\b", "\\bdo\\b", "\\belse\\b", "\\benum\\b", "\\bexport\\b",
        "\\bextends\\b", "\\bfalse\\b", "\\bfinally\\b", "\\bfor\\b", "\\bfrom\\b",
        "\\bfunction\\b", "\\bget\\b", "\\bif\\b", "\\bimplements\\b", "\\bimport\\b",
        "\\bin\\b", "\\binfer\\b", "\\binstanceof\\b", "\\binterface\\b", "\\bis\\b",
        "\\bkeyof\\b", "\\blet\\b", "\\bmodule\\b", "\\bnamespace\\b", "\\bnew\\b",
        "\\bnull\\b", "\\bnumber\\b", "\\bof\\b", "\\bpackage\\b", "\\bprivate\\b",
        "\\bprotected\\b", "\\bpublic\\b", "\\breadonly\\b", "\\breturn\\b", "\\brequires\\b",
        "\\bset\\b", "\\bstatic\\b", "\\bstring\\b", "\\bsuper\\b", "\\bswitch\\b",
        "\\bsymbol\\b", "\\bthis\\b", "\\bthrow\\b", "\\btrue\\b", "\\btry\\b",
        "\\btype\\b", "\\btypeof\\b", "\\bundefined\\b", "\\bunique\\b", "\\bunknown\\b",
        "\\bvar\\b", "\\bvoid\\b", "\\bwhile\\b", "\\bwith\\b", "\\byield\\b",
        "\\bany\\b", "\\bnever\\b", "\\bobject\\b", "\\baggregate\\b", "\\bconditional\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Type annotations
    HighlightingRule rule;
    rule.pattern = QRegularExpression(":\\s*([A-Z][a-zA-Z0-9_]*|[a-z][a-zA-Z0-9_]*)");
    rule.format = typeFormat;
    m_highlightingRules.append(rule);
    
    // Decorators
    rule.pattern = QRegularExpression("@[a-zA-Z0-9_]+");
    rule.format = decoratorFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
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
    
    // Template strings
    rule.pattern = QRegularExpression("`(?:\\\\`|[^`])*`");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\b[a-zA-Z0-9_]+(?=\\s*\\()");
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
    
    // JSX tags
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
