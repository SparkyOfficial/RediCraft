/*
 * server.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_SERVER_H
#define REDICRAFT_SERVER_H

#ifdef ASIO_STANDALONE
#include <asio.hpp>
#else
#include <asio.hpp>
#endif
#include <memory>
#include <thread>
#include <vector>
#include "storage.h"
#include "replication.h"
#include "cluster.h"

class Config;

class Server {
public:
    Server(asio::io_context& io_context, short port);
    void start();
    void stop();
    
    // Replication methods
    void enableReplication(ReplicationRole role, const std::string& master_host = "", int master_port = 0);
    void disableReplication();
    
    // Cluster methods
    void enableClustering(int cluster_port);
    void disableClustering();
    void addClusterNode(const std::string& host, int port, bool is_master = false);
    void removeClusterNode(const std::string& host, int port);

private:
    void do_accept();
    
    asio::ip::tcp::acceptor acceptor_;
    std::unique_ptr<Storage> storage_;
    std::vector<std::thread> threads_;
    
    // Replication support
    std::unique_ptr<ReplicationManager> replication_manager_;
    bool replication_enabled_;
    
    // Cluster support
    std::unique_ptr<ClusterManager> cluster_manager_;
    bool clustering_enabled_;
};

#endif // REDICRAFT_SERVER_H