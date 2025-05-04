#include "swifthighlighter.h"

SwiftLanguage::SwiftLanguage()
{
    m_name = "Swift";
    m_fileExtensions << "swift";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    typeFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor(128, 64, 0)); // Brown
    attributeFormat.setFontItalic(true);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat specialFormat;
    specialFormat.setForeground(QColor(180, 30, 90)); // Reddish-purple
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define Swift keywords
    const QStringList keywordPatterns = {
        "\\bas\\b", "\\bassociatedtype\\b", "\\bbreak\\b", "\\bcase\\b", "\\bcatch\\b",
        "\\bclass\\b", "\\bcontinue\\b", "\\bdeinit\\b", "\\bdefault\\b", "\\bdefer\\b",
        "\\bdo\\b", "\\belse\\b", "\\benum\\b", "\\bextension\\b", "\\bfallthrough\\b",
        "\\bfalse\\b", "\\bfileprivate\\b", "\\bfor\\b", "\\bfunc\\b", "\\bguard\\b",
        "\\bif\\b", "\\bin\\b", "\\binit\\b", "\\binout\\b", "\\bis\\b",
        "\\binitializer\\b", "\\binternal\\b", "\\blet\\b", "\\bnil\\b", "\\boperator\\b",
        "\\bprivate\\b", "\\bprotocol\\b", "\\bpublic\\b", "\\brepeat\\b", "\\brethrows\\b",
        "\\breturn\\b", "\\bself\\b", "\\bSelf\\b", "\\bstatic\\b", "\\bstruct\\b",
        "\\bsuper\\b", "\\bswitch\\b", "\\bthrow\\b", "\\bthrows\\b", "\\btrue\\b",
        "\\btry\\b", "\\btypealias\\b", "\\bunderscore\\b", "\\bvar\\b", "\\bwhere\\b",
        "\\bwhile\\b", "\\bany\\b", "\\bgame\\b", "\\bimport\\b", "\\bopen\\b",
        "\\bset\\b", "\\bget\\b", "\\binfix\\b", "\\bpostfix\\b", "\\bprefix\\b",
        "\\bpackage\\b", "\\brequired\\b", "\\bconvenience\\b", "\\bfinal\\b", "\\bmutating\\b",
        "\\bnonmutating\\b", "\\boptional\\b", "\\bindirect\\b", "\\boverview\\b", "\\bAsync\\b",
        "\\bawait\\b", "\\bactor\\b", "\\bdistributed\\b", "\\bisolated\\b", "\\bnonisolated\\b",
        "\\bmacro\\b", "\\basync\\b"
    };
    
    // Swift specially treated identifiers
    const QStringList specialIdentifiers = {
        "\\bBool\\b", "\\bCharacter\\b", "\\bDouble\\b", "\\bFloat\\b", "\\bInt\\b",
        "\\bString\\b", "\\bUInt\\b", "\\bArray\\b", "\\bDictionary\\b", "\\bSet\\b",
        "\\bResult\\b", "\\bError\\b", "\\bThrowable\\b", "\\bBundle\\b", "\\bData\\b",
        "\\bURL\\b", "\\bOptional\\b", "\\bRange\\b", "\\bClosedRange\\b", "\\bSequence\\b",
        "\\bCollection\\b", "\\bIterator\\b", "\\bBytes\\b", "\\bJSON\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Special identifiers
    for (const QString &pattern : specialIdentifiers) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = specialFormat;
        m_highlightingRules.append(rule);
    }
    
    // Type names (starting with capital letter)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format = typeFormat;
    m_highlightingRules.append(rule);
    
    // Attributes
    rule.pattern = QRegularExpression("@[a-zA-Z_][a-zA-Z0-9_]*\\b");
    rule.format = attributeFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\bfunc\\s+([a-zA-Z_][a-zA-Z0-9_]*)\\s*\\(");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // String literals
    rule.pattern = QRegularExpression("\"(?:\\\\\"|[^\"])*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line string literals
    rule.pattern = QRegularExpression("\"\"\".*\"\"\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?([eE][+-]?\\d+)?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
