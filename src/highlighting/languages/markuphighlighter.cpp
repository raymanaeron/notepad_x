#include "markuphighlighter.h"
#include <QStringList>

MarkupLanguage::MarkupLanguage()
{
    m_name = "Markup";
    m_fileExtensions << "md" << "markdown" << "txt" << "rst" << "adoc";
    
    // Define formats for different syntax elements
    QTextCharFormat headingFormat;
    headingFormat.setForeground(QColor(0, 0, 160)); // Dark blue
    headingFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat emphasisFormat;
    emphasisFormat.setFontItalic(true);
    emphasisFormat.setForeground(QColor(0, 100, 0)); // Dark green
    
    QTextCharFormat strongFormat;
    strongFormat.setFontWeight(QFont::Bold);
    strongFormat.setForeground(QColor(160, 0, 0)); // Dark red
    
    QTextCharFormat codeFormat;
    // Replace deprecated setFontFamily with setFontFamilies
    QStringList fontFamilies;
    fontFamilies << "Courier" << "Consolas" << "Monospace";
    codeFormat.setFontFamilies(fontFamilies);
    codeFormat.setBackground(QColor(240, 240, 240)); // Light gray
    codeFormat.setForeground(QColor(80, 80, 80));   // Dark gray
    
    QTextCharFormat linkFormat;
    linkFormat.setForeground(QColor(0, 0, 255)); // Blue
    linkFormat.setFontUnderline(true);
    
    QTextCharFormat imageFormat;
    imageFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    QTextCharFormat blockquoteFormat;
    blockquoteFormat.setForeground(QColor(128, 128, 128)); // Gray
    blockquoteFormat.setFontItalic(true);
    
    QTextCharFormat listFormat;
    listFormat.setForeground(QColor(120, 60, 0)); // Brown
    listFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat codeBlockFormat;
    // Replace deprecated setFontFamily with setFontFamilies
    codeBlockFormat.setFontFamilies(fontFamilies);
    codeBlockFormat.setBackground(QColor(240, 240, 240)); // Light gray
    codeBlockFormat.setForeground(QColor(80, 80, 80));   // Dark gray
    
    // Headings (# Heading)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^#+ .*$");
    rule.format = headingFormat;
    m_highlightingRules.append(rule);
    
    // Alternative heading style (Heading\n======)
    rule.pattern = QRegularExpression("^[^\\s].*\\n[=]+$");
    rule.format = headingFormat;
    m_highlightingRules.append(rule);
    
    // Alternative heading style (Heading\n------)
    rule.pattern = QRegularExpression("^[^\\s].*\\n[-]+$");
    rule.format = headingFormat;
    m_highlightingRules.append(rule);
    
    // Emphasis (*italic* or _italic_)
    rule.pattern = QRegularExpression("\\*[^\\*\\n]+\\*|_[^_\\n]+_");
    rule.format = emphasisFormat;
    m_highlightingRules.append(rule);
    
    // Strong (**bold** or __bold__)
    rule.pattern = QRegularExpression("\\*\\*[^\\*\\n]+\\*\\*|__[^_\\n]+__");
    rule.format = strongFormat;
    m_highlightingRules.append(rule);
    
    // Inline code (`code`)
    rule.pattern = QRegularExpression("`[^`\\n]+`");
    rule.format = codeFormat;
    m_highlightingRules.append(rule);
    
    // Links [text](url)
    rule.pattern = QRegularExpression("\\[([^\\[\\]]+)\\]\\(([^\\(\\)]+)\\)");
    rule.format = linkFormat;
    m_highlightingRules.append(rule);
    
    // Reference-style links [text][id]
    rule.pattern = QRegularExpression("\\[([^\\[\\]]+)\\]\\s*\\[[^\\[\\]]+\\]");
    rule.format = linkFormat;
    m_highlightingRules.append(rule);
    
    // Reference-style link definitions [id]: url
    rule.pattern = QRegularExpression("^\\s*\\[[^\\[\\]]+\\]:\\s+.*$");
    rule.format = linkFormat;
    m_highlightingRules.append(rule);
    
    // Images ![alt](url)
    rule.pattern = QRegularExpression("!\\[([^\\[\\]]+)\\]\\(([^\\(\\)]+)\\)");
    rule.format = imageFormat;
    m_highlightingRules.append(rule);
    
    // Blockquotes (> text)
    rule.pattern = QRegularExpression("^\\s*>.*$");
    rule.format = blockquoteFormat;
    m_highlightingRules.append(rule);
    
    // Lists (- item or * item or + item or 1. item)
    rule.pattern = QRegularExpression("^\\s*([-*+]|\\d+\\.)\\s");
    rule.format = listFormat;
    m_highlightingRules.append(rule);
    
    // Code blocks (indented by 4 spaces or 1 tab)
    rule.pattern = QRegularExpression("^(\\t|    ).*$");
    rule.format = codeBlockFormat;
    m_highlightingRules.append(rule);
    
    // Fenced code blocks (```language\ncode\n```)
    rule.pattern = QRegularExpression("^```.*$");
    rule.format = codeBlockFormat;
    m_highlightingRules.append(rule);
    
    // Horizontal rules (---, ***, ___)
    rule.pattern = QRegularExpression("^\\s*([-*_])\\s*\\1\\s*\\1(\\s*\\1)*\\s*$");
    rule.format = headingFormat; // Reusing heading format
    m_highlightingRules.append(rule);
    
    // HTML tags
    rule.pattern = QRegularExpression("</?[a-zA-Z0-9_:-]+\\b[^>]*>");
    rule.format = codeFormat; // Reusing the code format
    m_highlightingRules.append(rule);
    
    // No multi-line comments in Markdown, use a valid pattern that will never match
    m_commentStartExpression = QRegularExpression("(?!)"); // Valid pattern that never matches
    m_commentEndExpression = QRegularExpression("(?!)");   // Valid pattern that never matches
}
