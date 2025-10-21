/*
 * cluster.cpp
 * author: Андрій Будильников
 */

#include "../include/cluster.h"
#include "../include/parser.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <functional>

#ifdef ASIO_STANDALONE
using asio::ip::tcp;
#else
using asio::ip::tcp;
#endif

ClusterManager::ClusterManager(Storage& storage)
    : storage_(storage)
    , cluster_running_(false)
    , discovery_running_(false) {
}

ClusterManager::~ClusterManager() {
    stopCluster();
    stopNodeDiscovery();
}

void ClusterManager::addNode(const std::string& host, int port, bool is_master) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    nodes_.emplace_back(host, port, is_master);
    std::cout << "Added node " << host << ":" << port << " to cluster" << std::endl;
}

void ClusterManager::removeNode(const std::string& host, int port) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    nodes_.erase(
        std::remove_if(nodes_.begin(), nodes_.end(),
            [&host, port](const ClusterNode& node) {
                return node.host == host && node.port == port;
            }),
        nodes_.end());
    std::cout << "Removed node " << host << ":" << port << " from cluster" << std::endl;
}

void ClusterManager::startCluster(int port) {
    if (cluster_running_) {
        std::cerr << "Cluster already running" << std::endl;
        return;
    }
    
    try {
        cluster_io_context_ = std::make_unique<asio::io_context>();
        cluster_acceptor_ = std::make_unique<tcp::acceptor>(*cluster_io_context_, tcp::endpoint(tcp::v4(), port));
        cluster_running_ = true;
        
        // Start acceptor thread
        cluster_threads_.emplace_back([this]() {
            clusterAcceptLoop();
        });
        
        // Start io_context threads
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 4;
        
        for (unsigned int i = 0; i < num_threads; ++i) {
            cluster_threads_.emplace_back([this]() {
                cluster_io_context_->run();
            });
        }
        
        std::cout << "Cluster manager started on port " << port << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start cluster manager: " << e.what() << std::endl;
        cluster_running_ = false;
    }
}

void ClusterManager::stopCluster() {
    if (cluster_running_) {
        cluster_running_ = false;
        
        if (cluster_acceptor_) {
            cluster_acceptor_->close();
        }
        
        if (cluster_io_context_) {
            cluster_io_context_->stop();
        }
        
        for (auto& thread : cluster_threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        cluster_threads_.clear();
        std::cout << "Cluster manager stopped" << std::endl;
    }
}

void ClusterManager::startNodeDiscovery() {
    if (discovery_running_) {
        std::cerr << "Node discovery already running" << std::endl;
        return;
    }
    
    discovery_running_ = true;
    discovery_thread_ = std::thread([this]() {
        nodeDiscoveryLoop();
    });
    
    std::cout << "Node discovery started" << std::endl;
}

void ClusterManager::stopNodeDiscovery() {
    if (discovery_running_) {
        discovery_running_ = false;
        
        if (discovery_thread_.joinable()) {
            discovery_thread_.join();
        }
        
        std::cout << "Node discovery stopped" << std::endl;
    }
}

void ClusterManager::clusterAcceptLoop() {
    if (!cluster_acceptor_ || !cluster_io_context_) {
        return;
    }
    
    cluster_acceptor_->listen();
    
    while (cluster_running_) {
        try {
            tcp::socket socket(*cluster_io_context_);
            asio::error_code ec;
            cluster_acceptor_->accept(socket, ec);
            
            if (!ec && cluster_running_) {
                // Handle node connection in a separate thread
                std::thread([this, socket = std::move(socket)]() mutable {
                    handleNodeConnection(std::move(socket));
                }).detach();
            }
        } catch (const std::exception& e) {
            if (cluster_running_) {
                std::cerr << "Error in cluster accept loop: " << e.what() << std::endl;
            }
        }
    }
}

void ClusterManager::handleNodeConnection(tcp::socket socket) {
    try {
        std::array<char, 1024> data;
        asio::error_code ec;
        
        while (cluster_running_ && socket.is_open()) {
            size_t length = socket.read_some(asio::buffer(data), ec);
            
            if (ec) {
                if (ec != asio::error::eof) {
                    std::cerr << "Error reading from node: " << ec.message() << std::endl;
                }
                break;
            }
            
            std::string command(data.data(), length);
            
            // Process cluster commands
            if (command.substr(0, 5) == "PING ") {
                // Respond to ping
                std::string response = "PONG\r\n";
                asio::write(socket, asio::buffer(response), ec);
            } else if (command.substr(0, 4) == "NODE") {
                // Node registration
                std::string response = "NODE_OK\r\n";
                asio::write(socket, asio::buffer(response), ec);
            } else {
                // Forward command to storage
                // This is a simplified implementation
                std::string response = "COMMAND_PROCESSED\r\n";
                asio::write(socket, asio::buffer(response), ec);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling node connection: " << e.what() << std::endl;
    }
}

void ClusterManager::nodeDiscoveryLoop() {
    while (discovery_running_) {
        try {
            pingNodes();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        } catch (const std::exception& e) {
            std::cerr << "Error in node discovery loop: " << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
}

void ClusterManager::pingNodes() {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    for (auto& node : nodes_) {
        try {
            asio::io_context io_context;
            tcp::resolver resolver(io_context);
            tcp::socket socket(io_context);
            
            asio::error_code ec;
            auto endpoints = resolver.resolve(node.host, std::to_string(node.port), ec);
            
            if (!ec) {
                asio::connect(socket, endpoints, ec);
                
                if (!ec) {
                    // Send ping
                    std::string ping = "PING CLUSTER\r\n";
                    asio::write(socket, asio::buffer(ping), ec);
                    
                    if (!ec) {
                        // Read response
                        std::array<char, 256> buffer;
                        size_t length = socket.read_some(asio::buffer(buffer), ec);
                        
                        if (!ec) {
                            std::string response(buffer.data(), length);
                            if (response.find("PONG") != std::string::npos) {
                                node.is_alive = true;
                                continue;
                            }
                        }
                    }
                }
            }
            
            node.is_alive = false;
            std::cout << "Node " << node.host << ":" << node.port << " is not responding" << std::endl;
            
        } catch (const std::exception& e) {
            node.is_alive = false;
            std::cout << "Error pinging node " << node.host << ":" << node.port << ": " << e.what() << std::endl;
        }
    }
}

bool ClusterManager::routeRequest(const std::string& key, const std::string& command) {
    // Calculate hash slot for the key
    size_t slot = calculateHashSlot(key);
    
    // Find the node responsible for this slot
    ClusterNode* node = findNodeForSlot(slot);
    
    if (!node) {
        std::cerr << "No node found for key: " << key << std::endl;
        return false;
    }
    
    if (!node->is_alive) {
        std::cerr << "Node " << node->host << ":" << node->port << " is not alive" << std::endl;
        return false;
    }
    
    // In a real implementation, we would forward the command to the appropriate node
    // For now, we'll just process it locally
    std::cout << "Routing request for key '" << key << "' to node " << node->host << ":" << node->port << std::endl;
    return true;
}

std::vector<ClusterNode> ClusterManager::getClusterNodes() const {
    // For const methods, we need to use a different approach to lock
    // In a real implementation, we would use a shared_mutex or similar
    // For now, we'll just return a copy without locking (simplified implementation)
    return nodes_;
}

bool ClusterManager::isClusterHealthy() const {
    // For const methods, we need to use a different approach to lock
    // In a real implementation, we would use a shared_mutex or similar
    // For now, we'll just check without locking (simplified implementation)
    
    if (nodes_.empty()) {
        return false;
    }
    
    int alive_nodes = 0;
    for (const auto& node : nodes_) {
        if (node.is_alive) {
            alive_nodes++;
        }
    }
    
    // Cluster is healthy if at least half the nodes are alive
    return alive_nodes >= (nodes_.size() + 1) / 2;
}

size_t ClusterManager::calculateHashSlot(const std::string& key) const {
    // Simple hash function for demonstration
    // In a real implementation, you might use a more sophisticated hash function
    std::hash<std::string> hasher;
    return hasher(key) % 16384; // Redis uses 16384 hash slots
}

ClusterNode* ClusterManager::findNodeForSlot(size_t slot) {
    std::lock_guard<std::mutex> lock(nodes_mutex_);
    
    if (nodes_.empty()) {
        return nullptr;
    }
    
    // Simple distribution - assign slots to nodes in round-robin fashion
    size_t node_index = slot % nodes_.size();
    return &nodes_[node_index];
}