FROM ubuntu:16.04
ARG VERSION=esp32-bluetooth
ARG TARGET=esp32
ARG IDF_HASH=5c88c5996dbde6208e3bec05abc21ff6cd822d26
ARG TAG=1.10

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y gcc git wget zip make libncurses-dev flex bison gperf python-dev python3 python3-pip python-serial python3-pyparsing mc nano libtool-bin \
  && pip3 install pyparsing

WORKDIR /micropython
RUN git clone -b $VERSION https://github.com/ilyamordasov/micropython.git . \
    && git submodule update --init --recursive

WORKDIR /ESP-IDF
RUN git clone https://github.com/espressif/esp-idf.git . \
    && git checkout $IDF_HASH \
    && git submodule update --init
ENV IDF_PATH=/ESP-IDF

WORKDIR /xtensa
RUN wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz \
  && tar -zxvf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
ENV PATH=/xtensa/xtensa-esp32-elf/bin:$PATH

WORKDIR /micropython
RUN git submodule update --init \
  && git submodule init lib/berkeley-db-1.xx

WORKDIR /micropython
RUN git submodule update

RUN make ${MAKEOPTS} -C mpy-cross
RUN make ${MAKEOPTS} -C ports/$TARGET

WORKDIR /micropython/ports/$TARGET/build
RUN zip -r -j $TARGET_v.$TAG_$(date +%d%m%Y).zip *.bin *.elf

WORKDIR /xtensa
RUN rm -rf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

WORKDIR /micropython
