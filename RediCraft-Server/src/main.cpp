/*
 * main.cpp
 * author: Андрій Будильников
 */

#include "server.h"
#include "config.h"
#include "replication.h"
#include "cluster.h"
#include <asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <memory>

int main(int argc, char* argv[]) {
    try {
        // Load configuration
        Config config;
        if (!config.load("redicraft.conf")) {
            std::cout << "Using default configuration..." << std::endl;
        } else {
            std::cout << "Configuration loaded successfully." << std::endl;
        }
        
        asio::io_context io_context;
        Server server(io_context, config.getPort());
        
        std::cout << "RediCraft server starting on port " << config.getPort() << "..." << std::endl;
        
        // Check if replication is enabled in configuration
        if (config.isReplicationEnabled()) {
            if (config.getReplicationRole() == "master") {
                std::cout << "Starting server in master replication mode..." << std::endl;
                server.enableReplication(ReplicationRole::MASTER, "", config.getReplicationPort());
            } else if (config.getReplicationRole() == "slave") {
                std::cout << "Starting server in slave replication mode..." << std::endl;
                server.enableReplication(ReplicationRole::SLAVE, config.getMasterHost(), config.getMasterPort());
            }
        }
        
        // Check if clustering is enabled in configuration
        if (config.isClusteringEnabled()) {
            std::cout << "Starting server with clustering enabled..." << std::endl;
            server.enableClustering(config.getClusterPort());
            
            // Add cluster nodes from configuration
            auto clusterNodes = config.getClusterNodes();
            for (const auto& node : clusterNodes) {
                server.addClusterNode(node.host, node.port, node.is_master);
            }
        }
        
        server.start();
        
        // Run the io_context in multiple threads
        std::vector<std::thread> threads;
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) {
            num_threads = 4; // Default to 4 threads if hardware_concurrency() fails
        }
        
        std::cout << "Running with " << num_threads << " threads." << std::endl;
        
        for (unsigned int i = 0; i < num_threads; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }
        
        // Wait for all threads to complete
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}