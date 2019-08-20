FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update

# from https://github.com/pfalcon/esp-open-sdk
RUN apt-get install -y \
        make unrar-free autoconf automake libtool gcc g++ gperf \
        flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial \
        sed git unzip bash help2man wget bzip2 libtool-bin

# to install esp-idf requirements
RUN apt-get install -y python-pip

ARG GNU_ARM_TOOLCHAIN_TAR='https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2'
ARG ESP_TOOLCHAIN_TAR='https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz'
ARG ESP_IDF_GIT='https://github.com/espressif/esp-idf.git'
ARG ESP_OPEN_SDK_GIT='https://github.com/pfalcon/esp-open-sdk.git'

# esp-open-sdk won't compile with root user
ENV HOME /home/docker
RUN useradd -m docker -d $HOME
USER docker
WORKDIR $HOME

# setup esp-open-sdk (needed for esp8266)
RUN git clone --recursive $ESP_OPEN_SDK_GIT $HOME/esp-open-sdk
RUN cd $HOME/esp-open-sdk && make
ENV PATH $HOME/esp-open-sdk/xtensa-lx106-elf/bin:$PATH

# setup gnu arm toolchain
RUN mkdir $HOME/gcc-arm-none-eabi
WORKDIR $HOME/gcc-arm-none-eabi
RUN wget -qO- $GNU_ARM_TOOLCHAIN_TAR | tar -xjf -
ENV PATH $HOME/gcc-arm-none-eabi/bin:$PATH

# setup esp toolchain
RUN mkdir $HOME/esp
WORKDIR $HOME/esp
RUN wget -qO- $ESP_TOOLCHAIN_TAR | tar -xzf -
ENV PATH $HOME/esp/xtensa-esp32-elf/bin:$PATH

# setup esp-idf (needed for esp32 port)
RUN git clone $ESP_IDF_GIT $HOME/esp/esp-idf
WORKDIR $HOME/esp/esp-idf
COPY ports/esp32/ESPIDF_SUPHASH $HOME/ESPIDF_SUPHASH
RUN git checkout $(cat $HOME/ESPIDF_SUPHASH)
RUN rm $HOME/ESPIDF_SUPHASH
RUN git submodule update --init --recursive
ENV IDF_PATH $HOME/esp/esp-idf
RUN pip install -r $IDF_PATH/requirements.txt

WORKDIR $HOME
USER root

# required to build ports
RUN apt-get install -y sudo python3
