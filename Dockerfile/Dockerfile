# test comment 2
FROM ubuntu:16.04
ARG VERSION=master
ARG TARGET=esp32

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y make unrar-free autoconf automake libtool gcc g++ gperf \
    flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python3 python-serial python3-pip \
    sed git unzip bash help2man wget bzip2 curl zip jq
  && pip3 install pyparsing

RUN apt-get clean \
  && rm -rf /var/lib/apt/lists/* \
  && useradd --no-create-home micropython

RUN git clone https://github.com/espressif/esp-idf.git \
  && cd esp-idf && git checkout master \
  && git submodule update --init --recursive

RUN wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz \
&& tar -zxvf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz

RUN git clone https://github.com/ilyamordasov/micropython.git \
  && cd micropython && git checkout $VERSION && git pull && git submodule update --init \
  && chown -R micropython:micropython ../micropython

RUN apt-get update \
  && apt-get install -y mc libtool-bin

USER micropython

ENV PATH=/xtensa-esp32-elf/bin:$PATH

RUN cd /micropython/mpy-cross && make -C mpy-cross

RUN cd /micropython && git submodule init lib/berkeley-db-1.xx \
  && git submodule update

RUN cd /micropython/ports/$TARGET && make

RUN cd /micropython/ports/$TARGET/build \
    && zip -r -j $TARGET_v.1.10.1_$(date +%d%m%Y) /micropython/ports/esp32/build/*.bin *.bin *.elf

USER root
RUN mkdir /mnt/dev
ENV PATH=/micropython/mpy-cross:$PATH

RUN apt-get update && apt-get install -y usbutils picocom
