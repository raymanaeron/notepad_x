#include "languagedata.h"

LanguageData::LanguageData() : m_name("Plain Text")
{
    // Base implementation for plain text (no highlighting)
    m_fileExtensions << "txt";
}
