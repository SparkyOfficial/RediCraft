# RediCraft Developer Guide

## Table of Contents
1. [Project Structure](#project-structure)
2. [Development Environment](#development-environment)
3. [Building the Project](#building-the-project)
4. [Code Style and Conventions](#code-style-and-conventions)
5. [Testing](#testing)
6. [Adding New Features](#adding-new-features)
7. [Documentation](#documentation)
8. [Contributing Process](#contributing-process)

## Project Structure

### Overall Directory Layout
```
RediCraft/
├── RediCraft-Server/     # C++ server implementation
│   ├── include/          # Header files
│   ├── src/              # Source files
│   ├── build/            # Build output
│   ├── CMakeLists.txt    # Build configuration
│   └── redicraft.conf    # Default configuration
├── RediCraft-API/        # Java client library
│   ├── src/              # Source and test files
│   ├── target/           # Build output
│   └── pom.xml           # Maven configuration
├── Guide/                # Documentation
├── README.md             # Project overview
├── STATUS.md             # Project status
└── demo.*                # Demo scripts
```

### RediCraft-Server Structure
```
RediCraft-Server/
├── include/
│   ├── server.h          # Server class declaration
│   ├── storage.h         # Storage class declaration
│   ├── parser.h          # Command parser declaration
│   ├── session.h         # Client session handling
│   ├── config.h          # Configuration management
│   ├── persistence.h     # Data persistence
│   ├── replication.h     # Replication management
│   └── cluster.h         # Cluster management
├── src/
│   ├── main.cpp          # Main entry point
│   ├── server.cpp        # Server implementation
│   ├── storage.cpp       # Storage implementation
│   ├── parser.cpp        # Parser implementation
│   ├── session.cpp       # Session implementation
│   ├── config.cpp        # Configuration implementation
│   ├── persistence.cpp   # Persistence implementation
│   ├── replication.cpp   # Replication implementation
│   └── cluster.cpp       # Cluster implementation
├── CMakeLists.txt        # Build configuration
└── redicraft.conf        # Default configuration
```

### RediCraft-API Structure
```
RediCraft-API/
├── src/
│   ├── main/
│   │   └── java/
│   │       └── com/
│   │           └── redicraft/
│   │               ├── RedicraftClient.java      # Main client class
│   │               ├── ConnectionPool.java       # Connection pool
│   │               └── minecraft/                # Minecraft examples
│   │                   ├── MoneyPluginExample.java
│   │                   ├── MoneyCommandExample.java
│   │                   └── AsyncMoneyPluginExample.java
│   └── test/
│       └── java/
│           └── com/
│               └── redicraft/
│                   ├── RedicraftClientTest.java  # Client tests
│                   ├── ConnectionPoolTest.java   # Pool tests
│                   └── IntegrationTest.java      # Integration tests
├── pom.xml               # Maven configuration
└── target/               # Build output
```

## Development Environment

### Prerequisites

#### For RediCraft-Server (C++)
- **Compiler**: C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **Build System**: CMake 3.10+
- **Dependencies**: ASIO library (automatically managed by CMake)
- **IDE**: Visual Studio, CLion, or any C++ IDE

#### For RediCraft-API (Java)
- **JDK**: Java 8 or higher
- **Build Tool**: Maven 3.6+
- **IDE**: IntelliJ IDEA, Eclipse, or any Java IDE

### Setting Up Development Environment

#### Windows Setup
1. Install Visual Studio with C++ development tools
2. Install CMake
3. Install JDK and set JAVA_HOME
4. Install Maven
5. Clone the repository

#### Linux/Mac Setup
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake openjdk-8-jdk maven

# macOS with Homebrew
brew install cmake openjdk@8 maven
```

## Building the Project

### Building RediCraft-Server
```bash
cd RediCraft-Server
cmake -B build
cmake --build build
```

#### Build Options
```bash
# Debug build (default)
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Verbose build
cmake --build build --verbose
```

#### Cleaning Build
```bash
# Clean build directory
rm -rf build

# Or use CMake clean
cmake --build build --target clean
```

### Building RediCraft-API
```bash
cd RediCraft-API
mvn clean package
```

#### Build Options
```bash
# Compile only
mvn compile

# Run tests
mvn test

# Create JAR without running tests
mvn package -DskipTests

# Install to local repository
mvn install
```

#### Cleaning Build
```bash
# Clean build artifacts
mvn clean

# Clean and rebuild
mvn clean package
```

## Code Style and Conventions

### C++ Code Style

#### Naming Conventions
- **Classes**: PascalCase (`Storage`, `RedicraftClient`)
- **Functions**: snake_case (`get_value`, `set_key`)
- **Variables**: snake_case (`user_name`, `max_connections`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_BUFFER_SIZE`)
- **Private members**: snake_case with trailing underscore (`data_`, `is_running_`)

#### File Organization
```cpp
/*
 * filename.h
 * author: Андрій Будильников
 */

#ifndef PROJECT_FILENAME_H
#define PROJECT_FILENAME_H

// Includes
#include <string>
#include <vector>

// Class declarations
class Storage;

class ClassName {
public:
    // Public methods
    ClassName();
    ~ClassName();
    
    // Const methods
    int get_value() const;
    
    // Non-const methods
    void set_value(int value);
    
private:
    // Private methods
    void internal_method();
    
    // Member variables
    int value_;
    std::string name_;
};

#endif // PROJECT_FILENAME_H
```

#### Comments
- Use Ukrainian for comments with author name
- Keep comments minimal and casual
- Focus on why, not what

```cpp
// simple storage for key-value pairs
// author: Андрій Будильников
class Storage {
    // ...
};
```

#### Error Handling
- Use exceptions for error conditions
- Provide meaningful error messages
- Clean up resources in destructors

```cpp
void Storage::set(const std::string& key, const std::string& value) {
    std::unique_lock lock(mutex_);
    
    if (key.empty()) {
        throw std::invalid_argument("Key cannot be empty");
    }
    
    DataItem item(value);
    string_data_[key] = item;
}
```

### Java Code Style

#### Naming Conventions
- **Classes**: PascalCase (`RedicraftClient`, `ConnectionPool`)
- **Methods**: camelCase (`getValue`, `setKey`)
- **Variables**: camelCase (`userName`, `maxConnections`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_BUFFER_SIZE`)

#### File Organization
```java
/**
 * Brief class description.
 * author: Андрій Будильников
 */

package com.redicraft;

import java.util.*;
import java.io.*;

public class ClassName {
    // Constants
    private static final int DEFAULT_PORT = 7379;
    
    // Member variables
    private String host;
    private int port;
    
    // Constructors
    public ClassName() {
        this("localhost", DEFAULT_PORT);
    }
    
    public ClassName(String host, int port) {
        this.host = host;
        this.port = port;
    }
    
    // Public methods
    public boolean connect() {
        // Implementation
        return true;
    }
    
    // Private methods
    private void internalMethod() {
        // Implementation
    }
}
```

#### Comments
- Use Ukrainian for comments with author name
- Keep comments minimal and casual
- Focus on why, not what

```java
/**
 * simple client for redicraft server
 * author: Андрій Будильников
 */
public class RedicraftClient {
    // ...
}
```

#### Error Handling
- Use try-catch blocks for exception handling
- Provide meaningful error messages
- Close resources in finally blocks or use try-with-resources

```java
public String get(String key) {
    if (key == null || key.isEmpty()) {
        throw new IllegalArgumentException("Key cannot be null or empty");
    }
    
    try {
        // Send command to server
        sendCommand("GET " + key);
        
        // Read response
        return readResponse();
    } catch (IOException e) {
        throw new RedicraftException("Failed to get key: " + key, e);
    }
}
```

## Testing

### C++ Testing

#### Unit Tests
Currently, C++ unit tests are implemented through manual testing and integration with the Java client tests.

#### Integration Testing
Integration testing is performed by:
1. Starting the RediCraft server
2. Running the Java client integration tests
3. Verifying all commands work correctly

#### Manual Testing
Use the provided test scripts:
```bash
# Run basic command tests
cd RediCraft-Server
./test_commands.bat

# Run extended command tests
./test_extended_commands.bat

# Run configuration tests
./test_config.bat
```

### Java Testing

#### Unit Tests
Unit tests are located in `src/test/java/com/redicraft/` and use JUnit 4.

```java
public class RedicraftClientTest {
    private RedicraftClient client;
    
    @Before
    public void setUp() {
        client = new RedicraftClient();
        // Setup code
    }
    
    @Test
    public void testSetAndGet() {
        assertTrue(client.set("test", "value"));
        assertEquals("value", client.get("test"));
    }
    
    @After
    public void tearDown() {
        if (client != null) {
            client.close();
        }
    }
}
```

#### Running Tests
```bash
# Run all tests
mvn test

# Run specific test class
mvn test -Dtest=RedicraftClientTest

# Run tests with verbose output
mvn test -Dverbose=true
```

#### Integration Tests
Integration tests verify client-server communication:

```java
public class IntegrationTest {
    private RedicraftClient client;
    
    @Test
    public void testServerIntegration() {
        client = new RedicraftClient();
        assertTrue("Failed to connect to server", client.connect("localhost", 7379));
        
        // Test basic operations
        assertTrue(client.set("integration_test", "test_value"));
        assertEquals("test_value", client.get("integration_test"));
        
        // Test numeric operations
        assertEquals(1L, client.incr("counter"));
        assertEquals(2L, client.incr("counter"));
        
        client.close();
    }
}
```

## Adding New Features

### Adding New Commands

#### 1. Server-Side Implementation

1. **Add command parsing** in `parser.cpp`:
```cpp
Command Parser::parse(const std::string& commandStr) {
    
    } else if (command == "NEWCOMMAND") {
        cmd.type = CommandType::NEWCOMMAND;
        // Parse arguments
        for (size_t i = 1; i < tokens.size(); ++i) {
            cmd.args.push_back(tokens[i]);
        }
    }
    
    return cmd;
}
```

2. **Add command type** in `parser.h`:
```cpp
enum class CommandType {
    // ... existing commands ...
    NEWCOMMAND,
    UNKNOWN
};
```

3. **Add storage method** in `storage.h`:
```cpp
class Storage {
public:
    // ... existing methods ...
    bool newcommand(const std::string& key, const std::string& arg);
};
```

4. **Implement storage method** in `storage.cpp`:
```cpp
bool Storage::newcommand(const std::string& key, const std::string& arg) {
    std::unique_lock lock(mutex_);
    // Implementation
    return true;
}
```

5. **Add session handling** in `session.cpp`:
```cpp
void Session::handle_command(const std::string& commandStr) {
    Command cmd = Parser::parse(commandStr);
    
    switch (cmd.type) {
        // ... existing cases ...
        case CommandType::NEWCOMMAND:
            if (cmd.args.size() >= 2) {
                if (storage_.newcommand(cmd.args[0], cmd.args[1])) {
                    response_ = "OK\r\n";
                } else {
                    response_ = "ERROR\r\n";
                }
            } else {
                response_ = "ERROR: NEWCOMMAND requires key and argument\r\n";
            }
            break;
    }
}
```

#### 2. Client-Side Implementation

1. **Add method to RedicraftClient.java**:
```java
public boolean newcommand(String key, String arg) {
    try {
        sendCommand("NEWCOMMAND " + key + " " + arg);
        String response = readResponse();
        return "OK".equals(response.trim());
    } catch (IOException e) {
        throw new RedicraftException("Failed to execute NEWCOMMAND", e);
    }
}
```

2. **Add async version**:
```java
public CompletableFuture<Boolean> newcommandAsync(String key, String arg) {
    return CompletableFuture.supplyAsync(() -> newcommand(key, arg));
}
```

3. **Add unit tests**:
```java
@Test
public void testNewcommand() {
    assertTrue(client.newcommand("test_key", "test_arg"));
}
```

### Adding New Data Types

#### 1. Server-Side Implementation

1. **Add data structure** in `storage.h`:
```cpp
struct SetItem {
    std::unordered_set<std::string> members;
    std::chrono::steady_clock::time_point expiry;
    bool has_expiry;
    
    SetItem() : has_expiry(false) {
        expiry = std::chrono::steady_clock::time_point();
    }
};

// Add to storage class
private:
    std::unordered_map<std::string, SetItem> set_data_;
```

2. **Add methods** in `storage.h`:
```cpp
class Storage {
public:
    // Set operations
    bool sadd(const std::string& key, const std::vector<std::string>& members);
    std::vector<std::string> smembers(const std::string& key);
    bool sismember(const std::string& key, const std::string& member);
};
```

3. **Implement methods** in `storage.cpp`:
```cpp
bool Storage::sadd(const std::string& key, const std::vector<std::string>& members) {
    std::unique_lock lock(mutex_);
    remove_expired(key);
    
    auto it = set_data_.find(key);
    if (it != set_data_.end()) {
        // Set exists, add members
        size_t added = 0;
        for (const auto& member : members) {
            if (it->second.members.insert(member).second) {
                added++;
            }
        }
        return added > 0;
    } else {
        // Create new set
        SetItem item;
        for (const auto& member : members) {
            item.members.insert(member);
        }
        set_data_[key] = item;
        return true;
    }
}
```

#### 2. Client-Side Implementation

1. **Add methods to RedicraftClient.java**:
```java
public boolean sadd(String key, String... members) {
    try {
        StringBuilder command = new StringBuilder("SADD " + key);
        for (String member : members) {
            command.append(" ").append(member);
        }
        sendCommand(command.toString());
        String response = readResponse();
        return !"0".equals(response.trim());
    } catch (IOException e) {
        throw new RedicraftException("Failed to execute SADD", e);
    }
}

public Set<String> smembers(String key) {
    try {
        sendCommand("SMEMBERS " + key);
        String response = readResponse();
        // Parse response into set
        return parseSetResponse(response);
    } catch (IOException e) {
        throw new RedicraftException("Failed to execute SMEMBERS", e);
    }
}
```

## Documentation

### Code Comments
- Write comments in Ukrainian
- Include author name "Андрій Будильников"
- Keep comments minimal and casual
- Focus on explaining why, not what

```cpp
// simple mutex for thread safety
// author: Андрій Будильников
std::shared_mutex mutex_;
```

### API Documentation
Document all public APIs with clear descriptions:

```java
/**
 * sets a key-value pair in the server.
 * @param key the key to set
 * @param value the value to associate with the key
 * @return true if successful, false otherwise
 * @throws RedicraftException if there's a connection error
 */
public boolean set(String key, String value) {
    // Implementation
}
```

### README Files
All README files must be written in three languages:
- Russian
- Ukrainian
- English

Example structure:
```markdown
# Project Name

## English Section
...

## Раздел на русском
...

## Розділ українською
...
```

## Contributing Process

### Getting Started

1. **Fork the repository** on GitHub
2. **Clone your fork** to your local machine
3. **Create a new branch** for your feature or bug fix
4. **Make your changes**
5. **Test your changes**
6. **Commit your changes** with clear commit messages
7. **Push to your fork**
8. **Submit a pull request**

### Branch Naming
Use descriptive branch names:
- `feature/new-command` - For new features
- `bugfix/connection-issue` - For bug fixes
- `docs/improve-readme` - For documentation improvements
- `refactor/storage-optimization` - For refactoring

### Commit Messages
Follow conventional commit format:
```
feat: add new SETEX command
^--^  ^----------------^
|     |
|     +-> Summary in present tense
|
+-------> Type: feat, fix, docs, style, refactor, test, chore
```

Examples:
- `feat: add support for sorted sets`
- `fix: resolve connection leak in client`
- `docs: update API reference documentation`
- `refactor: optimize storage data structures`

### Pull Request Process

1. **Create a pull request** from your branch to the main repository
2. **Provide a clear description** of your changes
3. **Reference any related issues** (e.g., "Fixes #123")
4. **Ensure all tests pass** before submitting
5. **Be responsive** to feedback during review
6. **Make requested changes** if needed
7. **Wait for approval** from maintainers

### Code Review Guidelines

#### For Contributors
- Write clean, readable code
- Follow the established code style
- Include tests for new functionality
- Update documentation when needed
- Be open to feedback and suggestions

#### For Reviewers
- Check for code correctness
- Ensure adherence to coding standards
- Verify tests are adequate
- Confirm documentation is updated
- Provide constructive feedback

### Release Process

#### Versioning
Follow semantic versioning (MAJOR.MINOR.PATCH):
- **MAJOR**: Breaking changes
- **MINOR**: New features
- **PATCH**: Bug fixes

#### Release Steps
1. **Update version numbers** in all relevant files
2. **Update CHANGELOG.md** with release notes
3. **Create a release tag** on GitHub
4. **Build and package** the release
5. **Publish** to appropriate channels
6. **Announce** the release

### Continuous Integration

#### Build Process
The project uses GitHub Actions for CI:
- **Build**: Compile both C++ and Java components
- **Test**: Run all unit and integration tests
- **Lint**: Check code style and quality
- **Deploy**: Package and release artifacts

#### Quality Gates
- All tests must pass
- Code must compile without warnings
- Code coverage must meet minimum thresholds
- Security scans must pass

### Best Practices

#### Code Quality
- Write self-documenting code
- Keep functions small and focused
- Use meaningful variable names
- Avoid code duplication
- Handle errors gracefully

#### Performance
- Profile code before optimizing
- Use appropriate data structures
- Minimize memory allocations
- Consider thread safety
- Optimize for common cases

#### Security
- Validate all inputs
- Sanitize user data
- Use secure coding practices
- Keep dependencies up to date
- Follow security best practices

#### Maintainability
- Write modular code
- Use clear interfaces
- Document complex logic
- Keep dependencies minimal
- Follow established patterns