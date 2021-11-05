FROM gitpod/workspace-full-vnc

# https://community.gitpod.io/t/long-startup-times/3299/9
ENV GITPOD_TRIGGER_REBUILD=1

# Install dependencies
RUN sudo apt-get update && \
    sudo apt-get install -y libgtk-3-dev libsdl2-dev x11-xserver-utils parallel libfreetype-dev librlottie-dev

