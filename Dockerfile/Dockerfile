# test comment 2
FROM ubuntu:16.04
ARG VERSION=master
ARG TARGET=esp32

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
#  && --assume-yes apt-utils \
  && apt-get install -y gcc git wget make libncurses-dev flex bison gperf python-dev python3 python3-pip python-serial python3-pyparsing \
  && pip3 install pyparsing

RUN apt-get clean \
  && rm -rf /var/lib/apt/lists/* \
  && useradd --no-create-home micropython
  && chown -R micropython:micropython /home

RUN git clone https://github.com/ilyamordasov/micropython.git \
  && cd micropython \
  && git submodule update --init

RUN apt-get update \
  && apt-get install -y mc libtool-bin

USER micropython

RUN cd /
    && mkdir ESP-IDF && cd ESP-IDF \
    && git clone https://github.com/espressif/esp-idf.git . \
    && git checkout 5c88c5996dbde6208e3bec05abc21ff6cd822d26 \
    && git submodule update --init

ENV ESPIDF=/ESP-IDF

RUN cd / \
  && wget https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz \
  && tar -zxvf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz \
  && export PATH=/xtensa-esp32-elf/bin:$PATH

RUN cd ilyamordasov/micropython \
  && git submodule update --init \
  && make -j4 -C mpy-cross \
  && git submodule init lib/berkeley-db-1.xx \
  && git submodule init lib/esp-idf \
  && git submodule update \
  make -j4 -C ports/$TARGET

# RUN cd /micropython/ports/$TARGET/build \
#   && GITHUB_RELEASE_ID=$(curl --data '{"tag_name": "v.1.9.3-beta.1","target_commitish": "master","name": "v.1.9.3-beta.1","body": "Release of version v.1.9.3-beta.1","draft": false,"prerelease": false}' https://api.github.com/repos/ilyamordasov/micropython/releases?access_token=$GITHUB_ACCESS_TOKEN) \
#   && zip -R assets '*.bin' '*.elf' \
#   && curl -H "Content-Type: application/zip" --data-binary @assets.zip "https://uploads.github.com/repos/ilyamordasov/micropython/releases/$GITHUB_RELEASE_ID/assets?name=assets.zip&access_token=$GITHUB_ACCESS_TOKEN"

# RUN cd /micropython/ports/$TARGET/build \
#     && zip -r -j $TARGET_v.1.9.4_$(date +%d%m%Y) /esp-open-sdk/ESP8266_NONOS_SDK-2.1.0-18-g61248df/bin/*.bin *.bin *.elf
