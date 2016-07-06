This driver uses the generic GADC driver to provide a GAUDIO recording device.

It supports whatever high speed device channels that your GADC driver and board supports.

For stereo, the samples are interleaved. Remember to allocate enough space for two samples per
sample period.
