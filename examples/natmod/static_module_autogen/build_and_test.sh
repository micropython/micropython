#!/bin/bash
# Build and test dual-mode modules

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Dual-Mode Module Build & Test Script${NC}"
echo "======================================"

# Get the directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MPY_DIR="$SCRIPT_DIR/../../.."

# Function to build native modules
build_native_modules() {
    echo -e "\n${YELLOW}Building native modules (.mpy files)...${NC}"
    cd "$SCRIPT_DIR"
    
    # Clean previous builds
    rm -f *.mpy *.o
    
    # Build using dynruntime.mk
    make -f Makefile.natmod clean
    make -f Makefile.natmod ARCH=${ARCH:-x64}
    
    if [ -f dualmode_simple.mpy ] && [ -f dualmode_example.mpy ]; then
        echo -e "${GREEN}✓ Native modules built successfully${NC}"
        ls -la *.mpy
        return 0
    else
        echo -e "${RED}✗ Failed to build native modules${NC}"
        return 1
    fi
}

# Function to test native modules
test_native_modules() {
    echo -e "\n${YELLOW}Testing native modules...${NC}"
    cd "$SCRIPT_DIR"
    
    # Test with micropython (requires it to be in PATH or use full path)
    if command -v micropython &> /dev/null; then
        micropython test_dualmode.py
    elif [ -x "$MPY_DIR/ports/unix/build-standard/micropython" ]; then
        "$MPY_DIR/ports/unix/build-standard/micropython" test_dualmode.py
    else
        echo -e "${RED}✗ micropython not found in PATH or standard build location${NC}"
        echo "Please build the Unix port first or add micropython to PATH"
        return 1
    fi
}

# Function to build firmware with user C modules
build_firmware() {
    echo -e "\n${YELLOW}Building firmware with user C modules...${NC}"
    
    PORT=${PORT:-unix}
    echo "Building for port: $PORT"
    
    cd "$MPY_DIR/ports/$PORT"
    
    # Clean and build with user C modules
    make clean
    make USER_C_MODULES="$SCRIPT_DIR" -j4
    
    if [ -f "build-standard/micropython" ]; then
        echo -e "${GREEN}✓ Firmware built successfully with user C modules${NC}"
        return 0
    else
        echo -e "${RED}✗ Failed to build firmware${NC}"
        return 1
    fi
}

# Function to test user C modules
test_user_c_modules() {
    echo -e "\n${YELLOW}Testing user C modules in firmware...${NC}"
    
    PORT=${PORT:-unix}
    MICROPYTHON="$MPY_DIR/ports/$PORT/build-standard/micropython"
    
    if [ -x "$MICROPYTHON" ]; then
        cd "$SCRIPT_DIR"
        "$MICROPYTHON" test_dualmode.py
    else
        echo -e "${RED}✗ Firmware executable not found${NC}"
        return 1
    fi
}

# Main menu
show_menu() {
    echo -e "\n${YELLOW}Choose what to build and test:${NC}"
    echo "1) Native modules only (.mpy files)"
    echo "2) User C modules only (firmware)"
    echo "3) Both native and user C modules"
    echo "4) Clean all builds"
    echo "5) Exit"
}

# Parse command line arguments
if [ "$1" == "--native" ]; then
    CHOICE=1
elif [ "$1" == "--firmware" ]; then
    CHOICE=2
elif [ "$1" == "--both" ]; then
    CHOICE=3
elif [ "$1" == "--clean" ]; then
    CHOICE=4
else
    show_menu
    read -p "Enter choice (1-5): " CHOICE
fi

case $CHOICE in
    1)
        build_native_modules && test_native_modules
        ;;
    2)
        build_firmware && test_user_c_modules
        ;;
    3)
        echo -e "\n${YELLOW}Testing both modes...${NC}"
        build_native_modules && test_native_modules
        if [ $? -eq 0 ]; then
            build_firmware && test_user_c_modules
        fi
        ;;
    4)
        echo -e "\n${YELLOW}Cleaning all builds...${NC}"
        cd "$SCRIPT_DIR"
        rm -f *.mpy *.o
        cd "$MPY_DIR/ports/${PORT:-unix}"
        make clean
        echo -e "${GREEN}✓ Clean complete${NC}"
        ;;
    5)
        echo "Exiting..."
        exit 0
        ;;
    *)
        echo -e "${RED}Invalid choice${NC}"
        exit 1
        ;;
esac