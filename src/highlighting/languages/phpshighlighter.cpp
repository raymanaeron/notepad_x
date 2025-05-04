#include "phpshighlighter.h"

PhpLanguage::PhpLanguage()
{
    m_name = "PHP";
    m_fileExtensions << "php" << "phtml" << "php3" << "php4" << "php5" << "phps";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat builtInFormat;
    builtInFormat.setForeground(QColor(0, 100, 150));  // Blue
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(0, 128, 128)); // Teal
    
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(170, 0, 170)); // Purple
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat phpTagFormat;
    phpTagFormat.setForeground(QColor(128, 0, 0)); // Maroon
    phpTagFormat.setFontWeight(QFont::Bold);
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    m_multiLineCommentFormat.setFontItalic(true);
    
    // PHP tags
    HighlightingRule rule;
    rule.pattern = QRegularExpression("<\\?php|\\?>");
    rule.format = phpTagFormat;
    m_highlightingRules.append(rule);
    
    // PHP keywords
    const QStringList keywordPatterns = {
        "\\barray\\b", "\\bas\\b", "\\bbreak\\b", "\\bcase\\b", "\\bcatch\\b", 
        "\\bclass\\b", "\\bclone\\b", "\\bconst\\b", "\\bcontinue\\b", "\\bdeclare\\b", 
        "\\bdefault\\b", "\\bdo\\b", "\\becho\\b", "\\belse\\b", "\\belseif\\b", 
        "\\bempty\\b", "\\benddeclare\\b", "\\bendfor\\b", "\\bendforeach\\b", 
        "\\bendif\\b", "\\bendswitch\\b", "\\bendwhile\\b", "\\bextends\\b", 
        "\\bfinal\\b", "\\bfinally\\b", "\\bfor\\b", "\\bforeach\\b", "\\bfunction\\b", 
        "\\bglobal\\b", "\\bif\\b", "\\bimplements\\b", "\\binclude\\b", "\\binclude_once\\b", 
        "\\binstanceof\\b", "\\binsteadof\\b", "\\binterface\\b", "\\bisset\\b", 
        "\\blist\\b", "\\bnamespace\\b", "\\bnew\\b", "\\bor\\b", "\\bprint\\b", 
        "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\brequire\\b", 
        "\\brequire_once\\b", "\\breturn\\b", "\\bstatic\\b", "\\bswitch\\b", 
        "\\bthrow\\b", "\\btrait\\b", "\\btry\\b", "\\bunset\\b", "\\buse\\b", 
        "\\bvar\\b", "\\bwhile\\b", "\\byield\\b", "\\bfn\\b", "\\bmatch\\b",
        "\\benum\\b", "\\bfalse\\b", "\\bnull\\b", "\\btrue\\b", "\\band\\b",
        "\\bxor\\b", "\\bdie\\b", "\\bself\\b", "\\bparent\\b", "\\breadonly\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // PHP built-in functions (sample of common ones)
    const QStringList builtInPatterns = {
        "\\bstr_replace\\b", "\\bstrlen\\b", "\\barray_push\\b", "\\bcount\\b",
        "\\bdate\\b", "\\btime\\b", "\\bmysqli_connect\\b", "\\bmysqli_query\\b",
        "\\bmysqli_fetch_assoc\\b", "\\bfile_get_contents\\b", "\\bfile_put_contents\\b",
        "\\bjson_encode\\b", "\\bjson_decode\\b", "\\bpreg_match\\b", "\\bpreg_replace\\b",
        "\\bsprintf\\b", "\\bsubstr\\b", "\\bexplode\\b", "\\bimplode\\b", "\\btrim\\b",
        "\\bheader\\b", "\\bsession_start\\b", "\\bintval\\b", "\\bfloatval\\b"
    };
    
    // Create rules for built-in functions
    for (const QString &pattern : builtInPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = builtInFormat;
        m_highlightingRules.append(rule);
    }
    
    // Variables
    rule.pattern = QRegularExpression("\\$[a-zA-Z_][a-zA-Z0-9_]*\\b");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // Function declarations
    rule.pattern = QRegularExpression("\\bfunction\\s+([a-zA-Z0-9_]+)\\s*\\(");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted strings
    rule.pattern = QRegularExpression("'[^'\\\\]*(\\\\.[^'\\\\]*)*'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted strings
    rule.pattern = QRegularExpression("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Hash comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b[0-9]+\\b|\\b0x[0-9a-fA-F]+\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Set up multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
