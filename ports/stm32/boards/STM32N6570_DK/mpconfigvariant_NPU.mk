# The npu module, which needs ST Edge AI Core (see docs/library/npu.rst).  AXISRAM3-6
# and the NPU cache RAM are reserved for the NPU instead of the heap, and the SPI flash
# has a ROMFS partition for models.
MICROPY_PY_NPU = 1
