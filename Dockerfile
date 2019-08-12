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
RUN wget -q https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
RUN tar -xzf xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz
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

# setup micropython
WORKDIR $HOME
RUN git clone https://github.com/micropython/micropython.git

WORKDIR $HOME/micropython
RUN git submodule update --init
RUN make -C mpy-cross
RUN pip3 install pyserial pyparsing

# build every port in advance, for build cache
RUN cd ports/bare-arm && make
# RUN cd ports/cc3200 && make
RUN cd ports/esp32 && make
RUN cd ports/esp8266 && make
# RUN cd ports/javascript && make
# RUN cd ports/minimal && make
RUN cd ports/nrf && make
# RUN cd ports/pic16bit && make
RUN cd ports/qemu-arm && make
RUN cd ports/samd && make
RUN cd ports/stm32 && make
RUN cd ports/teensy && make
RUN cd ports/unix && make
# RUN cd ports/windows && make
# RUN cd ports/zephyr && make

USER root
RUN ln -s $HOME/micropython/ports/unix/micropython /usr/bin/micropython
