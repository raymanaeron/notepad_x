#include "shellhighlighter.h"

BashLanguage::BashLanguage()
{
    m_name = "Bash";
    m_fileExtensions << "sh" << "bash" << "zsh" << "ksh";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(0, 110, 140)); // Teal
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat commandFormat;
    commandFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    // Define Bash keywords and control structures
    const QStringList keywordPatterns = {
        "\\bif\\b", "\\bthen\\b", "\\belse\\b", "\\belif\\b", "\\bfi\\b", 
        "\\bwhile\\b", "\\bdo\\b", "\\bdone\\b", "\\bfor\\b", "\\bin\\b", 
        "\\bcase\\b", "\\besac\\b", "\\bfunction\\b", "\\breturn\\b", "\\blocal\\b",
        "\\buntil\\b", "\\bselect\\b", "\\btime\\b", "\\bcoproc\\b", "\\b\\{\\b", 
        "\\b\\}\\b", "\\b!\\b", "\\btest\\b", "\\b\\[\\b", "\\b\\]\\b"
    };
    
    // Common Bash commands
    const QStringList commandPatterns = {
        "\\becho\\b", "\\bcd\\b", "\\bls\\b", "\\bgrep\\b", "\\bawk\\b", "\\bsed\\b",
        "\\bcat\\b", "\\bcp\\b", "\\bmv\\b", "\\brm\\b", "\\bmkdir\\b", "\\btouched\\b",
        "\\bfind\\b", "\\bwhich\\b", "\\bexport\\b", "\\bsource\\b", "\\b\\.\\b",
        "\\bchmod\\b", "\\bchown\\b", "\\btail\\b", "\\bhead\\b", "\\bsort\\b",
        "\\buniq\\b", "\\bwc\\b", "\\btar\\b", "\\bzip\\b", "\\bunzip\\b", "\\bgzip\\b",
        "\\bgunzip\\b", "\\bssh\\b", "\\bscp\\b", "\\brsync\\b", "\\bcurl\\b",
        "\\bwget\\b", "\\bpwd\\b", "\\bxargs\\b", "\\bsudo\\b", "\\bsu\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for common commands
    for (const QString &pattern : commandPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = commandFormat;
        m_highlightingRules.append(rule);
    }
    
    // Function declarations
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\bfunction\\s+([a-zA-Z0-9_]+)\\s*\\(|\\b([a-zA-Z0-9_]+)\\s*\\(\\)\\s*\\{");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Variable references ($VAR, ${VAR})
    rule.pattern = QRegularExpression("\\$\\{?[a-zA-Z0-9_]+\\}?");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted strings
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted strings
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Here-documents
    rule.pattern = QRegularExpression("<<-?\\s*['\"](\\w+)['\"]");
    rule.format = keywordFormat;
    m_highlightingRules.append(rule);
    
    // No multi-line comments in Bash
    m_commentStartExpression = QRegularExpression("(?!)"); // Valid pattern that never matches
    m_commentEndExpression = QRegularExpression("(?!)");   // Valid pattern that never matches
}

PowerShellLanguage::PowerShellLanguage()
{
    m_name = "PowerShell";
    m_fileExtensions << "ps1" << "psm1" << "psd1";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(0, 110, 140)); // Teal
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontItalic(true);
    
    QTextCharFormat commandletFormat;
    commandletFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    QTextCharFormat parameterFormat;
    parameterFormat.setForeground(QColor(140, 70, 20)); // Brown
    
    QTextCharFormat operatorFormat;
    operatorFormat.setForeground(QColor(120, 120, 120)); // Gray
    
    // Define PowerShell keywords
    const QStringList keywordPatterns = {
        "\\bbegin\\b", "\\bbreak\\b", "\\bcatch\\b", "\\bclass\\b", "\\bcontinue\\b",
        "\\bdata\\b", "\\bdefine\\b", "\\bdo\\b", "\\bdynamicparam\\b", "\\belse\\b",
        "\\belseif\\b", "\\bend\\b", "\\bexit\\b", "\\bfilter\\b", "\\bfinally\\b",
        "\\bfor\\b", "\\bforeach\\b", "\\bfrom\\b", "\\bfunction\\b", "\\bif\\b",
        "\\bin\\b", "\\bparam\\b", "\\bprocess\\b", "\\breturn\\b", "\\bswitch\\b",
        "\\bthrow\\b", "\\btrap\\b", "\\btry\\b", "\\buntil\\b", "\\busing\\b",
        "\\bvar\\b", "\\bwhile\\b", "\\bworkflow\\b", "\\bTrue\\b", "\\bFalse\\b",
        "\\b\\.\\b", "\\bNot\\b", "\\bAnd\\b", "\\bOr\\b", "\\bXor\\b"
    };
    
    // Common PowerShell cmdlets
    const QStringList cmdletPatterns = {
        "\\bGet-\\w+\\b", "\\bSet-\\w+\\b", "\\bNew-\\w+\\b", "\\bRemove-\\w+\\b",
        "\\bAdd-\\w+\\b", "\\bClear-\\w+\\b", "\\bConvert-\\w+\\b", "\\bCopy-\\w+\\b",
        "\\bExport-\\w+\\b", "\\bImport-\\w+\\b", "\\bInvoke-\\w+\\b", "\\bOut-\\w+\\b",
        "\\bSelect-\\w+\\b", "\\bStart-\\w+\\b", "\\bStop-\\w+\\b", "\\bTest-\\w+\\b",
        "\\bWhere-Object\\b", "\\bForEach-Object\\b", "\\bWrite-Host\\b",
        "\\bWrite-Output\\b", "\\bWrite-Error\\b", "\\bRead-Host\\b"
    };
    
    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for cmdlets
    for (const QString &pattern : cmdletPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = commandletFormat;
        m_highlightingRules.append(rule);
    }
    
    // Function declarations
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\bfunction\\s+([a-zA-Z0-9_-]+)");
    rule.format = functionFormat;
    m_highlightingRules.append(rule);
    
    // Variable references ($VAR)
    rule.pattern = QRegularExpression("\\$[a-zA-Z0-9_:]+");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // Parameters (-Name, -Path etc.)
    rule.pattern = QRegularExpression("-[a-zA-Z][a-zA-Z0-9_]*");
    rule.format = parameterFormat;
    m_highlightingRules.append(rule);
    
    // Operators
    rule.pattern = QRegularExpression("\\+|\\-|\\*|\\/|%|=|!=|==|!|<|>|<=|>=|\\+=|-=|\\*=|\\/=|%=|\\+\\+|--|\\||\\&|\\|\\||\\&\\&|\\?|:");
    rule.format = operatorFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted strings
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted strings
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Here-strings
    rule.pattern = QRegularExpression("@'\r?\n.*\r?\n'@|@\"\r?\n.*\r?\n\"@");
    rule.pattern.setPatternOptions(QRegularExpression::InvertedGreedinessOption | QRegularExpression::DotMatchesEverythingOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // <# Block comments #>
    m_commentStartExpression = QRegularExpression("<#");
    m_commentEndExpression = QRegularExpression("#>");
    m_multiLineCommentFormat = commentFormat;
}

BatchLanguage::BatchLanguage()
{
    m_name = "Batch";
    m_fileExtensions << "bat" << "cmd";
    
    // Define formats for different syntax elements
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(0, 110, 140)); // Teal
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(Qt::darkGreen);
    
    QTextCharFormat labelFormat;
    labelFormat.setForeground(QColor(128, 0, 128)); // Purple
    labelFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat commandFormat;
    commandFormat.setForeground(QColor(170, 40, 0)); // Rust/brown color
    
    // Define batch file keywords and commands
    const QStringList keywordPatterns = {
        "\\bif\\b", "\\bgoto\\b", "\\bcall\\b", "\\becho\\b", "\\belse\\b", 
        "\\bendlocal\\b", "\\berrorlevel\\b", "\\bexist\\b", "\\bexit\\b", 
        "\\bfor\\b", "\\bin\\b", "\\bdo\\b", "\\bnot\\b", "\\bsetlocal\\b", 
        "\\bshift\\b", "\\brem\\b", "\\bthen\\b", "\\bset\\b"
    };
    
    // Common batch commands
    const QStringList commandPatterns = {
        "\\bcopy\\b", "\\bdel\\b", "\\bren\\b", "\\bmove\\b", "\\bdir\\b",
        "\\bmd\\b", "\\bmkdir\\b", "\\brd\\b", "\\brmdir\\b", "\\btype\\b",
        "\\bpause\\b", "\\bcls\\b", "\\bfindstr\\b", "\\bfind\\b", "\\bchdir\\b",
        "\\bcd\\b", "\\bdate\\b", "\\btime\\b", "\\blive\\b", "\\bstart\\b",
        "\\btasklist\\b", "\\btaskkill\\b", "\\bsc\\b", "\\bnet\\b", "\\bipconfig\\b",
        "\\bping\\b", "\\bwmic\\b", "\\bnslookup\\b", "\\btracert\\b", "\\bnetstat\\b",
        "\\bxcopy\\b", "\\battrib\\b", "\\bchkdsk\\b", "\\bcomp\\b", "\\bfc\\b",
        "\\bpushd\\b", "\\bpopd\\b"
    };

    // Create rules for each keyword
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        m_highlightingRules.append(rule);
    }
    
    // Create rules for common commands
    for (const QString &pattern : commandPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = commandFormat;
        m_highlightingRules.append(rule);
    }
    
    // Labels (lines starting with : and no whitespace)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^\\s*:[a-zA-Z0-9_]+");
    rule.format = labelFormat;
    m_highlightingRules.append(rule);
    
    // Variable references (%VAR%, !VAR!)
    rule.pattern = QRegularExpression("%[^%\n]+%|![^!\n]+!");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // Environment variables (%SYSTEMROOT%, etc.)
    rule.pattern = QRegularExpression("%[a-zA-Z0-9_]+%");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // SET and SETLOCAL arguments
    rule.pattern = QRegularExpression("(?<=\\bset\\s+)[a-zA-Z0-9_]+=");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // Comments (REM or :: at beginning of line)
    rule.pattern = QRegularExpression("^\\s*(?:rem\\b|::).*$", QRegularExpression::CaseInsensitiveOption);
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // String literals in echo statements
    rule.pattern = QRegularExpression("(?<=echo\\s+).*$", QRegularExpression::CaseInsensitiveOption);
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // No multi-line comments in batch files
    m_commentStartExpression = QRegularExpression("(?!)"); // Valid pattern that never matches
    m_commentEndExpression = QRegularExpression("(?!)");   // Valid pattern that never matches
}
