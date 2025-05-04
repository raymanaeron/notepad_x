#include "jsonhighlighter.h"

JsonLanguage::JsonLanguage()
{
    m_name = "JSON";
    m_fileExtensions << "json" << "jsonc" << "jsonl";
    
    // Define formats for different syntax elements
    QTextCharFormat keyFormat;
    keyFormat.setForeground(Qt::darkBlue);
    keyFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat valueFormat;
    valueFormat.setForeground(QColor(0, 128, 0)); // Dark green
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat literalFormat;
    literalFormat.setForeground(QColor(0, 0, 170)); // Dark blue
    literalFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat commentFormat;  // For JSONC (JSON with comments)
    commentFormat.setForeground(QColor(128, 128, 128)); // Gray
    commentFormat.setFontItalic(true);
    
    // Keys (string before colon)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\"[^\"]*\"(?=\\s*:)");
    rule.format = keyFormat;
    m_highlightingRules.append(rule);
    
    // String values
    rule.pattern = QRegularExpression("(?<=: )\"[^\"]*\"");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Other strings (like in arrays)
    rule.pattern = QRegularExpression("(?<!: )\"[^\"]*\"(?!\\s*:)");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Numbers
    rule.pattern = QRegularExpression("\\b-?(?:0|[1-9]\\d*)(?:\\.\\d+)?(?:[eE][+-]?\\d+)?\\b");
    rule.format = numberFormat;
    m_highlightingRules.append(rule);
    
    // Literals: true, false, null
    rule.pattern = QRegularExpression("\\b(?:true|false|null)\\b");
    rule.format = literalFormat;
    m_highlightingRules.append(rule);
    
    // Comments (for JSONC)
    rule.pattern = QRegularExpression("//[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // Structural characters
    QTextCharFormat structFormat;
    structFormat.setForeground(Qt::darkGray);
    structFormat.setFontWeight(QFont::Bold);
    
    rule.pattern = QRegularExpression("[\\{\\}\\[\\],:]{1}");
    rule.format = structFormat;
    m_highlightingRules.append(rule);
    
    // Set up multiline comment expressions (for JSONC)
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
    m_multiLineCommentFormat = commentFormat;
}
