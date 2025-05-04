#include "xmlhighlighter.h"

XmlLanguage::XmlLanguage()
{
    m_name = "XML";
    m_fileExtensions << "xml" << "xhtml" << "svg" << "xsl" << "xsd" << "rss";
    
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
    commentFormat.setFontItalic(true);
    
    QTextCharFormat cdataFormat;
    cdataFormat.setForeground(QColor(125, 80, 0)); // Dark brown
    
    QTextCharFormat dtdFormat;
    dtdFormat.setForeground(QColor(64, 64, 128)); // Dark blue-gray
    
    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor(0, 128, 128)); // Teal
    
    // XML tags: opening, closing, and self-closing
    HighlightingRule rule;
    rule.pattern = QRegularExpression("</?[A-Za-z0-9_:-]+|/?>|<");
    rule.format = tagFormat;
    m_highlightingRules.append(rule);
    
    // XML attributes
    rule.pattern = QRegularExpression("\\s[A-Za-z0-9_:-]+=");
    rule.format = attributeFormat;
    m_highlightingRules.append(rule);
    
    // XML attribute values
    rule.pattern = QRegularExpression("\"[^\"]*\"|'[^']*'");
    rule.format = attributeValueFormat;
    m_highlightingRules.append(rule);
    
    // XML processing instructions
    rule.pattern = QRegularExpression("<\\?.*\\?>");
    rule.format = dtdFormat;
    m_highlightingRules.append(rule);
    
    // XML entities
    rule.pattern = QRegularExpression("&[a-zA-Z0-9#]+;");
    rule.format = entityFormat;
    m_highlightingRules.append(rule);
    
    // XML DOCTYPE declaration
    rule.pattern = QRegularExpression("<!DOCTYPE\\s+[^>]*>");
    rule.format = dtdFormat;
    m_highlightingRules.append(rule);
    
    // CDATA sections
    rule.pattern = QRegularExpression("<!\\[CDATA\\[.*\\]\\]>");
    rule.pattern.setPatternOptions(QRegularExpression::DotMatchesEverythingOption);
    rule.format = cdataFormat;
    m_highlightingRules.append(rule);
    
    // Set up multiline comment expressions
    m_commentStartExpression = QRegularExpression("<!--");
    m_commentEndExpression = QRegularExpression("-->");
    m_multiLineCommentFormat = commentFormat;
}
