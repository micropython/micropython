FROM ubuntu

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y \
        libffi6 pkg-config libffi-dev pkg-config build-essential \
        gcc git wget make libncurses-dev flex bison gperf autoconf \
        python3 python3-dev python3-pip python python-dev python-pip \
        make unrar-free autoconf automake libtool gcc g++ gperf \
        flex bison texinfo gawk ncurses-dev libexpat-dev \
        sed git unzip bash help2man wget bzip2 \
        libtool-bin gcc-arm-none-eabi qemu-system-arm

RUN useradd -m docker
USER docker
ENV HOME /home/docker
WORKDIR $HOME

# setup esp toolchain
RUN mkdir -p $HOME/esp
WORKDIR $HOME/esp
RUN wget -qO- https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz | tar -xzf -
ENV PATH "$HOME/esp/xtensa-esp32-elf/bin:$PATH"

# setup esp-idf (needed for esp32 port)
WORKDIR $HOME/esp
RUN git clone https://github.com/espressif/esp-idf.git

WORKDIR $HOME/esp/esp-idf
RUN git checkout 6b3da6b1882f3b72e904cc90be67e9c4e3f369a9
RUN git submodule update --init --recursive
ENV IDF_PATH $HOME/esp/esp-idf
RUN pip install -r $IDF_PATH/requirements.txt

# setup esp-open-sdk (needed for esp8266)
WORKDIR $HOME
RUN git clone --recursive https://github.com/pfalcon/esp-open-sdk.git

RUN cd esp-open-sdk && make
ENV PATH "$HOME/esp-open-sdk/xtensa-lx106-elf/bin:$PATH"

USER root
