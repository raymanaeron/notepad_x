#ifndef CSSHIGHLIGHTER_H
#define CSSHIGHLIGHTER_H

#include "../languagedata.h"

class CssLanguage : public LanguageData
{
public:
    CssLanguage();

protected:
    void setupCssRules();
};

class ScssLanguage : public CssLanguage
{
public:
    ScssLanguage();

protected:
    void setupScssRules();
};

#endif // CSSHIGHLIGHTER_H
