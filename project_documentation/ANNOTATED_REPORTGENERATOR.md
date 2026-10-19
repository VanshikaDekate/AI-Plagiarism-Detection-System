# Annotated `ReportGenerator`

Source: [`ReportGenerator.h`](../ReportGenerator.h), [`ReportGenerator.cpp`](../ReportGenerator.cpp).

## Contract

The header includes the definitions needed for `FeatureVector` and `SimilarityResult`. Each method is `const`: it prints inputs but does not update report state. `<iostream>` provides `cout`; `<iomanip>` provides formatting manipulators.

## `showAIReport` (cpp line 23)

Prints a heading, then applies `fixed << setprecision(2)` to the shared `cout` stream. It prints all seven `FeatureVector` values, score with a percent sign, and the classification. It performs no scoring. Stream formatting persists on `cout` after return, which may affect later numbers.

## `showPlagiarismReport` (line 93)

1. Prints a heading. If `results.empty()`, prints `"No reference documents found."` and returns before `front()`, avoiding invalid access.
2. Sets fixed two-decimal formatting. `rank` begins at 1; each result prints filename and `finalScore * 100`, then increments rank. Iteration assumes the caller already sorted results.
3. `best` is a const reference to `results.front()`, safe after the emptiness guard. The function prints its component and combined scores as percentages.
4. The category uses thresholds on the **fractional** `finalScore`: >=0.70 high, >=0.40 moderate, otherwise low.
5. Matching phrases are printed in vector order, with a counter and a break once the counter exceeds 10; at most ten phrases are shown. If none exist, it prints a no-match message.

Time is O(R+Pshown) for R ranked results and at most ten displayed phrases; output is the main cost.

## `showCombinedReport` (line 232)

Calls `showAIReport`, then `showPlagiarismReport`, then prints an end banner. This is presentation orchestration only. The AI score and similarity result vector are passed by const reference/value as declared; no calculations are recomputed.

## Formatting caveat

`fixed` and `setprecision` alter stream state rather than a temporary formatted object. Current usage consistently prints score decimals, but callers should not assume the original `cout` precision is restored afterward.
