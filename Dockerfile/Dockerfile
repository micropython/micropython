# test comment 2
FROM ubuntu:16.04
ARG VERSION=master

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y make unrar-free autoconf automake libtool gcc g++ gperf \
    flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial python-pip\
    sed git unzip bash help2man wget bzip2 curl zip jq

RUN apt-get clean \
  && rm -rf /var/lib/apt/lists/* \
  && useradd --no-create-home micropython

RUN git clone --recursive https://github.com/pfalcon/esp-open-sdk.git \
  && git clone https://github.com/ilyamordasov/micropython.git \
  && cd micropython && git checkout $VERSION && git pull && git submodule update --init \
  && chown -R micropython:micropython ../esp-open-sdk ../micropython

RUN apt-get update \
  && apt-get install -y mc libtool-bin

RUN pip install adafruit-ampy

USER micropython

RUN cd esp-open-sdk && make STANDALONE=y

ENV PATH=/esp-open-sdk/xtensa-lx106-elf/bin:$PATH

RUN cd /micropython/mpy-cross && make

RUN cd /micropython/ports/esp8266 && make axtls && make

RUN cd /micropython/ports/esp8266/build \
  && GITHUB_RELEASE_ID=$(curl --data '{"tag_name": "v.1.9.3-beta.1","target_commitish": "master","name": "v.1.9.3-beta.1","body": "Release of version v.1.9.3-beta.1","draft": false,"prerelease": false}' https://api.github.com/repos/ilyamordasov/micropython/releases?access_token=$GITHUB_ACCESS_TOKEN) \
  && zip -R assets '*.bin' '*.elf' \
  && curl -H "Content-Type: application/zip" --data-binary @assets.zip "https://uploads.github.com/repos/ilyamordasov/micropython/releases/$GITHUB_RELEASE_ID/assets?name=assets.zip&access_token=$GITHUB_ACCESS_TOKEN"

USER root
RUN mkdir /mnt/dev
ENV PATH=/micropython/mpy-cross:$PATH

RUN apt-get update && apt-get install -y usbutils picocom
