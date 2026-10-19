
#include <iostream>
#include <vector>
#include <string>
#include "Paraphraser.h"

using namespace std;

int main()
{
    Paraphraser paraphraser;

    vector<string> tests = {
        "the method is important.",
        "Due to the fact that the result is large, it matters!",
        "This method is useful. The result is important.",
        "The methodology is important.",
        "",
        "AI, C++, ESP32, and 2026 are technical terms."
    };

    for (size_t i = 0; i < tests.size(); i++)
    {
        cout << "\nTest " << i + 1 << ":\n";
        cout << "Input:  " << tests[i] << '\n';
        cout << "Output: " << paraphraser.rephrase(tests[i]) << '\n';
    }

    return 0;
}
