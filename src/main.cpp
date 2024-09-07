#include <iostream>
#include <memory>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <glog/logging.h>
#include "interface/grpc_handler.h"
#include "interface/http_handler.h"
#include "service/hello_service.h"
#include "dao/hello_dao.h"

// gRPC服务器运行函数
void run_grpc_server(std::shared_ptr<HelloService> service) {
    // 设置gRPC服务器地址和端口
    std::string server_address("0.0.0.0:50051");
    // 创建gRPC处理器实例
    GrpcHandler handler(service);

    // 配置gRPC服务器
    grpc::ServerBuilder builder;
    // 添加监听端口，使用不安全的证书（仅用于测试）
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // 注册服务
    builder.RegisterService(&handler);

    // 构建并启动服务器
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG(INFO) << "gRPC server listening on " << server_address;
    // 等待服务器终止（这会阻塞当前线程）
    server->Wait();
}

// HTTP服务器运行函数
void run_http_server(std::shared_ptr<HelloService> service) {
    // 创建HTTP监听器
    web::http::experimental::listener::http_listener listener("http://0.0.0.0:8080");
    // 创建HTTP处理器实例
    HttpHandler handler(service);
    // 设置路由
    handler.setup_routes(listener);

    try {
        // 打开监听器并等待
        listener.open().wait();
        LOG(INFO) << "HTTP server listening on http://0.0.0.0:8080";
        // 保持服务器运行
        while (true);
    }
    catch (const std::exception & e) {
        // 捕获并打印任何异常
        LOG(ERROR) << "Exception in HTTP server: " << e.what();
    }
}

int main(int argc, char* argv[]) {
    // 初始化 glog
    google::InitGoogleLogging(argv[0]);
    // 将日志输出到 stderr
    FLAGS_logtostderr = 1;

    LOG(INFO) << "Starting server...";

    // 创建DAO层实例
    auto dao = std::make_shared<HelloDao>();
    // 创建服务层实例，注入DAO依赖
    auto service = std::make_shared<HelloService>(dao);

    // 创建并启动gRPC服务器线程
    std::thread grpc_thread(run_grpc_server, service);
    // 创建并启动HTTP服务器线程
    std::thread http_thread(run_http_server, service);

    LOG(INFO) << "Server threads started";

    // 等待两个线程结束（实际上它们不会结束，除非发生错误或手动终止）
    grpc_thread.join();
    http_thread.join();

    LOG(INFO) << "Server shutting down";

    return 0;
}