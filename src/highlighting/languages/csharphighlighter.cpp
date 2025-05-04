#include "csharphighlighter.h"

CSharpLanguage::CSharpLanguage()
{
    m_name = "C#";
    m_fileExtensions << "cs";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(128, 64, 0)); // Brown
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor(77, 77, 153)); // Dark blue-purple
    attributeFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define C# keywords
    const QStringList keywordPatterns = {
        "\\babstract\\b", "\\bas\\b", "\\bbase\\b", "\\bbool\\b", "\\bbreak\\b", 
        "\\bbyte\\b", "\\bcase\\b", "\\bcatch\\b", "\\bchar\\b", "\\bchecked\\b", 
        "\\bclass\\b", "\\bconst\\b", "\\bcontinue\\b", "\\bdecimal\\b", "\\bdefault\\b", 
        "\\bdelegate\\b", "\\bdo\\b", "\\bdouble\\b", "\\belse\\b", "\\benum\\b", 
        "\\bevent\\b", "\\bexplicit\\b", "\\bextern\\b", "\\bfalse\\b", "\\bfinally\\b", 
        "\\bfixed\\b", "\\bfloat\\b", "\\bfor\\b", "\\bforeach\\b", "\\bgoto\\b", 
        "\\bif\\b", "\\bimplicit\\b", "\\bin\\b", "\\bint\\b", "\\binterface\\b", 
        "\\binternal\\b", "\\bis\\b", "\\block\\b", "\\blong\\b", "\\bnamespace\\b", 
        "\\bnew\\b", "\\bnull\\b", "\\bobject\\b", "\\boperator\\b", "\\bout\\b", 
        "\\boverride\\b", "\\bparams\\b", "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", 
        "\\breadonly\\b", "\\bref\\b", "\\breturn\\b", "\\bsbyte\\b", "\\bsealed\\b", 
        "\\bshort\\b", "\\bsizeof\\b", "\\bstackalloc\\b", "\\bstatic\\b", "\\bstring\\b", 
        "\\bstruct\\b", "\\bswitch\\b", "\\bthis\\b", "\\bthrow\\b", "\\btrue\\b", 
        "\\btry\\b", "\\btypeof\\b", "\\buint\\b", "\\bulong\\b", "\\bunchecked\\b", 
        "\\bunsafe\\b", "\\bushort\\b", "\\busing\\b", "\\bvirtual\\b", "\\bvoid\\b", 
        "\\bvolatile\\b", "\\bwhile\\b", "\\badd\\b", "\\balias\\b", "\\bascending\\b", 
        "\\basync\\b", "\\bawait\\b", "\\bdescending\\b", "\\bdynamic\\b", "\\bfrom\\b", 
        "\\bget\\b", "\\bglobal\\b", "\\bgroup\\b", "\\binto\\b", "\\bjoin\\b", 
        "\\blet\\b", "\\bnameof\\b", "\\borderby\\b", "\\bpartial\\b", "\\bremove\\b", 
        "\\bselect\\b", "\\bset\\b", "\\bvalue\\b", "\\bvar\\b", "\\bwhen\\b", 
        "\\bwhere\\b", "\\byield\\b"
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
    
    // Preprocessor directives
    rule.pattern = QRegularExpression("#\\w+");
    rule.format = preprocessorFormat;
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
    
    // Verbatim string literals (@"...")
    rule.pattern = QRegularExpression("@\"[^\"]*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\s*\\()");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Attributes
    rule.pattern = QRegularExpression("\\[\\s*[A-Za-z0-9_]+\\s*\\]");
    rule.format = attributeFormat;
    m_highlightingRules.append(rule);
    
    // Numbers (decimal, hex, and floating-point)
    rule.pattern = QRegularExpression("\\b\\d+\\.\\d*([eE][+-]?\\d+)?[fFdDmM]?\\b|\\b\\.\\d+([eE][+-]?\\d+)?[fFdDmM]?\\b|\\b\\d+[eE][+-]?\\d+[fFdDmM]?\\b|\\b0x[0-9a-fA-F]+\\b|\\b\\d+[fFdDmM]?\\b|\\b\\d+[lL]?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
