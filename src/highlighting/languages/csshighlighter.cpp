#include "csshighlighter.h"

CssLanguage::CssLanguage()
{
    m_name = "CSS";
    m_fileExtensions << "css";
    
    setupCssRules();
}

ScssLanguage::ScssLanguage()
{
    m_name = "SCSS";
    m_fileExtensions << "scss" << "sass" << "less";
    
    // Since ScssLanguage now inherits from CssLanguage,
    // we don't need to call setupCssRules() again
    setupScssRules();
}

void CssLanguage::setupCssRules()
{
    // Define formats for different syntax elements
    QTextCharFormat selectorFormat;
    selectorFormat.setForeground(Qt::darkBlue);
    selectorFormat.setFontWeight(QFont::Bold);
    
    QTextCharFormat propertyFormat;
    propertyFormat.setForeground(QColor(170, 0, 0)); // Dark red
    
    QTextCharFormat valueFormat;
    valueFormat.setForeground(QColor(0, 110, 40)); // Dark green
    
    QTextCharFormat unitFormat;
    unitFormat.setForeground(QColor(0, 110, 40)); // Dark green
    
    QTextCharFormat colorFormat;
    colorFormat.setForeground(QColor(128, 0, 128)); // Purple
    
    QTextCharFormat pseudoFormat;
    pseudoFormat.setForeground(QColor(0, 110, 110)); // Teal
    
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(128, 128, 128)); // Gray
    
    QTextCharFormat importantFormat;
    importantFormat.setForeground(Qt::red);
    importantFormat.setFontWeight(QFont::Bold);
    
    // CSS selectors (class, id, elements)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^[\\s]*[.#]?[a-zA-Z0-9_-]+[^{]*");
    rule.format = selectorFormat;
    m_highlightingRules.append(rule);
    
    // CSS properties
    rule.pattern = QRegularExpression("[a-zA-Z-]+(?=\\s*:)");
    rule.format = propertyFormat;
    m_highlightingRules.append(rule);
    
    // CSS values (general)
    rule.pattern = QRegularExpression(":\\s*[^;]*");
    rule.format = valueFormat;
    m_highlightingRules.append(rule);
    
    // CSS units
    rule.pattern = QRegularExpression("\\d+(\\.\\d+)?(px|em|rem|%|ex|ch|vh|vw|vmin|vmax|pt|pc|in|cm|mm|s|ms|deg|rad|turn)");
    rule.format = unitFormat;
    m_highlightingRules.append(rule);
    
    // CSS color values (#hex, rgb, etc)
    rule.pattern = QRegularExpression("#([0-9a-fA-F]{3}|[0-9a-fA-F]{6})\\b|rgb\\([^)]*\\)|rgba\\([^)]*\\)|hsl\\([^)]*\\)|hsla\\([^)]*\\)");
    rule.format = colorFormat;
    m_highlightingRules.append(rule);
    
    // CSS pseudo-classes and pseudo-elements
    rule.pattern = QRegularExpression(":[a-zA-Z-]+");
    rule.format = pseudoFormat;
    m_highlightingRules.append(rule);
    rule.pattern = QRegularExpression("::[a-zA-Z-]+");
    rule.format = pseudoFormat;
    m_highlightingRules.append(rule);
    
    // CSS @-rules
    rule.pattern = QRegularExpression("@[a-zA-Z-]+");
    rule.format = pseudoFormat;
    m_highlightingRules.append(rule);
    
    // !important
    rule.pattern = QRegularExpression("!important");
    rule.format = importantFormat;
    m_highlightingRules.append(rule);
    
    // Store multi-line comment format
    m_multiLineCommentFormat = commentFormat;
    m_commentStartExpression = QRegularExpression("/\\*");
    m_commentEndExpression = QRegularExpression("\\*/");
}

void ScssLanguage::setupScssRules()
{
    // Additional formats for SCSS/SASS/LESS specific elements
    QTextCharFormat variableFormat;
    variableFormat.setForeground(QColor(0, 80, 200)); // Blue
    
    QTextCharFormat mixinFormat;
    mixinFormat.setForeground(QColor(170, 0, 170)); // Purple
    
    // SCSS/SASS variables
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\$[a-zA-Z0-9_-]+");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // LESS variables
    rule.pattern = QRegularExpression("@[a-zA-Z0-9_-]+");
    rule.format = variableFormat;
    m_highlightingRules.append(rule);
    
    // SCSS/SASS mixins
    rule.pattern = QRegularExpression("@mixin\\s+[a-zA-Z0-9_-]+|@include\\s+[a-zA-Z0-9_-]+");
    rule.format = mixinFormat;
    m_highlightingRules.append(rule);
    
    // SCSS/SASS control directives
    rule.pattern = QRegularExpression("@if|@else|@for|@each|@while");
    rule.format = mixinFormat;
    m_highlightingRules.append(rule);
    
    // SCSS/SASS nesting properties
    rule.pattern = QRegularExpression("&");
    rule.format = mixinFormat;
    m_highlightingRules.append(rule);
    
    // LESS mixins
    rule.pattern = QRegularExpression("\\.([a-zA-Z0-9_-]+)\\s*\\(.*\\)");
    rule.format = mixinFormat;
    m_highlightingRules.append(rule);
}
