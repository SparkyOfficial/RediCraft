/*
 * replication.h
 * author: Андрій Будильников
 */

#ifndef REDICRAFT_REPLICATION_H
#define REDICRAFT_REPLICATION_H

#include "storage.h"
#include <asio.hpp>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>

#ifdef ASIO_STANDALONE
using asio::ip::tcp;
#else
using asio::ip::tcp;
#endif

enum class ReplicationRole {
    MASTER,
    SLAVE
};

class ReplicationManager {
public:
    ReplicationManager(Storage& storage, ReplicationRole role);
    ~ReplicationManager();
    
    // Master functions
    void startMaster(int port);
    void stopMaster();
    
    // Slave functions
    void startSlave(const std::string& master_host, int master_port);
    void stopSlave();
    
    // Replication control
    void setReplicationRole(ReplicationRole role);
    ReplicationRole getReplicationRole() const;
    
    // Data synchronization
    bool syncToSlaves();
    bool syncFromMaster();
    
private:
    Storage& storage_;
    ReplicationRole role_;
    
    // Master specific
    std::unique_ptr<asio::io_context> master_io_context_;
    std::unique_ptr<tcp::acceptor> master_acceptor_;
    std::vector<std::thread> master_threads_;
    std::atomic<bool> master_running_;
    
    // Slave specific
    std::unique_ptr<asio::io_context> slave_io_context_;
    std::unique_ptr<tcp::socket> slave_socket_;
    std::thread slave_thread_;
    std::atomic<bool> slave_connected_;
    std::string master_host_;
    int master_port_;
    
    // Helper methods
    void masterAcceptLoop();
    void handleSlaveConnection(tcp::socket socket);
    void slaveConnectLoop();
    void handleMasterCommands();
    
    // Command processing
    void processReplicationCommand(const std::string& command);
    std::string generateReplicationLog(const std::string& command);
    std::string generateStorageSnapshot();

};

#endif // REDICRAFT_REPLICATION_H