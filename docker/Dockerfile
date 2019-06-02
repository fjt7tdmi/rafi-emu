FROM ubuntu:18.04

MAINTAINER Akifumi Fujita

# Install basic packages
RUN apt-get update
RUN apt-get install -y \
    git \
    wget

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

# Install packages for freedom-u-sdk
RUN apt-get install -y \
    cpio \
    python \
    unzip \
    rsync \
    libglib2.0-dev \
    libpixman-1-dev \
    linux-firmware

# Install packages for rafi-emu
RUN apt-get install -y \
    cmake \
    libboost-dev

# Setup envvar and workdir
ENV RISCV /opt/riscv
ENV PATH $RISCV/bin:$PATH
RUN mkdir -p $RISCV

# Setup riscv-gnu-toolchain
WORKDIR $RISCV
RUN git clone https://github.com/riscv/riscv-gnu-toolchain.git

WORKDIR $RISCV/riscv-gnu-toolchain
RUN git submodule update --init --recursive
RUN ./configure --prefix=/opt/riscv
RUN make
RUN make linux

# Setup riscv-tools
WORKDIR $RISCV
RUN git clone https://github.com/riscv/riscv-tools.git

WORKDIR $RISCV/riscv-tools
RUN git submodule update --init --recursive
RUN ./build.sh

# Setup freedom-u-sdk
WORKDIR $RISCV
RUN git clone https://github.com/sifive/freedom-u-sdk.git

WORKDIR $RISCV/freedom-u-sdk
RUN export unset RISCV
RUN git submodule update --init --recursive
RUN make bbl
RUN make vmlinux

# Re: Setup envvar
ENV RISCV /opt/riscv
ENV RISCV_TESTS $RISCV/riscv-tools/riscv-tests
ENV RAFI_FREEDOM_U_SDK $RISCV/freedom-u-sdk

# Setup rafi-emu
WORKDIR $RISCV
RUN git clone https://github.com/fjt7tdmi/rafi-emu -b hifiveu-2.0-alpha

WORKDIR $RISCV/riscv-tools
RUN git submodule update --init --recursive
RUN ./script/build_gtest.sh
RUN ./script/build_debug.sh
RUN ./script/build_release.sh
RUN ./script/prepare_riscv_tests.sh
RUN ./script/prepare_linux.sh
