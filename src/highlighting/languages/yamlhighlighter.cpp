#include "yamlhighlighter.h"

YamlLanguage::YamlLanguage()
{
    m_name = "YAML";
    m_fileExtensions << "yaml" << "yml";
    
    // Define formats for different syntax elements
    QTextCharFormat keyFormat;
    keyFormat.setForeground(Qt::darkBlue);
    keyFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat valueFormat;
    valueFormat.setForeground(QColor(0, 128, 0)); // Dark green
    
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(128, 128, 128)); // Gray
    commentFormat.setFontItalic(true);
    
    QTextCharFormat anchorFormat;
    anchorFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    QTextCharFormat directiveFormat;
    directiveFormat.setForeground(QColor(175, 0, 0)); // Dark red
    directiveFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat blockFormat;
    blockFormat.setForeground(QColor(175, 95, 0)); // Brown
    blockFormat.setFontWeight(QFont::Bold);
    
    // Keys
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^\\s*[\\-]?\\s*[^:]*:");
    rule.format = keyFormat;
    m_highlightingRules.append(rule);
    
    // Values (simple values after colon)
    rule.pattern = QRegularExpression(":\\s*[^#\\n]*");
    rule.format = valueFormat;
    m_highlightingRules.append(rule);
    
    // Directives
    rule.pattern = QRegularExpression("^%[a-zA-Z_][a-zA-Z0-9_]*");
    rule.format = directiveFormat;
    m_highlightingRules.append(rule);
    
    // Anchors and Aliases
    rule.pattern = QRegularExpression("&[a-zA-Z0-9_-]+|\\*[a-zA-Z0-9_-]+");
    rule.format = anchorFormat;
    m_highlightingRules.append(rule);
    
    // Double-quoted string
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Single-quoted string
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = stringFormat;
    m_highlightingRules.append(rule);
    
    // Block indicators
    rule.pattern = QRegularExpression("^\\s*[\\-|>|\\|]\\s");
    rule.format = blockFormat;
    m_highlightingRules.append(rule);
    
    // List items
    rule.pattern = QRegularExpression("^\\s*- ");
    rule.format = blockFormat;
    m_highlightingRules.append(rule);
    
    // Comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    m_highlightingRules.append(rule);
    
    // YAML has no multiline comments
    m_commentStartExpression = QRegularExpression("[^]"); // Never match
    m_commentEndExpression = QRegularExpression("[^]");   // Never match
}
