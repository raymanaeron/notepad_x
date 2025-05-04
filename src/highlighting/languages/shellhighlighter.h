#ifndef SHELLHIGHLIGHTER_H
#define SHELLHIGHLIGHTER_H

#include "../languagedata.h"

class BashLanguage : public LanguageData
{
public:
    BashLanguage();
};

class PowerShellLanguage : public LanguageData
{
public:
    PowerShellLanguage();
};

class BatchLanguage : public LanguageData
{
public:
    BatchLanguage();
};

#endif // SHELLHIGHLIGHTER_H
