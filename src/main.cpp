#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>  // 使用 Boost 的 filesystem 库
#include "interface/grpc_handler.h"
#include "interface/http_handler.h"
#include "service/hello_service.h"
#include "dao/hello_dao.h"
#include "common.h"
#include <grpc/support/log.h>
#include "interface/grpc_handler_async.h"

// 定义命令行参数
DEFINE_int32(num_threads, 32, "Number of threads for the thread pool");
DEFINE_string(grpc_address, "0.0.0.0:50051", "gRPC server address");
DEFINE_string(http_address, "http://0.0.0.0:8080", "HTTP server address");
DEFINE_int32(max_concurrent_streams, 1000, "Maximum number of concurrent streams for gRPC");
// 移除这行，因为 glog 已经定义了 log_dir
// DEFINE_string(log_dir, "", "Directory to store log files");

// gRPC服务器运行函数
void run_grpc_server_async(std::shared_ptr<HelloService> service, int num_threads) {
    std::string server_address(FLAGS_grpc_address);
    AsyncGrpcHandler async_handler(service);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(async_handler.GetAsyncService());  // 修改这里
    
    builder.SetMaxReceiveMessageSize(-1);
    builder.SetMaxSendMessageSize(-1);
    builder.AddChannelArgument(GRPC_ARG_MAX_CONCURRENT_STREAMS, FLAGS_max_concurrent_streams);

    std::vector<std::unique_ptr<grpc::ServerCompletionQueue>> cqs;
    for (int i = 0; i < num_threads; i++) {
        cqs.push_back(builder.AddCompletionQueue());
    }
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    LOG_INFO("Async gRPC server listening on " << server_address);

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back([&async_handler, &cqs, i]() {
            async_handler.HandleRpcs(cqs[i].get());
        });
    }

    server->Wait();

    for (auto& t : threads) {
        t.join();
    }
}

// HTTP服务器运行函数
void run_http_server(std::shared_ptr<HelloService> service, boost::asio::io_context& io_context) {
    // 创建HTTP监听器
    web::http::experimental::listener::http_listener listener(FLAGS_http_address);
    // 创建HTTP处理器实例
    HttpHandler handler(service);
    // 设置路由
    handler.setup_routes(listener);

    try {
        // 打开监听器并等待
        listener.open().wait();
        LOG_INFO("HTTP server listening on " << FLAGS_http_address);
        
        // 创建 work_guard 以保持 io_context 运行
        auto work_guard = boost::asio::make_work_guard(io_context);
        
        // 运行 io_context
        io_context.run();
    }
    catch (const std::exception & e) {
        LOG_ERROR("Exception in HTTP server: " << e.what());
    }
    LOG_INFO("Http Server shutting down");
}

int main(int argc, char* argv[]) {
    // 初始化 gflags
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    
    // 初始化 glog
    google::InitGoogleLogging(argv[0]);
    
    // 设置日志目录
    if (!FLAGS_log_dir.empty()) {
        // 确保日志目录存在
        boost::filesystem::create_directories(FLAGS_log_dir);
        
        // 设置日志文件名前缀
        google::SetLogDestination(google::INFO, (FLAGS_log_dir + "/info_").c_str());
        google::SetLogDestination(google::WARNING, (FLAGS_log_dir + "/warning_").c_str());
        google::SetLogDestination(google::ERROR, (FLAGS_log_dir + "/error_").c_str());
        
        // 设置 vlog 文件名前缀
        google::SetVLOGLevel("*", 0);  // 设置所有文件的 VLOG 级别为 0
        google::SetStderrLogging(google::INFO);  // 设置标准错误输出的最低日志级别
    }
    
    // 同时输出到文件和 stderr
    FLAGS_alsologtostderr = true;
    
    // 设置日志前缀格式，包含线程ID
    FLAGS_log_prefix = true;

    LOG_INFO("Starting server...");

    // 创建DAO层实例
    auto dao = std::make_shared<HelloDao>();
    // 创建服务层实例，注入DAO依赖
    auto service = std::make_shared<HelloService>(dao);

    // 创建boost::asio::io_context
    boost::asio::io_context io_context;

    // 创建 work_guard 以保持 io_context 运行
    auto work_guard = boost::asio::make_work_guard(io_context);

    // 创建并启动HTTP处理程池
    std::vector<std::thread> http_handler_threads;
    for (int i = 0; i < FLAGS_num_threads / 2; ++i) {
        http_handler_threads.emplace_back([&io_context]() { 
            io_context.run(); 
        });
    }

    // 创建并启动异步gRPC服务器线程
    std::thread grpc_thread(run_grpc_server_async, service, FLAGS_num_threads / 2);

    // 创建并启动HTTP服务器设置线程
    std::thread http_setup_thread(run_http_server, service, std::ref(io_context));

    LOG_INFO("Server threads started");

    // 等待线程结束（实际上它们不会结束，除非发生错误或手动终止）
    grpc_thread.join();
    http_setup_thread.join();
    for (auto& t : http_handler_threads) {
        t.join();
    }

    LOG_INFO("Server shutting down");

    return 0;
}