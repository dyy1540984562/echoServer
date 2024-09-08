cmake_minimum_required(VERSION 3.10)
project(MyGrpcService)

set(CMAKE_CXX_STANDARD 17)

# 在 CMakeLists.txt 的开头添加
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
# 手动指定库路径 告诉 CMake 在链接时也要查看这个目录
link_directories(/lib/x86_64-linux-gnu/)
# 查找依赖库
find_package(Protobuf REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GRPC REQUIRED grpc++)
pkg_check_modules(GRPCPP REQUIRED grpc++_unsecure)
find_package(cpprestsdk REQUIRED)

# 使用 pkg-config 查找 glog
pkg_check_modules(GLOG REQUIRED libglog)

# 添加 find_package 调用
find_package(GTest REQUIRED)

# 设置 Protoc 和 gRPC 插件路径
find_program(PROTOC protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

# 添加Boost库
find_package(Boost REQUIRED COMPONENTS system thread filesystem)

# 添加 gflags 库
find_package(gflags REQUIRED)

# 生成 Protobuf 和 gRPC 源代码
set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/protos/service.proto")
set(GENERATED_PROTO_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_PROTO_DIR})

add_custom_command(
    OUTPUT "${GENERATED_PROTO_DIR}/service.pb.cc"
           "${GENERATED_PROTO_DIR}/service.pb.h"
           "${GENERATED_PROTO_DIR}/service.grpc.pb.cc"
           "${GENERATED_PROTO_DIR}/service.grpc.pb.h"
    COMMAND ${PROTOC}
    ARGS --grpc_out="${GENERATED_PROTO_DIR}"
         --cpp_out="${GENERATED_PROTO_DIR}"
         -I "${CMAKE_CURRENT_SOURCE_DIR}/protos"
         --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
         "${PROTO_FILE}"
    DEPENDS "${PROTO_FILE}"
)

# 添加可执行文件
add_executable(myserver
    src/main.cpp
    src/interface/grpc_handler.cpp
    src/interface/grpc_handler_async.cpp
    src/interface/http_handler.cpp
    src/service/hello_service.cpp
    src/dao/hello_dao.cpp
    ${GENERATED_PROTO_DIR}/service.pb.cc
    ${GENERATED_PROTO_DIR}/service.pb.h
    ${GENERATED_PROTO_DIR}/service.grpc.pb.cc
    ${GENERATED_PROTO_DIR}/service.grpc.pb.h
)

# 修改设置包含目录和链接库
target_include_directories(myserver PRIVATE 
    ${GENERATED_PROTO_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interface
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dao
    ${GRPC_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${gflags_INCLUDE_DIR}
)

# 修改设置链接库
target_link_libraries(myserver
    ${GRPC_LIBRARIES}
    ${GRPCPP_LIBRARIES}
    libgpr.so.6
    ${Protobuf_LIBRARIES}
    cpprestsdk::cpprest
    ${GLOG_LIBRARIES}
    GTest::GTest
    GTest::Main
    ${Boost_LIBRARIES}
    gflags
    pthread
    Boost::filesystem
)

# 添加 gRPC 客户端测试可执行文件
add_executable(grpc_client_test
    src/test/grpc_client_test.cpp
    ${GENERATED_PROTO_DIR}/service.pb.cc
    ${GENERATED_PROTO_DIR}/service.pb.h
    ${GENERATED_PROTO_DIR}/service.grpc.pb.cc
    ${GENERATED_PROTO_DIR}/service.grpc.pb.h
)

target_include_directories(grpc_client_test PRIVATE 
    ${GENERATED_PROTO_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GRPC_INCLUDE_DIRS}
    ${Protobuf_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${gflags_INCLUDE_DIR}
)

target_link_libraries(grpc_client_test
    ${GRPC_LIBRARIES}
    ${GRPCPP_LIBRARIES}
    ${Protobuf_LIBRARIES}
    ${GLOG_LIBRARIES}
    GTest::GTest
    GTest::Main
    gflags
)

# 添加 HTTP 客户端测试可执行文件
add_executable(http_client_test
    src/test/http_client_test.cpp
)

target_link_libraries(http_client_test
    cpprestsdk::cpprest
    ${GLOG_LIBRARIES}
    GTest::GTest
    GTest::Main
    gflags
)