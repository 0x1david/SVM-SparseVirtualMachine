#!/bin/bash
# Configuration
BINARY_NAME="svm"
SRC_DIR="src"
TEST_DIR="tests"
TARGET_DIR="target"
BINARY_PATH="$TARGET_DIR/$BINARY_NAME"

# Compiler settings
CC="gcc"
CFLAGS="-Wall -Wextra -std=c99 -O2"
DEBUG_FLAGS="-g -DDEBUG"
TEST_FLAGS="-DDEBUG_TRACE_EXECUTION"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Function to compile the project
compile() {
    print_status "Compiling $BINARY_NAME..."
    
    mkdir -p "$TARGET_DIR"
    
    if [ "$1" = "debug" ]; then
        print_status "Building in debug mode..."
        COMPILE_CMD="$CC $CFLAGS $DEBUG_FLAGS $SRC_DIR/*.c -o $BINARY_PATH"
    else
        COMPILE_CMD="$CC $CFLAGS $SRC_DIR/*.c -o $BINARY_PATH"
    fi
    
    print_status "Running: $COMPILE_CMD"
    
    if $COMPILE_CMD; then
        print_status "Compilation successful! Binary created at $BINARY_PATH"
        return 0
    else
        print_error "Compilation failed!"
        return 1
    fi
}

# Function to compile tests
compile_test() {
    print_status "Compiling tests..."
    
    mkdir -p "$TARGET_DIR"
    
    # Find all test files
    TEST_FILES=$(find "$TEST_DIR" -name "test_*.c" 2>/dev/null)
    
    if [ -z "$TEST_FILES" ]; then
        print_error "No test files found in $TEST_DIR"
        return 1
    fi
    
    # Get all source files except main.c
    SRC_FILES=$(find "$SRC_DIR" -name "*.c" ! -name "main.c")
    
    for test_file in $TEST_FILES; do
        test_name=$(basename "$test_file" .c)
        test_binary="$TARGET_DIR/$test_name"
        
        COMPILE_CMD="$CC $CFLAGS $TEST_FLAGS $test_file $SRC_FILES -o $test_binary"
        
        print_status "Compiling $test_name..."
        
        if $COMPILE_CMD; then
            print_status "✓ $test_name compiled successfully"
        else
            print_error "✗ Failed to compile $test_name"
            return 1
        fi
    done
    
    return 0
}

# Function to run tests
run_tests() {
    if ! compile_test; then
        return 1
    fi
    
    print_status "Running tests..."
    echo "========================================"
    
    TEST_BINARIES=$(find "$TARGET_DIR" -name "test_*" -type f 2>/dev/null)
    
    if [ -z "$TEST_BINARIES" ]; then
        print_error "No test binaries found"
        return 1
    fi
    
    FAILED=0
    PASSED=0
    
    for test_binary in $TEST_BINARIES; do
        test_name=$(basename "$test_binary")
        echo ""
        print_status "Running $test_name..."
        echo "----------------------------------------"
        
        if ./"$test_binary"; then
            PASSED=$((PASSED + 1))
            print_status "✓ $test_name passed"
        else
            FAILED=$((FAILED + 1))
            print_error "✗ $test_name failed"
        fi
        echo "----------------------------------------"
    done
    
    echo ""
    echo "========================================"
    print_status "Test Results: $PASSED passed, $FAILED failed"
    echo "========================================"
    
    if [ $FAILED -eq 0 ]; then
        print_status "All tests passed!"
        return 0
    else
        print_error "Some tests failed!"
        return 1
    fi
}

# Function to compile and run the binary
run() {
    if compile; then
        print_status "Running $BINARY_PATH..."
        echo "----------------------------------------"
        ./"$BINARY_PATH" "$@"
        echo "----------------------------------------"
        print_status "Program finished with exit code $?"
    fi
}

# Function to compile and debug run
debug_run() {
    if compile debug; then
        print_status "Running $BINARY_PATH in debug mode..."
        echo "----------------------------------------"
        ./"$BINARY_PATH" "$@"
        echo "----------------------------------------"
        print_status "Program finished with exit code $?"
    fi
}

# Function to clean build artifacts
clean() {
    print_status "Cleaning build artifacts..."
    rm -rf "$TARGET_DIR"
    print_status "Clean complete!"
}

# Function to show usage
usage() {
    echo "Usage: $0 {build|debug|run|test|clean} [args]"
    echo ""
    echo "Commands:"
    echo "  build, compile, b    Build the project"
    echo "  debug, d             Build with debug flags"
    echo "  run, r [args]        Build and run the program"
    echo "  test, t              Compile and run all tests"
    echo "  clean, c             Remove build artifacts"
    echo ""
}

# Main script logic
case "$1" in
    "build"|"compile"|"b")
        compile
        ;;
    "debug"|"d")
        compile debug
        ;;
    "run"|"r")
        shift
        run "$@"
        ;;
    "test"|"t")
        run_tests
        ;;
    "clean"|"c")
        clean
        ;;
    *)
        usage
        exit 1
        ;;
esac
