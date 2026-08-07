# the name of the target operating system
set(CMAKE_SYSTEM_NAME RISCV32)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   /home/cpruefer/.espressif/tools/riscv32-esp-elf/esp-15.2.0_20251204/riscv32-esp-elf/bin/riscv32-esp-elf-gcc)
set(CMAKE_CXX_COMPILER /home/cpruefer/.espressif/tools/riscv32-esp-elf/esp-15.2.0_20251204/riscv32-esp-elf/bin/riscv32-esp-elf-g++)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH ~/.espressif/tools/riscv32-esp-elf/esp-15.2.0_20251204/riscv32-esp-elf)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
