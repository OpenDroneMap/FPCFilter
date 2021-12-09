FROM debian:stable AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    build-essential \
    libomp-dev \
    cmake

COPY . /fpcfilter

RUN cd fpcfilter && \
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

FROM debian:stable AS runner

RUN apt-get update && apt-get install -y --no-install-recommends \
    libgomp1 && apt-get clean

WORKDIR /root/
COPY --from=builder /fpcfilter/build ./

ENTRYPOINT  ["./FPCFilter"]  







