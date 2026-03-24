FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake \
    wget \
    lsb-release \
    software-properties-common \
    gnupg \
    build-essential \
    zlib1g-dev \
    libzstd-dev \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 21 all && \
    rm llvm.sh

WORKDIR /app

COPY . .

RUN cmake -B build -DLLVM_DIR=/usr/lib/llvm-21/lib/cmake/llvm -DBUILD_TESTING=OFF &&\
    cmake --build build

ENTRYPOINT ["./build/compiler"]