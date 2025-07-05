# Stage 1: Build gnu-efi
FROM ubuntu:jammy AS build-gnu-efi

RUN apt update && apt install -y \
    gcc-x86-64-linux-gnu \
    make \
    git \
    uuid-dev

WORKDIR /build
RUN apt update && \
    apt install -y gcc-x86-64-linux-gnu binutils make git uuid-dev && \
    git clone https://github.com/vathpela/gnu-efi.git && \
    cd gnu-efi && \
    make all install \
    ARCH=x86_64 \
    CC=x86_64-linux-gnu-gcc \
    LD=x86_64-linux-gnu-ld \
    OBJCOPY=x86_64-linux-gnu-objcopy \
    PREFIX=/gnu-efi-install

# Stage 2: Build dev container
FROM mcr.microsoft.com/devcontainers/base:noble

COPY --from=build-gnu-efi /gnu-efi-install /usr/local

RUN apt update && apt install -y \
    binutils nasm clang cmake qemu-system-x86 ovmf \
    mtools dosfstools qemu-utils gdb
RUN apt install -y gcc-x86-64-linux-gnu
RUN apt install -y cpio
