:mod:`npu` --- neural network accelerator
=========================================

.. module:: npu
   :synopsis: neural network accelerator

This module runs neural network models on a neural processing unit (NPU).  Models
are compiled for the NPU on a PC, and the compiled model is loaded at runtime, so
models can be changed without rebuilding the firmware.

The most common models work on images, such as object detectors and classifiers,
but there are models for other kinds of data too, such as recognising spoken
keywords from audio, or detecting gestures from accelerometer data.

Availability: STM32N6, with the firmware built with ``MICROPY_PY_NPU=1`` (see
`Building the firmware`_).  The module uses the STM32N6's Neural-ART NPU.

Example::

    import npu

    model = npu.Model("model.bin")
    print(model.inputs, model.outputs)

    # Run the model on the input data, such as an image of the size given by
    # model.inputs, and print each output.
    image = bytearray(224 * 224 * 3)
    outputs = model.run(image)

    for output in outputs:
        print(list(output))

The module only moves data in and out of the model and runs it.  Preparing the input
(for example scaling an image to the model's input size) and interpreting the output
(for example turning an object detector's output into boxes) are up to the
application, and depend on the model.

Functions
---------

.. function:: model_info(source)

   Read a compiled model's details without loading it.  ``source`` is the name of a
   model file, or an object with the buffer protocol holding a model file, as for
   `Model`.  Only the start of the file and a small part of its data are read, so this
   is quick and needs little memory, and works for models that `Model` can't load.

   Returns a tuple ``(version, file_size, acts_size, ext_ram_size, heap_size)``, of the
   values that ``compile_model.py`` prints:

   - ``version``: the runtime version the model was compiled for, as a tuple like
     `npu.version`;
   - ``file_size``: the size of the model file, in bytes;
   - ``acts_size``: the size of the model's activations, in bytes;
   - ``ext_ram_size``: the part of the activations that doesn't fit in AXISRAM3-6 and
     goes in PSRAM, 0 if they all fit;
   - ``heap_size``: the heap memory the model needs for its code data.

   Raises ``ValueError`` if ``source`` isn't a relocatable NPU model.  It doesn't check
   that the model was compiled for this module's memory layout, which `Model` does.

Constants
---------

.. data:: version

   The version of ST's NPU runtime in the firmware, as a tuple
   ``(major, minor, micro)``, eg ``(1, 1, 3)``.  Models must be compiled for a runtime
   with the same version (see `Runtime versions`_).

Classes
-------

.. class:: Model(source)

   Load a compiled model.  ``source`` is either:

   - the name of the model file; or
   - an object with the buffer protocol holding the model file.

   If the model file is in memory-mapped flash and 32-byte aligned, such as a file in
   ROMFS, the model is used in place.  Otherwise it is copied into the PSRAM reserved
   for the NPU, so on boards without PSRAM, models must be in ROMFS.  The model also
   needs heap memory for its code data, and PSRAM for any activations that don't fit
   in AXISRAM3-6, as printed by ``compile_model.py``.

   Several models can be loaded at once.  They share AXISRAM3-6, so a model's input
   and output buffers only keep their data until another model runs.

   Raises ``ValueError`` if ``source`` isn't a model compiled for this module, if it
   was compiled for a different runtime version (see `Runtime versions`_), or if it
   needs PSRAM that the board doesn't have, and ``MemoryError`` if there isn't enough
   memory.

   .. attribute:: Model.inputs
                  Model.outputs

      A tuple describing each of the model's inputs or outputs, each a tuple of
      ``(shape, typecode, scale, zero_point)``:

      - ``shape`` is a tuple of the dimensions of the data, eg ``(1, 224, 224, 3)``
        for an image of 224x224 pixels with 3 channels.
      - ``typecode`` is the type of the data, as for `array.array`: ``"B"``,
        ``"b"``, ``"H"``, ``"h"``, ``"I"``, ``"i"`` or ``"f"``.
      - ``scale`` and ``zero_point`` give the real value of quantised data, as
        ``(value - zero_point) * scale``.  For unquantised data they are 1.0 and 0.

      The inputs and outputs are in the order ST Edge AI Core gives them, which can
      differ from the order of the original model.  For example, a ``.tflite`` model
      whose inputs TensorFlow Lite's interpreter lists as ``(state, x)`` can have them
      in the order ``(x, state)`` here.  Use the shapes and types in these tuples to
      tell the inputs and outputs apart, rather than relying on their order in the
      original model.

   .. method:: Model.run([inputs])

      Run the model, wait for it to finish, and return a tuple with a read-only
      `memoryview` of each output buffer, with the output's typecode.  Pending events,
      such as scheduled callbacks, are handled while it runs.

      ``inputs`` is the input data, which is copied into the model's input buffers
      before the run.  It is either an object with the buffer protocol (for a model
      with one input), or a tuple or list of them (one for each input in the order of
      `Model.inputs`).  Each must have the same size in bytes as the input buffer, but
      can have any type.  If ``inputs`` is omitted or ``None``, the model runs on the
      data already in its input buffers (see `input()`).

      The memoryviews are made when the model is loaded, so each run returns the same
      ones, and running the model doesn't allocate memory.  They hold the outputs
      until this model or another one runs, so copy any output that is needed for
      longer, for example with ``bytes(output)``.

      Raises ``ValueError`` if the number or the sizes of the inputs don't match the
      model, and ``OSError(EBUSY)`` if a model is already running, for example if
      called from a scheduled callback while another model runs.

   .. method:: Model.input([index])

      Return a `memoryview` of the input buffer with the given index (default 0),
      with the input's typecode.  Data that is read or decoded straight into this,
      for example with ``f.readinto(model.input())``, needs no buffer of its own, and
      `run()` called with no inputs then uses it without copying it.

      The input buffer is part of the model's activations, which the model reuses as
      it runs, and which other models share, so the input must be written in full
      before each run.  Each call returns the same memoryview, made when the model is
      loaded.

   .. method:: Model.output([index])

      Return a read-only `memoryview` of the output buffer with the given index
      (default 0), with the output's typecode.  This is the same memoryview as in the
      tuple returned by `run()`.

   .. method:: Model.deinit()

      Free the model's memory.  This also happens when the model is garbage
      collected.  Any further use of the model raises ``OSError(EPERM)``.

      The memoryviews returned by `run()`, `input()` and `output()`, and any made
      from them, point at memory that other models use once this model is freed, so
      they must not be used after the model is deinitialised or garbage collected.
      Keep a reference to the model while any of its memoryviews are in use.

Models in ROMFS
---------------

A model file in ROMFS is used in place, without copying it into RAM, if its data is
32-byte aligned in the memory-mapped flash.  On boards without PSRAM, this is the only
way to load a model.  ``mpremote romfs build`` doesn't align files, so build the ROMFS
image with ``ports/stm32/npu/mkromfs.py`` instead, then deploy it with ``mpremote``::

    $ python3 mkromfs.py models -o models.romfs
    $ mpremote romfs deploy models.romfs

``mkromfs.py`` puts every file in the ``models`` directory in the image, with each
file's data 32-byte aligned.  It also compiles any ``.tflite`` or ``.onnx`` file in the
directory with ``compile_model.py`` (see `Compiling models`_), storing it with a
``.bin`` extension instead.  Options for compiling a model go in a file next to it,
named after it with ``.args`` added, for example ``model.onnx.args`` containing
``--input-data-type uint8 --inputs-ch-position chlast``.  The device mounts the
ROMFS at ``/rom``, so a model ``model.bin`` is loaded with ``npu.Model("/rom/model.bin")``.

Runtime versions
----------------

A compiled model contains code that calls ST's NPU runtime, so it only works with the
version of the runtime it was compiled for.  The runtime's version is separate from
ST Edge AI Core's: for example, ST Edge AI Core 4.0.1 includes runtime 1.1.3, as given
by ``Middlewares/ST/AI/Npu/ll_aton/ll_aton_version.h`` in its install.  The runtime's
version changes less often than ST Edge AI Core's, and ST Edge AI Core 3.0, 4.0 and
4.0.1 all include runtime 1.1.3.

A model loads if the first three numbers of the runtime version it was compiled for
match those of the firmware's runtime, `npu.version`.  Otherwise `Model` raises
``ValueError``.  To check a model file without loading it, use `npu.model_info()`::

    >>> import npu
    >>> npu.version
    (1, 1, 3)
    >>> npu.model_info("model.bin")
    ((1, 1, 3), 1159072, 1152000, 0, 221504)

Building the firmware
---------------------

ST's NPU runtime isn't part of MicroPython.  It is built from an install of ST Edge AI
Core given by ``STEDGEAI_DIR`` (by default ``~/STEdgeAI/4.0``), when the firmware is
built with ``MICROPY_PY_NPU=1``::

    $ make BOARD=STM32N6570_DK MICROPY_PY_NPU=1 STEDGEAI_DIR=/path/to/STEdgeAI/4.0

With ``MICROPY_PY_NPU=1``, AXISRAM3-6 and the NPU cache's RAM are reserved for the
NPU instead of being part of the heap, which leaves about 2MB of internal RAM for the
heap.  On boards with PSRAM, the top 16MB of the PSRAM is also reserved for model
files and overflowing activations; a board can change this with
``MICROPY_HW_NPU_PSRAM_SIZE``.

The runtime is under ST's licence (SLA0104), which allows it to be redistributed but
only used on ST devices.

Compiling models
----------------

Models in TensorFlow Lite (``.tflite``) or ONNX format, such as those in the
`ST model zoo <https://github.com/STMicroelectronics/stm32ai-modelzoo>`_, are compiled
with `ST Edge AI Core <https://www.st.com/en/development-tools/stedgeai-core.html>`_
into a *relocatable* model: a single file holding the model's code, its NPU command
streams and its weights.  The firmware contains ST's NPU runtime, and a model only
loads if it was compiled for the same version of the runtime (see
`Runtime versions`_).

``ports/stm32/npu/compile_model.py`` compiles a model with the settings that the
``npu`` module needs::

    $ python3 compile_model.py model.tflite model.bin

On Windows, run it with ``python`` instead of ``python3``.  It needs ST Edge AI Core,
installed at ``STEDGEAI_DIR`` (by default ``~/STEdgeAI/4.0``, or ``C:/ST/STEdgeAI/4.0``
on Windows), and ST Edge AI Core needs ``make`` and the Arm GNU toolchain
(``arm-none-eabi-gcc``) on the ``PATH`` to build relocatable models.

The script prints the runtime version the model is compiled for and the memory it
needs:

- ``rt_version_desc``: the runtime version, eg ``atonn-v1.1.3-275-g6770925d``, whose
  first three numbers must match `npu.version`;
- ``binary file size``: the size of the model file, which is loaded into PSRAM unless
  it's used in place from ROMFS (see `Model`);
- ``acts size``: the RAM for the model's activations, the intermediate results of the
  model, which include its input and output buffers;
- ``ext_ram_sz``: the part of the activations that doesn't fit in the NPU's RAM and
  goes in PSRAM, 0 if they all fit;
- ``XIP size``: the heap memory for the model's code data.

Options after the output file are passed to ``stedgeai generate``.  The most useful
ones set the data type and layout of the model's input and output buffers.  By default
these keep the types of the original model, which vary from model to model.  For
example, of the object detectors in the ST model zoo:

- ``st_yoloxn`` (``.tflite``) takes ``uint8`` pixel values with the colour channels
  last (height, width, channel), and gives ``float32`` outputs, converted from
  integers by the model's last layers;
- ``st_yolodv2milli`` (ONNX) takes ``int8`` values, each a pixel value minus 128,
  with the colour channels first (channel, height, width), and gives ``int8`` outputs.

These options change that:

- ``--input-data-type uint8`` makes the model take ``uint8`` pixel values, as an
  image in memory holds them.  The conversion is merged into the model's first layer,
  so it costs nothing.
- ``--inputs-ch-position chlast`` puts the colour channels last, so each pixel's red,
  green and blue values are together, as in an image in memory.
- ``--output-data-type`` sets the type of the outputs, ``int8``, ``uint8`` or
  ``float32``.  ``float32`` outputs are the easiest to use, but the CPU converts them
  after the NPU finishes, and in Python reading each value allocates memory.  For
  ``int8`` and ``uint8`` outputs, `Model.outputs` gives the scale and zero point that
  convert them to real values, which only the values of interest need.

So for an image model, use ``--input-data-type uint8`` and, for an ONNX model,
``--inputs-ch-position chlast``.  Compiling an ONNX image model then looks like::

    $ python3 compile_model.py model.onnx model.bin --input-data-type uint8 --inputs-ch-position chlast

The script compiles with ``ports/stm32/npu/stm32n6.mpool``, which describes the
memory the ``npu`` module gives a model.  This is the same layout as ST's examples
and OpenMV use:

- The activations go in AXISRAM3-6, the 1.75MB of RAM next to the NPU, at fixed
  addresses.  All models share this RAM.
- Activations that don't fit in AXISRAM3-6 overflow into PSRAM.  The NPU reads and
  writes them through its cache, but PSRAM is much slower than AXISRAM3-6, so a
  model that overflows runs several times slower.
- The weights stay in the model file, which the NPU reads through its cache.  The NPU
  can only cache external memory, so the model file must be in ROMFS or PSRAM.

Models compiled with other memory pools, such as stedgeai's defaults, can't be loaded.
