# syntax=docker/dockerfile:1
#
# Canonical Linux toolchain for the MicroPython UEFI port.
#
#   Stage `ovmf-builder` : builds NOOPT firmware for BOTH targets
#                          - OVMF  (X64)     via OvmfPkg/OvmfPkgX64.dsc
#                          - AAVMF (AArch64) via ArmVirtPkg/ArmVirtQemu.dsc
#                          Rebuilt rarely (only on EDK2_TAG / debug-flag change).
#   Stage `dev`          : day-to-day build/test/debug environment
#                          (clang+lld, qemu x86_64 & aarch64, gdb, python).
#
# Host-arch-agnostic: the firmware builder runs natively on the build host (arm64 or
# amd64) and cross-builds the *other* arch. EDK2 GCC5 uses DIFFERENT prefix variables per
# target arch: the X64 compiler is ENV(GCC5_BIN)gcc, while the AArch64 compiler is
# ENV(GCC5_AARCH64_PREFIX)gcc. So each build sets *its* arch's variable: empty for the
# native arch, the cross triplet for the other. No emulation needed on any host.
ARG EDK2_TAG=edk2-stable202502

# ---- Stage 1: firmware (OVMF X64 + AAVMF AArch64), native+cross per host arch ----
FROM ubuntu:24.04 AS ovmf-builder
ARG EDK2_TAG
ARG TARGETARCH                 # provided by BuildKit: "arm64" or "amd64"
ENV TARGETARCH=${TARGETARCH}
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential uuid-dev iasl nasm git python3 python3-venv python-is-python3 \
      ca-certificates \
 && case "$TARGETARCH" in \
      arm64) apt-get install -y --no-install-recommends gcc-x86-64-linux-gnu ;; \
      amd64) apt-get install -y --no-install-recommends gcc-aarch64-linux-gnu ;; \
      *) echo "unsupported build arch: $TARGETARCH" >&2; exit 1 ;; \
    esac \
 && rm -rf /var/lib/apt/lists/*
# `python-is-python3` only provides the `python` alias EDK2 BaseTools expects; it
# installs no Python packages, so it does not pollute a global site-packages.

RUN git clone --depth 1 --branch "$EDK2_TAG" \
        https://github.com/tianocore/edk2.git /edk2 \
 && cd /edk2 && git submodule update --init --depth 1 \
 && make -C BaseTools

# AArch64 firmware (uses GCC5_AARCH64_PREFIX): native gcc on arm64, cross on amd64.
RUN cd /edk2 && bash -c '. ./edksetup.sh && \
      if [ "$TARGETARCH" = arm64 ]; then P= ; else P=aarch64-linux-gnu- ; fi && \
      GCC5_AARCH64_PREFIX=$P build -a AARCH64 -t GCC5 -b NOOPT -p ArmVirtPkg/ArmVirtQemu.dsc'

# X64 firmware (uses GCC5_BIN): native gcc on amd64, cross on arm64.
RUN cd /edk2 && bash -c '. ./edksetup.sh && \
      if [ "$TARGETARCH" = amd64 ]; then P= ; else P=x86_64-linux-gnu- ; fi && \
      GCC5_BIN=$P build -a X64 -t GCC5 -b NOOPT -p OvmfPkg/OvmfPkgX64.dsc'

# ---- Stage 1b: network-enabled firmware (full UEFI network stack) ----------------
# Layered on ovmf-builder to reuse the edk2 checkout + BaseTools. Rebuilds both .fd
# sets with the whole NetworkPkg stack turned on (SNP/MNP/ARP/IP4+IP6/UDP/TCP/DHCP/
# DNS/HTTP/TLS + VirtioNetDxe). Kept SEPARATE from the lean default firmware so the
# base tests stay small/fast; extracted to ./firmware/net via docker/build-ovmf-net.sh
# and selected by the harness `--net` flag. Rebuilt rarely (EDK2_TAG / flag change).
FROM ovmf-builder AS ovmf-net-builder
ARG TARGETARCH
ENV TARGETARCH=${TARGETARCH}
# Full stack incl. IPv6, TLS, and HTTP (with cleartext allowed for test servers).
ARG NET_FLAGS="-D NETWORK_ENABLE=TRUE -D NETWORK_IP6_ENABLE=TRUE -D NETWORK_TLS_ENABLE=TRUE -D NETWORK_HTTP_BOOT_ENABLE=TRUE -D NETWORK_HTTP_ENABLE=TRUE -D NETWORK_ALLOW_HTTP_CONNECTIONS=TRUE"
ENV NET_FLAGS=${NET_FLAGS}
RUN cd /edk2 && bash -c '. ./edksetup.sh && \
      if [ "$TARGETARCH" = arm64 ]; then P= ; else P=aarch64-linux-gnu- ; fi && \
      GCC5_AARCH64_PREFIX=$P build -a AARCH64 -t GCC5 -b NOOPT -p ArmVirtPkg/ArmVirtQemu.dsc $NET_FLAGS'
RUN cd /edk2 && bash -c '. ./edksetup.sh && \
      if [ "$TARGETARCH" = amd64 ]; then P= ; else P=x86_64-linux-gnu- ; fi && \
      GCC5_BIN=$P build -a X64 -t GCC5 -b NOOPT -p OvmfPkg/OvmfPkgX64.dsc $NET_FLAGS'

# ---- Stage 2: dev / build / test / debug ----
# ---- Stage `build`: just the build toolchain (compile the .efi + mpy-cross/freeze) ----
# Deliberately NO qemu, venv, or baked firmware — so building this stage does NOT pull in
# the EDK2 build that the `dev` stage's `COPY --from=ovmf-builder` requires. CI uses this
# (`docker build --target build`): it runs QEMU on the host and builds the release firmware
# separately (and caches it), so it never needs EDK2 baked into the toolchain image.
FROM ubuntu:24.04 AS build
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential clang lld llvm \
      git make python3 python3-venv \
      mtools dosfstools \
      ca-certificates && rm -rf /var/lib/apt/lists/*
WORKDIR /work

# ---- Stage `dev`: full local environment = build toolchain + QEMU + venv + firmware ----
# For `make run` / `make docker-<mode>` / interactive use. Building this DOES build
# ovmf-builder (EDK2), via the firmware COPY below — that's expected; only local dev needs it.
FROM build AS dev
RUN apt-get update && apt-get install -y --no-install-recommends \
      qemu-system-x86 qemu-system-arm gdb-multiarch \
      ca-certificates && rm -rf /var/lib/apt/lists/*
# Python is managed by uv — a dedicated venv, never global pip installs.
COPY --from=ghcr.io/astral-sh/uv:latest /uv /uvx /usr/local/bin/
RUN uv venv /opt/venv \
 && VIRTUAL_ENV=/opt/venv uv pip install --no-cache pexpect pyserial
# Put the venv first on PATH so `python`/`python3` resolve to it everywhere.
ENV VIRTUAL_ENV=/opt/venv
ENV PATH=/opt/venv/bin:$PATH

# Carry the prebuilt firmware so `make run` / harness modes need no separate fetch.
# X64:
COPY --from=ovmf-builder /edk2/Build/OvmfX64/NOOPT_GCC5/FV/OVMF_CODE.fd /opt/ovmf/OVMF_CODE.fd
COPY --from=ovmf-builder /edk2/Build/OvmfX64/NOOPT_GCC5/FV/OVMF_VARS.fd /opt/ovmf/OVMF_VARS.fd
# AArch64: the QEMU `virt` machine's two pflash banks are each 64 MiB, so pad.
COPY --from=ovmf-builder /edk2/Build/ArmVirtQemu-AARCH64/NOOPT_GCC5/FV/QEMU_EFI.fd  /opt/ovmf/AAVMF_CODE.fd
COPY --from=ovmf-builder /edk2/Build/ArmVirtQemu-AARCH64/NOOPT_GCC5/FV/QEMU_VARS.fd /opt/ovmf/AAVMF_VARS.fd
RUN truncate -s 64M /opt/ovmf/AAVMF_CODE.fd /opt/ovmf/AAVMF_VARS.fd
