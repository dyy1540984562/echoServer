#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <gflags/gflags.h>

using namespace web;
using namespace web::http;
using namespace web::http::client;

DEFINE_int32(concurrency, 1, "Number of concurrent clients");
DEFINE_int32(total_requests, 1000, "Total number of requests to send");
DEFINE_string(server_address, "http://localhost:8080", "HTTP server address");

void run_client(int id, std::atomic<int>& request_count, std::chrono::steady_clock::time_point& start_time) {
    http_client client(U(FLAGS_server_address));
    uri_builder builder(U("/hello"));
    builder.append_query(U("name"), U("Client") + utility::conversions::to_string_t(std::to_string(id)));

    while (true) {
        int current_count = request_count.fetch_add(1);
        if (current_count >= FLAGS_total_requests) {
            break;
        }

        auto task = client.request(methods::GET, builder.to_string())
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_string();
                }
                throw std::runtime_error("HTTP request failed");
            })
            .then([](std::string body) {
                // Uncomment the next line if you want to see each response (may affect performance measurement)
                // std::cout << "Received: " << body << std::endl;
            });

        try {
            task.wait();
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
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