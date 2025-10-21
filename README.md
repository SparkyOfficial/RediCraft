# RediCraft - Distributed Key-Value Store for Minecraft Plugins

[![Build Status](https://github.com/your-username/RediCraft/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/your-username/RediCraft/actions)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

RediCraft is a high-performance, distributed in-memory key-value store specifically designed for Minecraft plugin development. It provides Redis-like functionality with additional features tailored for game server applications.

## Features

- **High Performance**: Asynchronous I/O with ASIO and multi-threading
- **Distributed System**: Master-slave replication and clustering support
- **Data Persistence**: Automatic data saving and loading
- **Multiple Data Types**: Strings, hashes, and lists
- **Minecraft Integration**: Specialized client library for Minecraft plugins
- **Easy Configuration**: Flexible configuration system

## Components

1. **RediCraft-Server**: C++ high-performance key-value store
2. **RediCraft-API**: Java client library for Minecraft plugins

## Quick Start

### Prerequisites
- CMake 3.10+ and C++17 compiler for the server
- JDK 8+ and Maven 3.6+ for the client

### Building

```bash
# Build the server
cd RediCraft-Server
cmake -B build
cmake --build build

# Build the client
cd RediCraft-API
mvn clean package
```

### Running

```bash
# Start the server
cd RediCraft-Server/build/Debug
./RediCraftServer

# Use in your Minecraft plugin
RedicraftClient client = new RedicraftClient();
client.connect("localhost", 7379);
client.set("player:Sparky:money", "1000");
String money = client.get("player:Sparky:money");
```

## Documentation

For detailed documentation, please see:
- [Main Documentation](Guide/README.md)
- [Architecture](Guide/ARCHITECTURE.md)
- [API Reference](Guide/API_REFERENCE.md)
- [Usage Guide](Guide/USAGE.md)

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for details.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support

If you encounter any issues or have questions, please [open an issue](https://github.com/your-username/RediCraft/issues/new) on GitHub.