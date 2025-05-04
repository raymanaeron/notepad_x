#include "cpphighlighter.h"

CppLanguage::CppLanguage()
{
    m_name = "C++";
    m_fileExtensions << "cpp" << "h" << "hpp" << "cc" << "cxx" << "c";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat classFormat;
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat functionFormat;
    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define C++ keywords
    const QStringList keywordPatterns = {
        "\\bclass\\b", "\\bconst\\b", "\\benum\\b", "\\bexplicit\\b",
        "\\bfriend\\b", "\\binline\\b", "\\bnamespace\\b", "\\boperator\\b",
        "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\bsignals\\b",
        "\\bslots\\b", "\\bstatic\\b", "\\bstruct\\b", "\\btemplate\\b",
        "\\btypedef\\b", "\\btypename\\b", "\\bunion\\b", "\\bvirtual\\b",
        "\\bvolatile\\b", "\\bbool\\b", "\\bbreak\\b", "\\bcase\\b",
        "\\bcatch\\b", "\\bchar\\b", "\\bcontinue\\b", "\\bdefault\\b",
        "\\bdelete\\b", "\\bdo\\b", "\\bdouble\\b", "\\belse\\b",
        "\\bfloat\\b", "\\bfor\\b", "\\bif\\b", "\\bint\\b",
        "\\blong\\b", "\\bnew\\b", "\\breturn\\b", "\\bshort\\b",
        "\\bsigned\\b", "\\bsizeof\\b", "\\bstatic_cast\\b", "\\bswitch\\b",
        "\\bthis\\b", "\\bthrow\\b", "\\btry\\b", "\\bunsigned\\b",
        "\\bvoid\\b", "\\bwhile\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Class names (Qt classes starting with Q)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = singleLineCommentFormat;
    m_highlightingRules.append(rule);
    
    // Quotations
    rule.pattern = QRegularExpression("\".*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = quotationFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
