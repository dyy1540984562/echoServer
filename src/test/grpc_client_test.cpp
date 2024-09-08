#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"
#include <gflags/gflags.h>

DEFINE_int32(concurrency, 1, "Number of concurrent clients");
DEFINE_int32(total_requests, 1000, "Total number of requests to send");
DEFINE_string(server_address, "localhost:50051", "gRPC server address");

class GrpcClient {
public:
    GrpcClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(myservice::MyService::NewStub(channel)) {}

    std::string SayHello(const std::string& name) {
        myservice::HelloRequest request;
        request.set_name(name);

        myservice::HelloReply reply;
        grpc::ClientContext context;

        grpc::Status status = stub_->SayHello(&context, request, &reply);

        if (status.ok()) {
            return reply.message();
        } else {
            std::cout << "RPC failed: " << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }

private:
    std::unique_ptr<myservice::MyService::Stub> stub_;
};

void run_client(int id, std::atomic<int>& request_count, std::chrono::steady_clock::time_point& start_time) {
    GrpcClient client(grpc::CreateChannel(FLAGS_server_address, grpc::InsecureChannelCredentials()));
    
    while (true) {
        int current_count = request_count.fetch_add(1);
        if (current_count >= FLAGS_total_requests) {
            break;
        }
        
        std::string reply = client.SayHello("Client " + std::to_string(id));
        // Uncomment the next line if you want to see each response (may affect performance measurement)
        // std::cout << "Client " << id << " Received: " << reply << std::endl;
    }
}

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    
    std::vector<std::thread> threads;
    std::atomic<int> request_count(0);
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < FLAGS_concurrency; ++i) {
        threads.emplace_back(run_client, i, std::ref(request_count), std::ref(start_time));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    double qps = static_cast<double>(FLAGS_total_requests) / (duration.count() / 1000.0);

    std::cout << "Total requests: " << FLAGS_total_requests << std::endl;
    std::cout << "Concurrency: " << FLAGS_concurrency << std::endl;
    std::cout << "Time taken: " << duration.count() << " ms" << std::endl;
    std::cout << "Requests per second: " << qps << std::endl;

    return 0;
}