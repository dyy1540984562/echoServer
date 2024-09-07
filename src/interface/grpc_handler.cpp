#include "grpc_handler.h"

grpc::Status GrpcHandler::SayHello(grpc::ServerContext* context, const myservice::HelloRequest* request, myservice::HelloReply* reply) {
    std::string result = hello_service_->say_hello(request->name());
    reply->set_message(result);
    return grpc::Status::OK;
}