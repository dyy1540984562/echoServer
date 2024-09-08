#include "grpc_handler_async.h"
#include <thread>
#include <glog/logging.h>

AsyncGrpcHandler::AsyncGrpcHandler(std::shared_ptr<HelloService> service)
    : service_(service) {}

void AsyncGrpcHandler::HandleRpcs(grpc::ServerCompletionQueue* cq) {
    new CallData(&async_service_, cq, service_);
    void* tag;
    bool ok;
    while (true) {
        if (!cq->Next(&tag, &ok)) {
            LOG(ERROR) << "AsyncGrpcHandler::HandleRpcs: CompletionQueue is shutting down";
            return;
        }
        if (!ok) {
            LOG(WARNING) << "AsyncGrpcHandler::HandleRpcs: Event not ok";
            continue;
        }
        static_cast<CallData*>(tag)->Proceed();
    }
}

AsyncGrpcHandler::CallData::CallData(myservice::MyService::AsyncService* service, grpc::ServerCompletionQueue* cq, std::shared_ptr<HelloService> hello_service)
    : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE), hello_service_(hello_service) {
    Proceed();
}

void AsyncGrpcHandler::CallData::Proceed() {
    if (status_ == CREATE) {
        status_ = PROCESS;
        service_->RequestSayHello(&ctx_, &request_, &responder_, cq_, cq_, this);
    } else if (status_ == PROCESS) {
        new CallData(service_, cq_, hello_service_);
        std::string response = hello_service_->say_hello(request_.name());
        reply_.set_message(response);
        status_ = FINISH;
        responder_.Finish(reply_, grpc::Status::OK, this);
    } else {
        if (status_ != FINISH) {
            LOG(ERROR) << "AsyncGrpcHandler::CallData::Proceed: Unexpected status";
        }
        delete this;
    }
}

// 实现其他必要的方法