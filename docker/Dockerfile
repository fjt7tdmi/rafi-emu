FROM ubuntu:18.04

MAINTAINER Akifumi Fujita

# Install basic packages
RUN apt-get update && apt-get install -y openssh-server
RUN apt-get install -y \
    git \
    wget

# Install packages for riscv-tools
RUN apt-get install -y \
    autoconf \
    automake \
    autotools-dev \
    curl \
    emacs \
    libboost-dev \
    libboost-program-options-dev \
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

# Setup ssh server
RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd
ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile
EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]

# Setup envvar and workdir
ENV RISCV /opt/riscv
ENV PATH $PATH:$RISCV/bin
RUN mkdir -p $RISCV

# Setup riscv-gnu-toolchain
WORKDIR $RISCV
RUN git clone https://github.com/riscv/riscv-gnu-toolchain.git

WORKDIR $RISCV/riscv-gnu-toolchain
RUN git submodule update --init --recursive
RUN ./configure --prefix=/opt/riscv
RUN make -j8
RUN make -j8 linux

# Setup riscv-tools
WORKDIR $RISCV
RUN git clone https://github.com/riscv/riscv-tools.git

WORKDIR $RISCV/riscv-tools
RUN git submodule update --init --recursive
RUN ./build.sh

# Checkout rafi-emu
WORKDIR $RISCV
RUN git clone https://github.com/fjt7tdmi/rafi-emu
RUN mkdir -p rafi-emu/dtb

# Setup freedom-u-sdk
WORKDIR $RISCV
RUN git clone https://github.com/sifive/freedom-u-sdk.git -b hifiveu-2.0-alpha

WORKDIR $RISCV/freedom-u-sdk
RUN git submodule update --init --recursive
RUN sed -i -e 's/riscv64-buildroot-linux-gnu/riscv64-unknown-linux-gnu/' /opt/riscv/freedom-u-sdk/Makefile
RUN sed -i -e 's|\-machine\ virt|\-machine\ virt,dumpdtb=/opt/riscv/rafi-emu/dtb/qemu.dtb|' /opt/riscv/freedom-u-sdk/Makefile
RUN make -j8 qemu

# Re: Setup envvar
ENV RISCV_TESTS $RISCV/riscv-tools/riscv-tests
ENV RAFI_FREEDOM_U_SDK $RISCV/freedom-u-sdk

# Build rafi-emu
WORKDIR $RISCV/rafi-emu
RUN git submodule update --init --recursive
RUN ./script/build_gtest.sh
RUN ./script/build_debug.sh
RUN ./script/build_release.sh
RUN ./script/prepare_riscv_tests.sh
RUN ./script/prepare_linux.sh
