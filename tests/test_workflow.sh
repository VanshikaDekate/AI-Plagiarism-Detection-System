#!/bin/sh
set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 /path/to/AIPlagiarismSystem" >&2
    exit 2
fi

program=$1
failures=0

check_output() {
    name=$1
    input=$2
    expected=$3
    forbidden=${4-}
    output=$(printf '%s' "$input" | "$program" 2>&1) || {
        echo "[FAIL] $name (unexpected nonzero exit)"
        printf '%s\n' "$output"
        failures=$((failures + 1))
        return
    }

    case "$output" in
        *"$expected"*) ;;
        *)
            echo "[FAIL] $name (expected output not found: $expected)"
            printf '%s\n' "$output"
            failures=$((failures + 1))
            return
            ;;
    esac

    if [ -n "$forbidden" ]; then
        case "$output" in
            *"$forbidden"*)
                echo "[FAIL] $name (unexpected output found: $forbidden)"
                failures=$((failures + 1))
                return
                ;;
        esac
    fi

    echo "[PASS] $name"
}

check_failure() {
    name=$1
    input=$2
    expected=$3
    if output=$(printf '%s' "$input" | "$program" 2>&1); then
        echo "[FAIL] $name (expected nonzero exit)"
        printf '%s\n' "$output"
        failures=$((failures + 1))
        return
    fi

    case "$output" in
        *"$expected"*) echo "[PASS] $name" ;;
        *)
            echo "[FAIL] $name (expected output not found: $expected)"
            printf '%s\n' "$output"
            failures=$((failures + 1))
            ;;
    esac
}

check_failure "empty input rejected" "END
" "Please enter valid text"
whitespace_input=$(printf ' \t\r\n')
check_failure "whitespace input rejected" "${whitespace_input}
END
" "Please enter valid text"
check_failure "punctuation-only input rejected" "... !?
END
" "Please enter valid text"

check_output "short input and decline paraphrase" "hi
END
n
" "Total trigrams : 0" "REPHRASED TEXT ANALYSIS"
check_output "invalid yes/no response retries" "alpha beta gamma
END
maybe
n
" "Invalid input. Please enter y or n." "REPHRASED TEXT"
check_output "rephrase without second analysis" "Method is important.
END
y
n
" "Approach is significant." "REPHRASED TEXT ANALYSIS"
check_output "rephrase and analyze branch" "Method is important.
END
y
y
" "BEFORE / AFTER" ""
check_output "before/after similarity uses percent units" "qzxv blorpt flarn.
END
y
y
" "Similarity before : 0.00%" ""
check_output "before/after rephrased similarity uses percent units" "qzxv blorpt flarn.
END
y
y
" "Similarity after  : 0.00%" ""
check_output "EOF at yes/no prompt exits safely" "alpha beta gamma
END
" "Input ended. Exiting safely." ""
check_output "EOF also terminates text collection" "alpha beta gamma
" "Would you like to rephrase" ""

if [ "$failures" -ne 0 ]; then
    echo "$failures workflow test(s) failed."
    exit 1
fi

echo "All workflow tests passed."
