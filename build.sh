#!/bin/bash

# Configuration
BINARY_NAME="svm"
SRC_DIR="src"
TARGET_DIR="target"
BINARY_PATH="$TARGET_DIR/$BINARY_NAME"

# Compiler settings
CC="gcc"
CFLAGS="-Wall -Wextra -std=c99 -O2"
DEBUG_FLAGS="-g -DDEBUG"

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
        print_status "Running $BINARY_PATH..."
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
    "clean"|"c")
        clean
        ;;
    "help"|"h"|"-h"|"--help")
        echo "Usage: $0 [command] [arguments...]"
        echo ""
        echo "Commands:"
        echo "  build, b          Compile the project only"
        echo "  run, r [args]     Compile and run with optional arguments (default)"
        echo "  debug, d [args]   Compile with debug flags and run with optional arguments"
        echo "  clean, c          Remove build artifacts"
        echo "  help, h           Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0                # Compile and run (same as 'run')"
        echo "  $0 run            # Compile and run"
        echo "  $0 run arg1 arg2  # Compile and run with arguments"
        echo "  $0 debug          # Compile with debug flags and run"
        echo "  $0 build          # Just compile"
        ;;
    *)
        print_error "Unknown command: $1"
        print_warning "Use '$0 help' to see available commands"
        exit 1
        ;;
esac
