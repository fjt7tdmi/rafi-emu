FROM fjt7tdmi/rafi-dev:latest

# Setup envvar and workdir
ENV RISCV /opt/riscv
ENV PATH $PATH:$RISCV/bin
RUN mkdir -p $RISCV

# Checkout rafi-emu
WORKDIR $RISCV
RUN git clone https://github.com/fjt7tdmi/rafi-emu

# Build rafi-emu and run unit test
WORKDIR $RISCV/rafi-emu
RUN git submodule update --init --recursive
RUN ./script/build_gtest.sh
RUN ./script/build_debug.sh
RUN ./script/build_release.sh
RUN ./script/run_unit_test.sh
