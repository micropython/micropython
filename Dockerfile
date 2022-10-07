FROM minyiky/dev_environment:ubuntu-22.04 as prebuilder

RUN apt update && \
    apt install -y --no-install-recommends build-essential make libreadline-dev libffi-dev git pkg-config libsdl2-2.0-0 libsdl2-dev python3 parallel

RUN rm -rf /var/lib/apt/lists/* 

# COPY . /micropython

# RUN cd micropython && \
#     git submodule update --init --recursive lib/lv_bindings

# RUN make -C micropython/mpy-cross

# RUN make -C micropython/ports/unix VARIANT=dev submodules deplibs && \
#     make -C micropython/ports/unix VARIANT=dev && \
#     make -C micropython/ports/unix VARIANT=dev install

# FROM ubuntu as final

# COPY --from=prebuilder /usr/local/bin/micropython /usr/local/bin/micropython

# ENV DISPLAY=host.docker.internal:0.0

# CMD ["/usr/local/bin/micropython"]
