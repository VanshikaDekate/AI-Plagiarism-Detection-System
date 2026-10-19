
#include "Paraphraser.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <string>
#include <utility>

using namespace std;

Paraphraser::Paraphraser()
{
    // Phrase replacements are applied before word replacements.
    phraseMap["due to the fact that"] = "because";
    phraseMap["in order to"] = "to";
    phraseMap["a large number of"] = "many";
    phraseMap["as a result of"] = "because of";
    phraseMap["at the present time"] = "currently";

    // Conservative synonyms: avoid common verb substitutions
    // that may break sentence grammar.
    synonyms["important"] = {"significant"};
    synonyms["method"] = {"approach"};
    synonyms["result"] = {"outcome"};
    synonyms["large"] = {"substantial"};
    synonyms["small"] = {"limited"};
    synonyms["difficult"] = {"challenging"};
    synonyms["accurate"] = {"precise"};
    synonyms["simple"] = {"straightforward"};
    synonyms["rapid"] = {"fast"};
}

string Paraphraser::replacePhrases(const string& text)
{
    string result = text;

    for (const auto& entry : phraseMap)
    {
        const string pattern = "\\b" + entry.first + "\\b";
        const regex expression(pattern, regex_constants::icase);

        result = regex_replace(
            result,
            expression,
            entry.second
        );
    }

    return result;
}


string Paraphraser::replaceWords(const string& text)
{
    string result = text;

    for (const auto& entry : synonyms)
    {
        const string pattern = "\\b" + entry.first + "\\b";
        const regex expression(pattern, regex_constants::icase);

        result = regex_replace(
            result,
            expression,
            entry.second.front()
        );
    }

    return result;
}


string Paraphraser::rephrase(const string& text)
{
    if (text.empty())
        return text;

    // Step 1: Replace complete phrases.
    string result = replacePhrases(text);

    // Step 2: Replace selected individual words.
    result = replaceWords(result);

    // Step 3: Capitalize the first alphabetic character
    // of the text and each sentence.
    bool capitalizeNext = true;

    for (char& ch : result)
    {
        unsigned char uch = static_cast<unsigned char>(ch);

        if (capitalizeNext && isalpha(uch))
        {
            ch = static_cast<char>(toupper(uch));
            capitalizeNext = false;
        }
        else if (ch == '.' || ch == '!' || ch == '?')
        {
            capitalizeNext = true;
        }
    }

    return result;
}
