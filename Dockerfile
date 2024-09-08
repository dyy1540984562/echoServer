FROM it-artifactory.yitu-inc.com/docker-public/ylu/dev_env:ubuntu_20.04_x86_64 

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libgrpc++-dev \
    libprotobuf-dev \
    protobuf-compiler-grpc \
    libcpprest-dev \
    libgrpc-dev \
    libgoogle-glog-dev \
    libgtest-dev \
    libssl-dev \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgflags-dev \
    libiberty-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libunwind-dev

# Install Folly from source
# RUN git clone https://github.com/facebook/folly.git && \
#     cd folly && \
#     git checkout v2021.06.28.00 && \
#     mkdir _build && \
#     cd _build && \
#     cmake .. && \
#     make -j $(nproc) && \
#     make install && \
#     cd ../.. && \
#     rm -rf folly

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the project
# RUN bash build.sh

# Expose the gRPC and HTTP ports
EXPOSE 50051 8080

# Run the server
# CMD ["./build/myserver"]