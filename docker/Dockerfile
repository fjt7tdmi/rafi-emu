FROM ubuntu:18.04

MAINTAINER Akifumi Fujita

# Install basic packages
RUN apt-get update
RUN apt-get install -y git

# Install packages for riscv-tools
RUN apt-get install -y \
    autoconf \
    automake \
    autotools-dev \
    curl \
    libmpc-dev \
    libmpfr-dev \
    libgmp-dev \
    libusb-1.0-0-dev \
    gawk \
    build-essential \
    bison \
    flex \
    texinfo \
    gperf \
    libtool \
    patchutils \
    bc \
    zlib1g-dev \
    device-tree-compiler \
    pkg-config \
    libexpat-dev

# Setup envvar and workdir
ENV RISCV /opt/riscv
ENV PATH $RISCV/bin:$PATH
RUN mkdir -p $RISCV

# Checkout git repos
WORKDIR $RISCV
RUN git clone https://github.com/riscv/riscv-tools.git && \
    cd riscv-tools && \
    git submodule update --init --recursive

# Build riscv-tools
WORKDIR $RISCV/riscv-tools
RUN ./build.sh
