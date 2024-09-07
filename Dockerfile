FROM ubuntu:22.04

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
    libgtest-dev
# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the project
RUN bash build.sh

# Expose the gRPC and HTTP ports
EXPOSE 50051 8080

# Run the server
CMD ["./build/myserver"]