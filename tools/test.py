import subprocess
from enum import Enum

class Color(Enum):
    Blue = '\033[94m'
    Green = '\033[92m'
    Orange = '\033[93m'
    Red = '\033[91m'
    Reset = '\033[0m'
    Bold = '\033[1m'
    Underline = '\033[4m'

class Result(Enum):
    Passed = "Passed"
    Failed = "Failed"

UTILITIES = ["str", "file"]

def report_test_result(module: str, result: Result):
    result_color = ""
    match result:
        case Result.Passed:
            result_color = Color.Green.value
        case Result.Failed:
            result_color = Color.Red.value

    print(f"Module {module}: {result_color}{result.value}{Color.Reset.value}")

for utility in UTILITIES:
    expected = ""
    with open(f"tests/{utility}/expected.txt", "r", encoding="utf-8") as file:
        expected = file.read()

    actual = subprocess.run(
        ["make", "tests"],
        stdout=subprocess.PIPE
    )

    if actual.stdout != expected:
        report_test_result(utility, Result.Failed)
    else:
        report_test_result(utility, Result.Passed)