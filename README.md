# rafi-emu

RAFI (RISCV Akifumi Fujita Implementation) is my hobby project to make a RISCV processor.

The main purpose of this emulator (rafi-emu) is to help debugging my HDL implementation.
It's not a pragmatic emulator for general purposes.

**This repository is archived and integrated into https://github.com/fjt7tdmi/rafi-1st** .

![](https://github.com/fjt7tdmi/rafi-emu/workflows/run-test/badge.svg)

## Implemented features

* RV32GC (RV32IMAFDC)
* RV64GC (RV64IMAFDC)
* RV32I privileged instructions with virtual memory
* RV64I privileged instructions with virtual memory

## Supported environment

* Windows 10
* Ubuntu 18.04

## How to build

### Install required modules

#### Windows 10

Install the following programs manually.

* Visual Studio 2019
* Boost (>= 1.65)
* CMake (>= 3.8)
* MSYS2 (CUI environment)
* Python (>= 3.6)

#### Ubuntu 18.04
```
apt-get install cmake libboost-filesystem1.65.1 libboost-program-options1.65.1 libboost1.65-dev
```

### Run build scripts
```
# Build googletest in git submodule
./script/build_gtest.sh

# Debug build
./script/build_debug.sh

# Release build
./script/build_release.sh
```

## How to run unit tests
```
./script/run_unit_test.sh
```

## How to run riscv-tests and linux

First, it's necessary to checkout prebuilt binaries.
However, the prebuilt binaries repository is private because of a license issue now (Jan 2020).
Sorry!

```
./script/checkout_prebuilt_binary.sh
```

Then, run riscv-tests or linux.

```
# Run riscv-tests
./script/run_riscv_tests.sh

# Boot linux (it will halt while running /init because of my bug :P)
./script/run_linux.sh
```
