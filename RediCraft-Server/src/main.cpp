/*
 * main.cpp
 * author: Андрій Будильников
 */

#include "../include/server.h"
#include "../include/config.h"
#include "../include/replication.h"
#include "../include/cluster.h"
#include <iostream>
#include <thread>
#include <vector>

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
        
        // Run the io_context in multiple threads with proper strand usage
        // Using strands to ensure handlers for the same socket are not called concurrently
        std::vector<std::thread> threads;
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) {
            num_threads = 4; // Default to 4 threads if hardware_concurrency() fails
        }
        
        std::cout << "Running with " << num_threads << " threads." << std::endl;
        
        // Create a work guard to keep the io_context running
        auto work_guard = asio::make_work_guard(io_context);
        
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