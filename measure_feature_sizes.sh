#!/bin/bash
# Measure size impact of each metaclass and enum feature

set -e

BOARD=PYBV10
PORT_DIR=ports/stm32
RESULTS_FILE="FEATURE_SIZE_REPORT.md"

echo "# Feature Size Measurement Report" > $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "**Board:** $BOARD" >> $RESULTS_FILE
echo "**Date:** $(date '+%Y-%m-%d %H:%M:%S')" >> $RESULTS_FILE
echo "**Default ROM Level:** EXTRA_FEATURES" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "---" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Function to build and measure
build_and_measure() {
    local desc="$1"
    local cflags="$2"
    local manifest="$3"

    echo "Building: $desc"
    echo "  CFLAGS: $cflags"

    cd $PORT_DIR
    make BOARD=$BOARD clean > /dev/null 2>&1

    if [ -n "$manifest" ]; then
        make -j BOARD=$BOARD FROZEN_MANIFEST="$manifest" CFLAGS_EXTRA="$cflags" 2>&1 | tail -5
    else
        make -j BOARD=$BOARD CFLAGS_EXTRA="$cflags" 2>&1 | tail -5
    fi

    local size=$(arm-none-eabi-size build-$BOARD/firmware.elf | awk 'NR==2 {print $1}')
    echo "  Size: $size bytes"
    echo ""
    cd - > /dev/null

    echo "$size"
}

# Baseline: All metaclass features disabled, no enum
echo "## 1. Baseline Measurement" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "Building baseline (all metaclass features disabled, no enum)..." >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

BASELINE=$(build_and_measure "Baseline (all disabled)" \
    "-DMICROPY_PY_METACLASS_INIT=0 -DMICROPY_PY_METACLASS_OPS=0 -DMICROPY_PY_METACLASS_PROPERTIES=0 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "")

echo "| Configuration | Text Size (bytes) | Change from Baseline | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|---------------------|----------|" >> $RESULTS_FILE
echo "| Baseline (all disabled) | $BASELINE | 0 | 0.00% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Feature 1: MICROPY_PY_METACLASS_INIT only
echo "## 2. Individual Metaclass Features" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "### Feature 1: MICROPY_PY_METACLASS_INIT" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "Enables metaclass \`__init__\` invocation after class creation." >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

SIZE_INIT=$(build_and_measure "METACLASS_INIT only" \
    "-DMICROPY_PY_METACLASS_INIT=1 -DMICROPY_PY_METACLASS_OPS=0 -DMICROPY_PY_METACLASS_PROPERTIES=0 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "")

DELTA_INIT=$((SIZE_INIT - BASELINE))
PCT_INIT=$(echo "scale=3; ($DELTA_INIT * 100.0) / $BASELINE" | bc)

echo "| Configuration | Text Size (bytes) | Change from Baseline | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|---------------------|----------|" >> $RESULTS_FILE
echo "| + METACLASS_INIT | $SIZE_INIT | +$DELTA_INIT | +$PCT_INIT% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Feature 2: MICROPY_PY_METACLASS_OPS only
echo "### Feature 2: MICROPY_PY_METACLASS_OPS" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "Enables operator support on type objects (\`__len__\`, \`__contains__\`, \`__hash__\`)." >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

SIZE_OPS=$(build_and_measure "METACLASS_OPS only" \
    "-DMICROPY_PY_METACLASS_INIT=0 -DMICROPY_PY_METACLASS_OPS=1 -DMICROPY_PY_METACLASS_PROPERTIES=0 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "")

DELTA_OPS=$((SIZE_OPS - BASELINE))
PCT_OPS=$(echo "scale=3; ($DELTA_OPS * 100.0) / $BASELINE" | bc)

echo "| Configuration | Text Size (bytes) | Change from Baseline | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|---------------------|----------|" >> $RESULTS_FILE
echo "| + METACLASS_OPS | $SIZE_OPS | +$DELTA_OPS | +$PCT_OPS% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Feature 3: MICROPY_PY_METACLASS_PROPERTIES only
echo "### Feature 3: MICROPY_PY_METACLASS_PROPERTIES" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "Enables property and method lookup on classes from metaclass." >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

SIZE_PROPS=$(build_and_measure "METACLASS_PROPERTIES only" \
    "-DMICROPY_PY_METACLASS_INIT=0 -DMICROPY_PY_METACLASS_OPS=0 -DMICROPY_PY_METACLASS_PROPERTIES=1 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "")

DELTA_PROPS=$((SIZE_PROPS - BASELINE))
PCT_PROPS=$(echo "scale=3; ($DELTA_PROPS * 100.0) / $BASELINE" | bc)

echo "| Configuration | Text Size (bytes) | Change from Baseline | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|---------------------|----------|" >> $RESULTS_FILE
echo "| + METACLASS_PROPERTIES | $SIZE_PROPS | +$DELTA_PROPS | +$PCT_PROPS% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Feature 4: MICROPY_PY_METACLASS_PREPARE only
echo "### Feature 4: MICROPY_PY_METACLASS_PREPARE" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "Enables \`__prepare__\` metaclass method (PEP 3115) for custom class namespace." >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

SIZE_PREPARE=$(build_and_measure "METACLASS_PREPARE only" \
    "-DMICROPY_PY_METACLASS_INIT=0 -DMICROPY_PY_METACLASS_OPS=0 -DMICROPY_PY_METACLASS_PROPERTIES=0 -DMICROPY_PY_METACLASS_PREPARE=1" \
    "")

DELTA_PREPARE=$((SIZE_PREPARE - BASELINE))
PCT_PREPARE=$(echo "scale=3; ($DELTA_PREPARE * 100.0) / $BASELINE" | bc)

echo "| Configuration | Text Size (bytes) | Change from Baseline | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|---------------------|----------|" >> $RESULTS_FILE
echo "| + METACLASS_PREPARE | $SIZE_PREPARE | +$DELTA_PREPARE | +$PCT_PREPARE% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Cumulative builds
echo "## 3. Cumulative Feature Builds" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "Building with features enabled cumulatively:" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# CORE level (INIT only)
SIZE_CORE=$(build_and_measure "CORE level (INIT)" \
    "-DMICROPY_PY_METACLASS_INIT=1 -DMICROPY_PY_METACLASS_OPS=0 -DMICROPY_PY_METACLASS_PROPERTIES=0 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "")

DELTA_CORE=$((SIZE_CORE - BASELINE))
PCT_CORE=$(echo "scale=3; ($DELTA_CORE * 100.0) / $BASELINE" | bc)

# EXTRA level (INIT + OPS + PROPERTIES)
SIZE_EXTRA=$(build_and_measure "EXTRA level (INIT+OPS+PROPS)" \
    "-DMICROPY_PY_METACLASS_INIT=1 -DMICROPY_PY_METACLASS_OPS=1 -DMICROPY_PY_METACLASS_PROPERTIES=1 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "")

DELTA_EXTRA=$((SIZE_EXTRA - BASELINE))
PCT_EXTRA=$(echo "scale=3; ($DELTA_EXTRA * 100.0) / $BASELINE" | bc)

# FULL level (all enabled)
SIZE_FULL=$(build_and_measure "FULL level (all enabled)" \
    "-DMICROPY_PY_METACLASS_INIT=1 -DMICROPY_PY_METACLASS_OPS=1 -DMICROPY_PY_METACLASS_PROPERTIES=1 -DMICROPY_PY_METACLASS_PREPARE=1" \
    "")

DELTA_FULL=$((SIZE_FULL - BASELINE))
PCT_FULL=$(echo "scale=3; ($DELTA_FULL * 100.0) / $BASELINE" | bc)

echo "| Configuration | Text Size (bytes) | Change from Baseline | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|---------------------|----------|" >> $RESULTS_FILE
echo "| Baseline | $BASELINE | 0 | 0.00% |" >> $RESULTS_FILE
echo "| CORE (INIT) | $SIZE_CORE | +$DELTA_CORE | +$PCT_CORE% |" >> $RESULTS_FILE
echo "| EXTRA (INIT+OPS+PROPS) | $SIZE_EXTRA | +$DELTA_EXTRA | +$PCT_EXTRA% |" >> $RESULTS_FILE
echo "| FULL (all metaclass) | $SIZE_FULL | +$DELTA_FULL | +$PCT_FULL% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Enum module impact
echo "## 4. Enum Module Impact" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "### With Default Features (EXTRA level)" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

SIZE_ENUM_DEFAULT=$(build_and_measure "EXTRA + enum.py frozen" \
    "-DMICROPY_PY_METACLASS_INIT=1 -DMICROPY_PY_METACLASS_OPS=1 -DMICROPY_PY_METACLASS_PROPERTIES=1 -DMICROPY_PY_METACLASS_PREPARE=0" \
    "boards/manifest_enum.py")

DELTA_ENUM_DEFAULT=$((SIZE_ENUM_DEFAULT - SIZE_EXTRA))
PCT_ENUM_DEFAULT=$(echo "scale=3; ($DELTA_ENUM_DEFAULT * 100.0) / $SIZE_EXTRA" | bc)

echo "| Configuration | Text Size (bytes) | Change from EXTRA | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|-------------------|----------|" >> $RESULTS_FILE
echo "| EXTRA (no enum) | $SIZE_EXTRA | 0 | 0.00% |" >> $RESULTS_FILE
echo "| EXTRA + enum.py frozen | $SIZE_ENUM_DEFAULT | +$DELTA_ENUM_DEFAULT | +$PCT_ENUM_DEFAULT% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# With PREPARE for auto() support
echo "### With PREPARE Enabled (for auto() support)" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

SIZE_ENUM_FULL=$(build_and_measure "FULL + enum.py frozen" \
    "-DMICROPY_PY_METACLASS_INIT=1 -DMICROPY_PY_METACLASS_OPS=1 -DMICROPY_PY_METACLASS_PROPERTIES=1 -DMICROPY_PY_METACLASS_PREPARE=1" \
    "boards/manifest_enum.py")

DELTA_ENUM_FULL=$((SIZE_ENUM_FULL - SIZE_FULL))
PCT_ENUM_FULL=$(echo "scale=3; ($DELTA_ENUM_FULL * 100.0) / $SIZE_FULL" | bc)

echo "| Configuration | Text Size (bytes) | Change from FULL | % Change |" >> $RESULTS_FILE
echo "|--------------|------------------|------------------|----------|" >> $RESULTS_FILE
echo "| FULL (no enum) | $SIZE_FULL | 0 | 0.00% |" >> $RESULTS_FILE
echo "| FULL + enum.py frozen | $SIZE_ENUM_FULL | +$DELTA_ENUM_FULL | +$PCT_ENUM_FULL% |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

# Summary
echo "## 5. Summary" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "### Individual Feature Costs (C code only)" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "| Feature | Size (bytes) | Description |" >> $RESULTS_FILE
echo "|---------|-------------|-------------|" >> $RESULTS_FILE
echo "| METACLASS_INIT | $DELTA_INIT | Metaclass \`__init__\` invocation |" >> $RESULTS_FILE
echo "| METACLASS_OPS | $DELTA_OPS | Operator support on types |" >> $RESULTS_FILE
echo "| METACLASS_PROPERTIES | $DELTA_PROPS | Property/method lookup |" >> $RESULTS_FILE
echo "| METACLASS_PREPARE | $DELTA_PREPARE | \`__prepare__\` method support |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

echo "### ROM Level Configurations" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "| ROM Level | Features Enabled | Size (bytes) | Overhead |" >> $RESULTS_FILE
echo "|-----------|-----------------|--------------|----------|" >> $RESULTS_FILE
echo "| MINIMAL | None | $BASELINE | baseline |" >> $RESULTS_FILE
echo "| CORE | INIT | $SIZE_CORE | +$DELTA_CORE bytes |" >> $RESULTS_FILE
echo "| EXTRA | INIT, OPS, PROPERTIES | $SIZE_EXTRA | +$DELTA_EXTRA bytes |" >> $RESULTS_FILE
echo "| FULL | All metaclass features | $SIZE_FULL | +$DELTA_FULL bytes |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

echo "### Enum Module Costs" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "| Configuration | Enum Module Size | Total Overhead |" >> $RESULTS_FILE
echo "|--------------|------------------|----------------|" >> $RESULTS_FILE
echo "| EXTRA + enum.py | $DELTA_ENUM_DEFAULT bytes | +$((DELTA_EXTRA + DELTA_ENUM_DEFAULT)) bytes |" >> $RESULTS_FILE
echo "| FULL + enum.py | $DELTA_ENUM_FULL bytes | +$((DELTA_FULL + DELTA_ENUM_FULL)) bytes |" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

echo "### Total Implementation Cost" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
TOTAL_C=$((DELTA_INIT + DELTA_OPS + DELTA_PROPS + DELTA_PREPARE))
echo "- **C code (all metaclass features):** $TOTAL_C bytes" >> $RESULTS_FILE
echo "- **Python enum.py (frozen):** $DELTA_ENUM_FULL bytes" >> $RESULTS_FILE
echo "- **Total:** $((TOTAL_C + DELTA_ENUM_FULL)) bytes" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE

echo "---" >> $RESULTS_FILE
echo "" >> $RESULTS_FILE
echo "*Report generated: $(date)*" >> $RESULTS_FILE

echo ""
echo "✅ Measurement complete! Results saved to $RESULTS_FILE"
cat $RESULTS_FILE
