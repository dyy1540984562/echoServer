#!/bin/bash
# 运行服务器：./run.sh server [log_dir]
# 运行 HTTP 客户端测试：./run.sh http_client
# 运行 gRPC 客户端测试：./run.sh grpc_client

# 创建并进入构建目录
mkdir -p build
cd build

# 运行 CMake 和 make 来构建项目
cmake ..
make -j$(nproc)

# 检查构建是否成功
if [ $? -ne 0 ]; then
    echo "构建失败，请检查错误信息"
    exit 1
fi

# 解析命令行参数
if [ "$1" == "server" ]; then
    rm -rf myserver_logs
    echo "运行服务器..."
    # 检查是否提供了日志目录参数
    if [ -n "$2" ]; then
        LOG_DIR="$2"
    else
        # 如果没有提供日志目录，使用默认值
        LOG_DIR="/app/myserver_logs"
    fi
    # 确保日志目录存在
    mkdir -p "$LOG_DIR"
    echo "日志将保存在: $LOG_DIR"
    ./myserver --num_threads=64 --grpc_address=0.0.0.0:50051 --http_address=http://0.0.0.0:8080 --max_concurrent_streams=2000 --log_dir="$LOG_DIR" --alsologtostderr=true
elif [ "$1" == "grpc_client" ]; then
    echo "运行 gRPC 客户端测试..."
    ./grpc_client_test --concurrency=32 --total_requests=10000 --server_address=localhost:50051
elif [ "$1" == "http_client" ]; then
    echo "运行 HTTP 客户端测试..."
    ./http_client_test --concurrency=32 --total_requests=10000 --server_address=http://localhost:8080
else
    echo "用法: ./run.sh [server|http_client|grpc_client] [log_dir]"
    echo "  server [log_dir]: 运行服务器，可选参数指定日志目录"
    echo "  http_client: 运行 HTTP 客户端测试"
    echo "  grpc_client: 运行 gRPC 客户端测试"
    exit 1
fi
