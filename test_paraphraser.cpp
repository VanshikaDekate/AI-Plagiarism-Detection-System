
#include <iostream>
#include <string>
#include "Paraphraser.h"

using namespace std;

int main()
{
    Paraphraser paraphraser;
    int failures = 0;

    auto check = [&](const string& name,
                     const string& input,
                     const string& expected)
    {
        string actual = paraphraser.rephrase(input);

        if (actual == expected)
        {
            cout << "[PASS] " << name << '\n';
        }
        else
        {
            cout << "[FAIL] " << name << '\n';
            cout << "  Expected: " << expected << '\n';
            cout << "  Actual:   " << actual << '\n';
            ++failures;
        }
    };

    check("Phrase replacement",
          "Due to the fact that the result is large, it matters!",
          "Because the outcome is substantial, it matters!");

    check("Capitalization",
          "Method is important.",
          "Approach is significant.");

    check("Word boundary",
          "The methodology is important.",
          "The methodology is significant.");

    check("Technical terms preserved",
          "AI, C++, ESP32, and 2026 are technical terms.",
          "AI, C++, ESP32, and 2026 are technical terms.");

    check("Empty input",
          "",
          "");

    cout << "\n";
    if (failures == 0)
    {
        cout << "All tests passed.\n";
        return 0;
    }

    cout << failures << " test(s) failed.\n";
    return 1;
}
