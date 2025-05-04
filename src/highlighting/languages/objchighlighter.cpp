#include "objchighlighter.h"

ObjCLanguage::ObjCLanguage()
{
    m_name = "Objective-C";
    m_fileExtensions << "m" << "mm" << "h";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat typeFormat;
    typeFormat.setForeground(Qt::darkMagenta);
    typeFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat objcSpecificFormat;
    objcSpecificFormat.setForeground(QColor(180, 30, 90)); // Reddish-purple
    objcSpecificFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(QColor(128, 64, 0)); // Brown
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    // Store multi-line comment format
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    
    // Define C/C++ keywords (shared with Objective-C)
    const QStringList keywordPatterns = {
        "\\bbreak\\b", "\\bcase\\b", "\\bchar\\b", "\\bconst\\b", "\\bcontinue\\b",
        "\\bdefault\\b", "\\bdo\\b", "\\bdouble\\b", "\\belse\\b", "\\benum\\b",
        "\\bextern\\b", "\\bfloat\\b", "\\bfor\\b", "\\bgoto\\b", "\\bif\\b",
        "\\bint\\b", "\\blong\\b", "\\bregister\\b", "\\breturn\\b", "\\bshort\\b",
        "\\bsigned\\b", "\\bsizeof\\b", "\\bstatic\\b", "\\bstruct\\b", "\\bswitch\\b",
        "\\btypedef\\b", "\\bunion\\b", "\\bunsigned\\b", "\\bvoid\\b", "\\bvolatile\\b",
        "\\bwhile\\b", "\\bauto\\b", "\\bconst\\b", "\\binline\\b"
    };
    
    // Objective-C specific keywords
    const QStringList objcKeywordPatterns = {
        "\\b@interface\\b", "\\b@implementation\\b", "\\b@protocol\\b", "\\b@end\\b",
        "\\b@private\\b", "\\b@protected\\b", "\\b@public\\b", "\\b@try\\b", "\\b@catch\\b",
        "\\b@finally\\b", "\\b@throw\\b", "\\b@synthesize\\b", "\\b@dynamic\\b", 
        "\\b@property\\b", "\\bid\\b", "\\b@selector\\b", "\\b@class\\b", "\\b@encode\\b",
        "\\bself\\b", "\\bsuper\\b", "\\b_cmd\\b", "\\b@synchronized\\b", "\\b@autoreleasepool\\b",
        "\\b@YES\\b", "\\b@NO\\b", "\\b@true\\b", "\\b@false\\b", "\\bNO\\b", "\\bYES\\b",
        "\\bnil\\b", "\\bNULL\\b", "\\bSEL\\b", "\\bunsafe_unretained\\b", "\\bweak\\b",
        "\\bstrong\\b", "\\bretain\\b", "\\bnonatomic\\b", "\\batomic\\b", "\\breadonly\\b",
        "\\breadwrite\\b", "\\bcopy\\b", "\\bassign\\b", "\\bNSObject\\b", "\\bNSString\\b",
        "\\bNSArray\\b", "\\bNSDictionary\\b", "\\bNSNumber\\b", "\\b@import\\b", "\\b@optional\\b",
        "\\b@required\\b", "\\bNS_ENUM\\b", "\\bNS_OPTIONS\\b", "\\bin\\b", "\\bout\\b", "\\binout\\b"
    };
    
    // Create rules for each C/C++ keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for each Objective-C keyword
    for (const QString &pattern : objcKeywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = objcSpecificFormat;
        m_highlightingRules.append(rule);
    }
    
    // Objective-C method declarations (- or + followed by return type and method name)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^\\s*[+-]\\s*\\([^)]*\\)");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Method selectors in square brackets
    rule.pattern = QRegularExpression("\\[\\s*[a-zA-Z0-9_]+\\s+[a-zA-Z0-9_:]+\\s*.*\\]");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Class names (starting with NS or UI or capital letter)
    rule.pattern = QRegularExpression("\\bNS[A-Za-z]+\\b|\\bUI[A-Za-z]+\\b|\\b[A-Z][a-zA-Z0-9_]*\\b");
    rule.format = typeFormat;
    m_highlightingRules.append(rule);
    
    // Preprocessor directives
    rule.pattern = QRegularExpression("^\\s*#\\s*[a-zA-Z]+");
    rule.format = preprocessorFormat;
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
    
    // Objective-C string literals (@"string")
    rule.pattern = QRegularExpression("@\"(?:\\\\\"|[^\"])*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Numbers (decimal, hex, octal and floating-point)
    rule.pattern = QRegularExpression("\\b\\d+\\.\\d*([eE][+-]?\\d+)?[fFlL]?\\b|\\b\\.\\d+([eE][+-]?\\d+)?[fFlL]?\\b|\\b\\d+[eE][+-]?\\d+[fFlL]?\\b|\\b0x[0-9a-fA-F]+[uUlL]*\\b|\\b\\d+[uUlL]*\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}
