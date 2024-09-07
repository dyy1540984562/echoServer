#pragma once
#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"
#include "../service/hello_service.h"

class GrpcHandler final : public myservice::MyService::Service {
public:
    GrpcHandler(std::shared_ptr<HelloService> service) : hello_service_(service) {}
    grpc::Status SayHello(grpc::ServerContext* context, const myservice::HelloRequest* request, myservice::HelloReply* reply) override;

private:
    std::shared_ptr<HelloService> hello_service_;
};