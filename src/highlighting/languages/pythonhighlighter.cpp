#include "pythonhighlighter.h"

PythonLanguage::PythonLanguage()
{
    m_name = "Python";
    m_fileExtensions << "py" << "pyw" << "pyi";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat builtinsFormat;
    builtinsFormat.setForeground(Qt::darkCyan);
    
    QTextCharFormat selfFormat;
    selfFormat.setForeground(QColor(150, 0, 150)); // Purple
    selfFormat.setFontItalic(true);
    
    QTextCharFormat decoratorFormat;
    decoratorFormat.setForeground(QColor(125, 80, 120)); // Light purple
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    // Define Python keywords
    const QStringList keywordPatterns = {
        "\\band\\b", "\\bas\\b", "\\bassert\\b", "\\bbreak\\b", "\\bclass\\b",
        "\\bcontinue\\b", "\\bdef\\b", "\\bdel\\b", "\\belif\\b", "\\belse\\b",
        "\\bexcept\\b", "\\bfinally\\b", "\\bfor\\b", "\\bfrom\\b", "\\bglobal\\b",
        "\\bif\\b", "\\bimport\\b", "\\bin\\b", "\\bis\\b", "\\blambda\\b",
        "\\bnonlocal\\b", "\\bnot\\b", "\\bor\\b", "\\bpass\\b", "\\braise\\b",
        "\\breturn\\b", "\\btry\\b", "\\bwhile\\b", "\\bwith\\b", "\\byield\\b",
        "\\bTrue\\b", "\\bFalse\\b", "\\bNone\\b", "\\basync\\b", "\\bawait\\b"
    };
    
    // Python built-in functions
    const QStringList builtinPatterns = {
        "\\babs\\b", "\\ball\\b", "\\bany\\b", "\\bascii\\b", "\\bbin\\b",
        "\\bbool\\b", "\\bbytearray\\b", "\\bbytes\\b", "\\bcallable\\b",
        "\\bchr\\b", "\\bclassmethod\\b", "\\bcompile\\b", "\\bcomplex\\b",
        "\\bdict\\b", "\\bdir\\b", "\\bdivmod\\b", "\\benumerate\\b", "\\beval\\b",
        "\\bexec\\b", "\\bfilter\\b", "\\bfloat\\b", "\\bformat\\b", "\\bfrozenset\\b",
        "\\bgetattr\\b", "\\bglobals\\b", "\\bhasattr\\b", "\\bhash\\b", "\\bhelp\\b",
        "\\bhex\\b", "\\bid\\b", "\\binput\\b", "\\bint\\b", "\\bisinstance\\b",
        "\\bissubclass\\b", "\\biter\\b", "\\blen\\b", "\\blist\\b", "\\blocals\\b",
        "\\bmap\\b", "\\bmax\\b", "\\bmemoryview\\b", "\\bmin\\b", "\\bnext\\b",
        "\\bobject\\b", "\\boct\\b", "\\bopen\\b", "\\bord\\b", "\\bpow\\b",
        "\\bprint\\b", "\\bproperty\\b", "\\brange\\b", "\\brepr\\b", "\\breversed\\b",
        "\\bround\\b", "\\bset\\b", "\\bsetattr\\b", "\\bslice\\b", "\\bsorted\\b",
        "\\bstaticmethod\\b", "\\bstr\\b", "\\bsum\\b", "\\bsuper\\b", "\\btuple\\b",
        "\\btype\\b", "\\bvars\\b", "\\bzip\\b", "\\b__import__\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for built-in functions
    for (const QString &pattern : builtinPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = builtinsFormat;
        m_highlightingRules.append(rule);
    }
    
    // 'self' parameter
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\bself\\b");
    rule.format = selfFormat;
    m_highlightingRules.append(rule);
    
    // 'cls' parameter for class methods
    rule.pattern = QRegularExpression("\\bcls\\b");
    rule.format = selfFormat;
    m_highlightingRules.append(rule);
    
    // Decorators
    rule.pattern = QRegularExpression("@[A-Za-z0-9_.]+");
    rule.format = decoratorFormat;
    m_highlightingRules.append(rule);
    
    // Single-line comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // String literals (double quotes)
    rule.pattern = QRegularExpression("\"(?:\\\\\"|[^\"])*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // String literals (single quotes)
    rule.pattern = QRegularExpression("'(?:\\\\'|[^'])*'");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Function definitions
    rule.pattern = QRegularExpression("\\bdef\\s+([A-Za-z0-9_]+)\\s*\\(");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Class definitions
    rule.pattern = QRegularExpression("\\bclass\\s+([A-Za-z0-9_]+)\\s*[:\\(]");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b\\d+\\.\\d*([eE][+-]?\\d+)?\\b|\\b\\d+[eE][+-]?\\d+\\b|\\b0[xX][0-9a-fA-F]+\\b|\\b\\d+\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Python multiline strings don't use the C-style /* */ syntax,
    // but we'll implement it for consistent behavior with docstrings
    m_multiLineCommentFormat.setForeground(Qt::darkGreen);
    m_commentStartExpression = QRegularExpression("(\"\"\"|\'\'\')");
    m_commentEndExpression = QRegularExpression("(\"\"\"|\'\'\')");
}
