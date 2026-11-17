/* MicroPython linker script for RP2040 and RP2350.

   This uses the Pico SDK's include-based ("overridable") default linker script
   (memmap_default.incl) and only overrides the parts MicroPython needs to
   change. The per-part overrides live in:
     memmap_rp2/     - common to both chips
     memmap_rp2040/  - RP2040 only
     memmap_rp2350/  - RP2350 only
   These directories are added to the linker script search path (ahead of the
   SDK's) by pico_add_linker_script_override_path() in CMakeLists.txt. */

/* Delete scratch X and expand scratch Y, so all of the C stack lives in a
   single 8K scratch region (matches the previous hand-written scripts). */
SCRATCH_X_LENGTH = 0;
SCRATCH_Y_ORIGIN = SCRATCH_X_ORIGIN_DEFAULT;
SCRATCH_Y_LENGTH = SCRATCH_Y_LENGTH_DEFAULT*2;

INCLUDE "memmap_default.incl"
