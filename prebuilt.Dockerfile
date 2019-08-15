FROM micropython

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
