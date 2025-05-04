#include "htmlhighlighter.h"

HtmlLanguage::HtmlLanguage()
{
    m_name = "HTML";
    m_fileExtensions << "html" << "htm" << "xhtml" << "shtml";
    
    // Define formats for different syntax elements
    QTextCharFormat tagFormat;
    tagFormat.setForeground(Qt::darkBlue);
    tagFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat attributeFormat;
    attributeFormat.setForeground(QColor(0, 128, 0)); // Dark green
    attributeFormat.setFontItalic(true);
    
    QTextCharFormat attributeValueFormat;
    attributeValueFormat.setForeground(Qt::darkRed);
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(128, 128, 128)); // Gray
    
    QTextCharFormat doctypeFormat;
    doctypeFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor(0, 128, 128)); // Teal
    
    // HTML tags
    HighlightingRule rule;
    rule.pattern = QRegularExpression("</?[a-zA-Z0-9_:-]+\\b");
    rule.format = tagFormat;
    m_highlightingRules.append(rule);
    
    // Closing angle bracket
    rule.pattern = QRegularExpression(">");
    rule.format = tagFormat;
    m_highlightingRules.append(rule);
    
    // HTML attributes
    rule.pattern = QRegularExpression("\\s+[a-zA-Z0-9_:-]+=");
    rule.format = attributeFormat;
    m_highlightingRules.append(rule);
    
    // Attribute values with double quotes
    rule.pattern = QRegularExpression("\"[^\"]*\"");
    rule.format = attributeValueFormat;
    m_highlightingRules.append(rule);
    
    // Attribute values with single quotes
    rule.pattern = QRegularExpression("'[^']*'");
    rule.format = attributeValueFormat;
    m_highlightingRules.append(rule);
    
    // HTML entities
    rule.pattern = QRegularExpression("&[a-zA-Z0-9#]+;");
    rule.format = entityFormat;
    m_highlightingRules.append(rule);
    
    // DOCTYPE declaration
    rule.pattern = QRegularExpression("<!DOCTYPE\\s+[^>]*>");
    rule.format = doctypeFormat;
    m_highlightingRules.append(rule);
    
    // Multi-line comment expressions
    m_multiLineCommentFormat = commentFormat;
    m_commentStartExpression = QRegularExpression("<!--");
    m_commentEndExpression = QRegularExpression("-->");
    
    // Special scripts and styles handling
    // In a more advanced implementation, we could handle inline JavaScript and CSS
    
    // Void elements (self-closing tags)
    const QStringList voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    // Add special highlighting for void elements if desired
    // For now, they will be formatted like regular tags
}
