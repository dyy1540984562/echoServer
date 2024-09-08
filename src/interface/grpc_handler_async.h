#pragma once

#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"
#include "../service/hello_service.h"

class AsyncGrpcHandler final {
public:
    AsyncGrpcHandler(std::shared_ptr<HelloService> service);
    void HandleRpcs(grpc::ServerCompletionQueue* cq);
    myservice::MyService::AsyncService* GetAsyncService() { return &async_service_; }  // 添加这个方法

private:
    std::shared_ptr<HelloService> service_;
    myservice::MyService::AsyncService async_service_;

    class CallData {
    public:
        CallData(myservice::MyService::AsyncService* service, grpc::ServerCompletionQueue* cq, std::shared_ptr<HelloService> hello_service);
        void Proceed();

    private:
        myservice::MyService::AsyncService* service_;
        grpc::ServerCompletionQueue* cq_;
        grpc::ServerContext ctx_;
        myservice::HelloRequest request_;
        myservice::HelloReply reply_;
        grpc::ServerAsyncResponseWriter<myservice::HelloReply> responder_;
        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus status_;
        std::shared_ptr<HelloService> hello_service_;
    };
};