# gRPC Echo Server

这是一个基于gRPC和HTTP的Echo服务器示例项目,使用C++17实现。

## 使用的框架和组件

- gRPC: 用于实现RPC服务
- Protocol Buffers: 用于定义服务接口和消息格式
- cpprestsdk: 用于实现HTTP服务
- glog: 用于日志记录
- Folly: Facebook的开源C++库,提供了许多有用的组件和工具
- Boost: 广泛使用的C++库集合,提供了许多有用的工具和算法
- Docker: 用于容器化部署

## 目录结构

```
echoserver/
── protos/
│ ├── service.proto
│ ├── service.pb.cc
│ └── service.pb.h
├── src/
│ ├── main.cpp
│ ├── interface/
│ │ ├── grpc_handler.h
│ │ └── http_handler.h
│ ├── service/
│ │ └── hello_service.h
│ └── dao/
│ └── hello_dao.h
├── Dockerfile
└── readme.md
```

## 文件说明

- `protos/service.proto`: 定义gRPC服务接口和消息格式
- `protos/service.pb.cc` 和 `protos/service.pb.h`: 由Protocol Buffers编译器生成的C++代码
- `src/main.cpp`: 主程序入口,启动gRPC和HTTP服务器
- `src/interface/grpc_handler.h`: gRPC服务处理器
- `src/interface/http_handler.h`: HTTP服务处理器
- `src/service/hello_service.h`: 业务逻辑服务实现
- `src/dao/hello_dao.h`: 数据访问对象(DAO)实现
- `Dockerfile`: 用于构建Docker镜像的配置文件
- `readme.md`: 项目说明文档

## 构建和运行

1. 确保已安装Docker并启动Docker守护进程。

2. 在项目根目录下构建Docker镜像:
   ```
   docker build -t mygrpcservice_develop .
   ```

   注意：如果您有自定义的SSH配置文件，可以将其放在项目根目录的 `.ssh/` 文件夹中。
   Dockerfile会尝试复制这个文件（如果存在的话）。如果没有，构建过程会继续而不会失败。

3. 运行Docker容器（包含SSH配置，后台运行）:
   ```
   docker run -it -d \
     -p 50051:50051 -p 8080:8080 \
     -v $HOME/.ssh:/root/.ssh:ro \
     --name mygrpcservice \
     mygrpcservice_develop
   ```

   这个命令会在后台启动容器，并将您的本地SSH配置挂载到容器中。

4. 查看容器日志（如果需要）:
   ```
   docker logs mygrpcservice
   ```

5. 停止容器:
   ```
   docker stop mygrpcservice
   ```

6. 删除容器:
   ```
   docker rm mygrpcservice
   ```

## 注意事项

- 确保Dockerfile中正确设置了工作目录并复制了所有必要文件。
- 如果项目依赖外部文件或配置,请确保它们被正确复制到Docker镜像中。
- 在实际部署时,可能需要考虑使用Docker Compose来管理多个相关服务。
- 如果在构建或运行过程中遇到问题,请检查Docker的输出信息以获取错误诊断信息。
- Dockerfile中包含了从源码编译安装Boost和Folly的步骤,这可能会导致Docker镜像构建时间较长。
- 如果遇到与Folly或Boost相关的编译问题,请检查版本兼容性和编译选项。
