#include "languagedata.h"

LanguageData::LanguageData() : m_name("Plain Text")
{
    // Base implementation for plain text (no highlighting)
    m_fileExtensions << "txt";
    
    // Use a valid regex pattern that will never match anything instead of an invalid pattern
    m_commentStartExpression = QRegularExpression("(?!)"); // This is a valid pattern that never matches
    m_commentEndExpression = QRegularExpression("(?!)");   // Same here
}
