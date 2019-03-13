FROM ubuntu:16.04
ARG VERSION=master
ARG TARGET=esp32
ARG TAG=1.10

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y gcc git wget make libncurses-dev flex bison gperf python-dev python3 python3-pip python-serial python3-pyparsing mc libtool-bin \
  && pip3 install pyparsing

#RUN apt-get clean \
#  && rm -rf /var/lib/apt/lists/*
  
# RUN useradd --no-create-home micropython
# USER micropython
  
WORKDIR /micropython
RUN git clone https://github.com/ilyamordasov/micropython.git . \
  && git submodule update --init --recursive
  
WORKDIR /tmp/ESP-IDF
RUN git clone https://github.com/espressif/esp-idf.git . \
    && git checkout 5c88c5996dbde6208e3bec05abc21ff6cd822d26 \
    && git submodule update --init
ENV ESPIDF=/micropython/tmp/ESP-IDF

WORKDIR /tmp
RUN wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz \
  && tar -zxvf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
ENV PATH=/tmp/xtensa-esp32-elf/bin:$PATH

WORKDIR /micropython
RUN git submodule update --init \
  && git submodule init lib/berkeley-db-1.xx

WORKDIR /micropython/lib/ESP-IDF
RUN git checkout $IDF_HASH
ENV ESPIDF=/micropython/lib/ESP-IDF

WORKDIR /micropython
RUN git submodule update

RUN make ${MAKEOPTS} -C mpy-cross
RUN make ${MAKEOPTS} -C ports/$TARGET
  
WORKDIR /micropython/ports/$TARGET/build
RUN zip -r -j $TARGET_v.$TAG_$(date +%d%m%Y).zip *.bin *.elf
