/*
 * server.cpp
 * author: Андрій Будильников
 */

#include "server.h"
#include "storage.h"
#include "config.h"
#include "session.h"
#include "replication.h"
#include "cluster.h"
#include <iostream>
#include <functional>
#include <memory>

#ifdef ASIO_STANDALONE
using asio::ip::tcp;
#else
using asio::ip::tcp;
#endif

Server::Server(asio::io_context& io_context, short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
      storage_(std::make_unique<Storage>()),
      replication_enabled_(false),
      clustering_enabled_(false) {
}

void Server::start() {
    do_accept();
}

void Server::stop() {
    acceptor_.close();
    if (replication_manager_) {
        if (replication_manager_->getReplicationRole() == ReplicationRole::MASTER) {
            replication_manager_->stopMaster();
        } else if (replication_manager_->getReplicationRole() == ReplicationRole::SLAVE) {
            replication_manager_->stopSlave();
        }
    }
    if (cluster_manager_) {
        cluster_manager_->stopCluster();
        cluster_manager_->stopNodeDiscovery();
    }
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                // Create a new session for the client
                std::make_shared<Session>(std::move(socket), *storage_)->start();
            }
            
            // Continue accepting new connections
            do_accept();
        });
}

void Server::enableReplication(ReplicationRole role, const std::string& master_host, int master_port) {
    if (!replication_manager_) {
        replication_manager_ = std::make_unique<ReplicationManager>(*storage_, role);
    } else {
        replication_manager_->setReplicationRole(role);
    }
    
    if (role == ReplicationRole::MASTER) {
        replication_manager_->startMaster(master_port);
    } else if (role == ReplicationRole::SLAVE) {
        replication_manager_->startSlave(master_host, master_port);
    }
    
    replication_enabled_ = true;
}

void Server::disableReplication() {
    if (replication_manager_) {
        if (replication_manager_->getReplicationRole() == ReplicationRole::MASTER) {
            replication_manager_->stopMaster();
        } else if (replication_manager_->getReplicationRole() == ReplicationRole::SLAVE) {
            replication_manager_->stopSlave();
        }
    }
    replication_enabled_ = false;
}

void Server::enableClustering(int cluster_port) {
    if (!cluster_manager_) {
        cluster_manager_ = std::make_unique<ClusterManager>(*storage_);
    }
    
    cluster_manager_->startCluster(cluster_port);
    cluster_manager_->startNodeDiscovery();
    clustering_enabled_ = true;
    
    std::cout << "Clustering enabled on port " << cluster_port << std::endl;
}

void Server::disableClustering() {
    if (cluster_manager_) {
        cluster_manager_->stopCluster();
        cluster_manager_->stopNodeDiscovery();
    }
    clustering_enabled_ = false;
    
    std::cout << "Clustering disabled" << std::endl;
}

void Server::addClusterNode(const std::string& host, int port, bool is_master) {
    if (cluster_manager_) {
        cluster_manager_->addNode(host, port, is_master);
    }
}

void Server::removeClusterNode(const std::string& host, int port) {
    if (cluster_manager_) {
        cluster_manager_->removeNode(host, port);
    }
}