#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"

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

int main(int argc, char** argv) {
    std::string target_str = "localhost:50051";
    GrpcClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
    
    std::string name = "World";
    if (argc > 1) {
        name = argv[1];
    }
    
    std::string reply = client.SayHello(name);
    std::cout << "Received: " << reply << std::endl;

    return 0;
}