#include "kotlinhighlighter.h"

KotlinLanguage::KotlinLanguage()
{
    m_name = "Kotlin";
    m_fileExtensions << "kt" << "kts";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    typeFormat.setFontWeight(QFont::Bold);
    
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
    
    QTextCharFormat nullSafetyFormat;
    nullSafetyFormat.setForeground(QColor(255, 100, 0)); // Orange
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define Kotlin keywords
    const QStringList keywordPatterns = {
        "\\bas\\b", "\\bas?\\b", "\\bbreak\\b", "\\bclass\\b", "\\bcontinue\\b",
        "\\bdo\\b", "\\belse\\b", "\\bfalse\\b", "\\bfor\\b", "\\bfun\\b",
        "\\bif\\b", "\\bin\\b", "\\bin\\?\\b", "\\binterface\\b", "\\bis\\b", "\\bis!\\b",
        "\\bnull\\b", "\\bobject\\b", "\\bpackage\\b", "\\breturn\\b", "\\bsuper\\b",
        "\\bthis\\b", "\\bthrow\\b", "\\btrue\\b", "\\btry\\b", "\\btypealias\\b",
        "\\bval\\b", "\\bvar\\b", "\\bwhen\\b", "\\bwhile\\b", "\\bby\\b", "\\bcatch\\b",
        "\\bconstructor\\b", "\\bdelegates\\b", "\\bdynamic\\b", "\\bfield\\b",
        "\\bfile\\b", "\\bfinally\\b", "\\bget\\b", "\\bimport\\b", "\\binit\\b",
        "\\bparam\\b", "\\bproperty\\b", "\\breceiver\\b", "\\bset\\b", "\\bsetparam\\b",
        "\\bwhere\\b", "\\bactual\\b", "\\babstract\\b", "\\bannotation\\b", "\\bcompanion\\b",
        "\\bconst\\b", "\\bcrossinline\\b", "\\bdata\\b", "\\benum\\b", "\\bexpect\\b",
        "\\bexternal\\b", "\\bfinal\\b", "\\binfix\\b", "\\binline\\b", "\\binner\\b",
        "\\binternal\\b", "\\blateinit\\b", "\\bnoinline\\b", "\\bopen\\b", "\\boperator\\b",
        "\\bout\\b", "\\boverride\\b", "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b",
        "\\breified\\b", "\\bsealed\\b", "\\bsuspend\\b", "\\btailrec\\b", "\\bvararg\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Type names (starting with capital letter)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format = typeFormat;
    m_highlightingRules.append(rule);
    
    // Annotations (starting with @)
    rule.pattern = QRegularExpression("@[a-zA-Z0-9_.]+");
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
    
    // Raw string literals (with triple quotes)
    rule.pattern = QRegularExpression("\"\"\".*\"\"\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Char literals
    rule.pattern = QRegularExpression("'(?:\\\\.|[^\\\\'])'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\bfun\\s+([a-zA-Z0-9_]+)");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Lambda expressions
    rule.pattern = QRegularExpression("\\{\\s*[^}]*->.*\\}");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Null-safety operators
    rule.pattern = QRegularExpression("\\?:|\\?\\.|!!");
    rule.format = nullSafetyFormat;
    m_highlightingRules.append(rule);
    
    // Numbers (decimal, hex, binary and floating-point)
    rule.pattern = QRegularExpression("\\b\\d+[uULl]*\\b|\\b0x[0-9a-fA-F]+[uULl]*\\b|\\b0b[01]+[uULl]*\\b|\\b\\d+\\.\\d*([eE][+-]?\\d+)?[fF]?\\b|\\b\\.\\d+([eE][+-]?\\d+)?[fF]?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
