from collections import namedtuple
from enum import Enum
import subprocess
import os
import re

class Color(Enum):
    Blue = '\033[94m'
    Green = '\033[92m'
    Orange = '\033[93m'
    Red = '\033[91m'
    Reset = '\033[0m'
    Bold = '\033[1m'
    Underline = '\033[4m'

class Status(Enum):
    Passed = "Passed"
    Failed = "Failed"

class TestResult:
    def __init__(self, name: str):
        self.name = name
        self.message = None
        self.status = None
    
    def set_passed(self):
        """Set the test status to passed."""
        self.status = Status.Passed

    def set_failed(self, message: str | None = None):
        """Set the test status to failed."""
        self.status = Status.Failed
        self.message = message

    def print(self):
        """Print an appropriately-colored test status to stdout."""
        status_color = ""
        match self.status:
            case Status.Passed:
                status_color = Color.Green.value
            case Status.Failed:
                status_color = Color.Red.value

        message = f" ({self.message})" if self.message else ""
        print(f"{self.name}: {status_color}{self.status.value}{Color.Reset.value}{message}")

DECL_PATTERN = re.compile(r"^([0-9A-Za-z_#]+)\s+(?P<name>[0-9A-Za-z_]+)\(")
HEADER_DIR = "include/fiesta"
BUILD_DIR = "build"
TESTS_DIR = "tests"

Coverage = namedtuple("Coverage", ["percentage", "unused_functions"])

def get_module_functions() -> dict[str, list[str]]:
    """Get each module's list of functions / macros from their header file."""
    module_functions = {}
    headers = os.listdir(HEADER_DIR)
    for header in headers:
        module_name = header.split(".")[0]
        module_functions[module_name] = []
        with open(f"{HEADER_DIR}/{header}", "r", encoding="utf-8") as file:
            reading_prototypes = False
            for line in file:
                # Don't start matching on function names
                # until we're at the delimiter line.
                if line.startswith("/*"):
                    reading_prototypes = True
                if not reading_prototypes:
                    continue
                # Extract each function name
                matches = DECL_PATTERN.match(line)
                if matches:
                    module_functions[module_name].append(matches.group("name"))
    return module_functions

MODULE_FUNCTIONS = get_module_functions()

def float_to_percent_str(percentage: float) -> str:
    """Convert a floating point percentage [0, 1] into a human-friendly string."""
    return f"{percentage * 100:.2f}"

def check_coverage(used_functions: dict[str, list[str]]) -> dict[str, Coverage]:
    """Check which functions in each module are used in that module's tests."""
    module_coverages = {}
    for module, functions in MODULE_FUNCTIONS.items():
        unused_functions = []
        for function in functions:
            if function not in used_functions:
                unused_functions.append(function)

        # If the module has no tests, break out.
        if module not in used_functions.keys():
            break

        module_coverages[module] = Coverage(
            float_to_percent_str(
                len(used_functions[module])
                / len(MODULE_FUNCTIONS[module])
            ),
            unused_functions
        )

    return module_coverages

def get_used_functions(test_filename: str, functions: list[str]) -> list[str]:
    """Get which functions from the provided list are
    actually used in the provided file."""
    used_functions = []
    with open(test_filename, "r", encoding="utf-8") as file:
        text = file.read()
        for function in functions:
            if function in text:
                used_functions.append(function)
    return used_functions

def check_coverages() -> dict[str, Coverage]:
    """Check which functions in each module are used in that module's tests."""
    used_functions = {}
    # Ignore everything in tests/ besides directories.
    test_dirs = filter(
        lambda x: os.path.isdir(os.path.join(TESTS_DIR, x)),
        os.listdir(TESTS_DIR)
    )
    for test_dir in test_dirs:
        test_dir_path = os.path.join(TESTS_DIR, test_dir)
        # Ignore everything in the current test
        # directory besides test source files.
        test_filenames = filter(
            lambda x: x.endswith(".c"),
            os.listdir(test_dir_path)
        )

        used_functions[test_dir] = []
        # Get the list of which of this module's
        # functions are used across its test files.
        for test_filename in test_filenames:
            used_functions[test_dir] += get_used_functions(
                os.path.join(test_dir_path, test_filename),
                MODULE_FUNCTIONS[test_dir]
            )
        # Remove duplicate references to functions.
        used_functions[test_dir] = list(set(used_functions[test_dir]))

    return check_coverage(used_functions)

def get_test_names(module: str) -> list[str]:
    """Get the name of each test for the given module."""
    test_filenames = filter(
        lambda x: x.endswith(".c"),
        os.listdir(os.path.join(TESTS_DIR, module))
    )
    return [x.split(".")[0] for x in test_filenames]

def get_test_exe(test_name: str) -> str:
    """Get the name of the executable for the given test, for the current platform."""
    match os.name:
        case "nt":
            return f"{test_name}.exe"
        case "posix":
            return test_name

def normalize_str(string: str) -> str:
    """Normalize a string's control characters to be platform-agnostic."""
    return string.replace("\r\n", "\n")

def run_test(module: str, test_name: str) -> TestResult:
    """Run the provided from the provided module"""
    test_result = TestResult(test_name)
    test_exe_name = os.path.join(
        BUILD_DIR, get_test_exe(test_name)
    )
    test_name_expected = os.path.join(
        TESTS_DIR, module, f"{test_name}.expected"
    )

    with open(test_name_expected, "r", encoding="utf-8") as file:
        expected = file.read()
        # TODO: Timeout handling.
        actual = subprocess.run(
            args=[test_exe_name],
            capture_output=True
        )

        actual_stdout = normalize_str(actual.stdout.decode("utf-8"))
        actual_stderr = normalize_str(actual.stderr.decode("utf-8"))
        if actual_stdout != expected:
            test_result.set_failed(actual_stderr)
        else:
            test_result.set_passed()

    return test_result

def run_tests() -> dict[str, list[TestResult]]:
    """Run the tests for all modules"""
    test_dirs = filter(
        lambda x: os.path.isdir(os.path.join(TESTS_DIR, x)),
        os.listdir(TESTS_DIR)
    )
    module_results = {}
    for test_dir in test_dirs:
        test_names = get_test_names(test_dir)
        # Run each test and store its result.
        module_results[test_dir] = \
            [run_test(test_dir, test_name) for test_name in test_names]

    return module_results

def main():
    coverages = check_coverages()
    module_results = run_tests()
    print_indent = lambda n: print(" " * n, end="")
    ind = 0
    for module, results in module_results.items():
        print(f"Module: {module} | {coverages[module].percentage}% Coverage")
        ind += 2
        for result in results:
            print_indent(ind)
            result.print()
        ind = 0

if __name__ == "__main__":
    main()
