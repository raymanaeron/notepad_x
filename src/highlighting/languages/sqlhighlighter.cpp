#include "sqlhighlighter.h"

SqlLanguage::SqlLanguage()
{
    m_name = "SQL";
    m_fileExtensions << "sql" << "ddl" << "dml";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(0, 100, 150));  // Blue
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    commentFormat.setFontItalic(true);
    
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(QColor(120, 120, 120)); // Gray
    
    QTextCharFormat specialFormat;
    specialFormat.setForeground(QColor(150, 20, 100)); // Reddish-purple
    
    // Define SQL keywords
    const QStringList keywordPatterns = {
        "\\bADD\\b", "\\bALL\\b", "\\bALTER\\b", "\\bAND\\b", "\\bANY\\b", "\\bAS\\b", 
        "\\bASC\\b", "\\bBACKUP\\b", "\\bBETWEEN\\b", "\\bBY\\b", "\\bCASE\\b", 
        "\\bCHECK\\b", "\\bCLUSTER\\b", "\\bCOLUMN\\b", "\\bCONSTRAINT\\b", 
        "\\bCREATE\\b", "\\bCROSS\\b", "\\bCURRENT_DATE\\b", "\\bCURRENT_TIME\\b", 
        "\\bCURRENT_TIMESTAMP\\b", "\\bCURRENT_USER\\b", "\\bDATABASE\\b", 
        "\\bDEFAULT\\b", "\\bDELETE\\b", "\\bDESC\\b", "\\bDISTINCT\\b", "\\bDROP\\b", 
        "\\bEACH\\b", "\\bELSE\\b", "\\bEND\\b", "\\bEXCEPT\\b", "\\bEXISTS\\b", 
        "\\bFOREIGN\\b", "\\bFROM\\b", "\\bFULL\\b", "\\bGRANT\\b", "\\bGROUP\\b", 
        "\\bHAVING\\b", "\\bIN\\b", "\\bINDEX\\b", "\\bINNER\\b", "\\bINSERT\\b", 
        "\\bINTERSECT\\b", "\\bINTO\\b", "\\bIS\\b", "\\bJOIN\\b", "\\bKEY\\b", 
        "\\bLEFT\\b", "\\bLIKE\\b", "\\bLIMIT\\b", "\\bNOT\\b", "\\bNULL\\b", 
        "\\bOFFSET\\b", "\\bON\\b", "\\bOR\\b", "\\bORDER\\b", "\\bOUTER\\b", 
        "\\bPRIMARY\\b", "\\bPROCEDURE\\b", "\\bREFERENCES\\b", "\\bRIGHT\\b", 
        "\\bROLLBACK\\b", "\\bROW\\b", "\\bSELECT\\b", "\\bSET\\b", "\\bSOME\\b", 
        "\\bTABLE\\b", "\\bTHEN\\b", "\\bTO\\b", "\\bTOP\\b", "\\bTRANSACTION\\b", 
        "\\bTRUNCATE\\b", "\\bUNION\\b", "\\bUNIQUE\\b", "\\bUPDATE\\b", "\\bUSING\\b", 
        "\\bVALUES\\b", "\\bVIEW\\b", "\\bWHEN\\b", "\\bWHERE\\b", "\\bWITH\\b",
        "\\bBEGIN\\b", "\\bCOMMIT\\b", "\\bREVOKE\\b", "\\bROLLBACK\\b", "\\bSAVEPOINT\\b"
    };
    
    // SQL data types
    const QStringList typePatterns = {
        "\\bBIT\\b", "\\bTINYINT\\b", "\\bSMALLINT\\b", "\\bINT\\b", "\\bBIGINT\\b", 
        "\\bDECIMAL\\b", "\\bNUMERIC\\b", "\\bFLOAT\\b", "\\bREAL\\b", "\\bDATETIME\\b", 
        "\\bSMALLDATETIME\\b", "\\bCHAR\\b", "\\bVARCHAR\\b", "\\bTEXT\\b", "\\bNVARCHAR\\b", 
        "\\bNTEXT\\b", "\\bBINARY\\b", "\\bVARBINARY\\b", "\\bIMAGE\\b", "\\bMONEY\\b", 
        "\\bBOOLEAN\\b", "\\bBOOL\\b", "\\bDATE\\b", "\\bTIME\\b", "\\bTIMESTAMP\\b", 
        "\\bINTERVAL\\b"
    };
    
    // SQL functions
    const QStringList functionPatterns = {
        "\\bABS\\b", "\\bAVG\\b", "\\bCOALESCE\\b", "\\bCONCAT\\b", "\\bCONVERT\\b", 
        "\\bCOUNT\\b", "\\bCURRENT_DATE\\b", "\\bCURRENT_TIME\\b", "\\bDATE\\b", 
        "\\bDATEDIFF\\b", "\\bDATEPART\\b", "\\bFORMAT\\b", "\\bLEN\\b", "\\bLOWER\\b", 
        "\\bMAX\\b", "\\bMIN\\b", "\\bNOW\\b", "\\bROUND\\b", "\\bROW_NUMBER\\b", 
        "\\bSUM\\b", "\\bSUBSTRING\\b", "\\bTRIM\\b", "\\bUPPER\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for data types
    for (const QString &pattern : typePatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = typeFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for functions
    for (const QString &pattern : functionPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = functionFormat;
        m_highlightingRules.append(rule);
    }
    
    // Operators
    HighlightingRule rule;
    rule.pattern = QRegularExpression("=|<|>|<=|>=|<>|!=|\\+|-|\\*|/|%|\\|\\||\\&\\&|!");
    rule.format = operatorFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted strings
    rule.pattern = QRegularExpression("'[^']*'");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted identifiers
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = specialFormat;
    m_highlightingRules.append(rule);
    
    // Backtick-quoted identifiers (MySQL)
    rule.pattern = QRegularExpression("`[^`]*`");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = specialFormat;
    m_highlightingRules.append(rule);
    
    // Bracket-quoted identifiers (SQL Server)
    rule.pattern = QRegularExpression("\\[[^\\]]*\\]");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = specialFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b\\d+\\.\\d*|\\b\\.\\d+|\\b\\d+\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("--[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Database objects (schema.table.column format)
    rule.pattern = QRegularExpression("\\b[a-zA-Z0-9_]+\\.[a-zA-Z0-9_]+(\\.[a-zA-Z0-9_]+)?\\b");
    rule.format = specialFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions (/* ... */)
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
    m_multiLineCommentFormat = commentFormat;
}
