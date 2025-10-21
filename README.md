# RediCraft

## English

External in-memory key-value server for Minecraft plugins.

### 🎉 Current Status: Phase 3 Complete!

All components have been successfully implemented with full distributed system capabilities:

1. **RediCraft-Server** - C++ high-performance key-value store ✅
2. **RediCraft-API** - Java/Kotlin client library for Minecraft plugins ✅

### Project Structure

- `RediCraft-Server/` - Core C++ server implementation
- `RediCraft-API/` - Java client library
- `Guide/` - Comprehensive documentation (see below)
- `STATUS.md` - Current implementation status

### Development Roadmap

#### ✅ Phase 1: Single Node MVP (Complete)
- ✅ Basic TCP server in C++
- ✅ Simple protocol implementation (PING, SET, GET)
- ✅ Thread-safe storage
- ✅ Basic Java client with socket connections

#### ✅ Phase 2: Extended Functionality (Complete)
- ✅ Additional commands (INCR, DECR, INCRBY)
- ✅ Additional commands (HSET, HGET, HGETALL)
- ✅ Additional commands (LPUSH, RPOP, LRANGE)
- ✅ Additional commands (EXPIRE, TTL)
- ✅ Asynchronous Java client methods
- ✅ Connection pooling

#### ✅ Phase 3: Distributed System (Complete)
- ✅ Master-slave replication ✅
- ✅ Sharding/clustering support ✅
- ✅ Data persistence ✅

### Comprehensive Documentation

Detailed documentation is available in the [Guide](Guide/) folder:

- [README.md](Guide/README.md) - Main documentation
- [ARCHITECTURE.md](Guide/ARCHITECTURE.md) - Detailed architecture documentation
- [API_REFERENCE.md](Guide/API_REFERENCE.md) - Complete API reference
- [USAGE.md](Guide/USAGE.md) - Usage guide with Minecraft plugin examples
- [DEVELOPER.md](Guide/DEVELOPER.md) - Developer guide for contributing
- [TROUBLESHOOTING.md](Guide/TROUBLESHOOTING.md) - Troubleshooting guide
- [CHANGELOG.md](Guide/CHANGELOG.md) - Project changelog
- [BENCHMARKING.md](Guide/BENCHMARKING.md) - Performance benchmarking guide
- [SECURITY.md](Guide/SECURITY.md) - Security guide

### Quick Start

1. Build the server:
   ```bash
   cd RediCraft-Server
   cmake -B build
   cmake --build build
   ```

2. Build the client:
   ```bash
   cd RediCraft-API
   mvn clean package
   ```

3. Run the server:
   ```bash
   # Windows
   build\Debug\RediCraftServer.exe
   
   # Unix-like systems
   ./build/RediCraftServer
   ```

4. Use the client in your Minecraft plugin:
   ```java
   // Synchronous usage
   RedicraftClient client = new RedicraftClient();
   client.connect("localhost", 7379);
   client.set("player:Sparky:money", "1000");
   String money = client.get("player:Sparky:money");
   long kills = client.incr("player:Sparky:kills");
   
   // Hash operations
   client.hset("player:Sparky:info", "name", "Andriy");
   client.hset("player:Sparky:info", "level", "15");
   String name = client.hget("player:Sparky:info", "name");
   
   // List operations
   client.lpush("recent_players", "Sparky", "John", "Jane");
   String lastPlayer = client.rpop("recent_players");
   
   // Expiration
   client.set("temp_key", "temp_value");
   client.expire("temp_key", 60); // Expires in 60 seconds
   long ttl = client.ttl("temp_key"); // Get time to live
   
   // Asynchronous usage
   CompletableFuture<String> futureMoney = client.getAsync("player:Sparky:money");
   futureMoney.thenAccept(money -> System.out.println("Money: " + money));
   
   // Connection pooling
   ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);
   RedicraftClient pooledClient = pool.getConnection();
   // ... use client ...
   pool.returnConnection(pooledClient);
   ```

### Running the Demo

To run a complete demonstration of RediCraft's functionality:

**Windows PowerShell:**
```powershell
.\demo.ps1
```

**Windows Command Prompt:**
```cmd
demo.bat
```

The demo will:
1. Start the RediCraft server
2. Run synchronous client operations
3. Run asynchronous client operations
4. Execute performance benchmarks
5. Display results

### GitHub Actions

This project uses GitHub Actions for continuous integration and delivery:

- **Build and Test**: Automatically builds and tests the C++ server and Java client on every push and pull request
- **Code Quality**: Runs code linting and security scans
- **Release**: Creates GitHub releases when tags are pushed

## Русский

Внешний in-memory key-value сервер для плагинов Minecraft.

### 🎉 Текущий статус: Фаза 3 завершена!

Все компоненты были успешно реализованы с полноценными возможностями распределенной системы:

1. **RediCraft-Server** - высокопроизводительное хранилище на C++ ✅
2. **RediCraft-API** - клиентская библиотека на Java/Kotlin для плагинов Minecraft ✅

### Структура проекта

- `RediCraft-Server/` - основная реализация сервера на C++
- `RediCraft-API/` - клиентская библиотека на Java
- `Guide/` - Полная документация (см. ниже)
- `STATUS.md` - текущий статус реализации

### Дорожная карта разработки

#### ✅ Фаза 1: MVP с одним узлом (Завершена)
- ✅ Базовый TCP-сервер на C++
- ✅ Простая реализация протокола (PING, SET, GET)
- ✅ Потокобезопасное хранилище
- ✅ Базовый Java-клиент с соединениями через сокеты

#### ✅ Фаза 2: Расширенный функционал (Завершена)
- ✅ Дополнительные команды (INCR, DECR, INCRBY)
- ✅ Дополнительные команды (HSET, HGET, HGETALL)
- ✅ Дополнительные команды (LPUSH, RPOP, LRANGE)
- ✅ Дополнительные команды (EXPIRE, TTL)
- ✅ Асинхронные методы в Java-клиенте
- ✅ Пул соединений

#### ✅ Фаза 3: Распределенная система (Завершена)
- ✅ Репликация мастер-слейв ✅
- ✅ Поддержка шардинга/кластеризации ✅
- ✅ Сохранение данных на диск ✅

### Полная документация

Подробная документация доступна в папке [Guide](Guide/):

- [README.md](Guide/README.md) - Основная документация
- [ARCHITECTURE.md](Guide/ARCHITECTURE.md) - Подробная архитектурная документация
- [API_REFERENCE.md](Guide/API_REFERENCE.md) - Полный справочник API
- [USAGE.md](Guide/USAGE.md) - Руководство по использованию с примерами для Minecraft плагинов
- [DEVELOPER.md](Guide/DEVELOPER.md) - Руководство разработчика для участия в проекте
- [TROUBLESHOOTING.md](Guide/TROUBLESHOOTING.md) - Руководство по устранению неполадок
- [CHANGELOG.md](Guide/CHANGELOG.md) - Журнал изменений проекта
- [BENCHMARKING.md](Guide/BENCHMARKING.md) - Руководство по тестированию производительности
- [SECURITY.md](Guide/SECURITY.md) - Руководство по безопасности

### Быстрый старт

1. Сборка сервера:
   ```bash
   cd RediCraft-Server
   cmake -B build
   cmake --build build
   ```

2. Сборка клиента:
   ```bash
   cd RediCraft-API
   mvn clean package
   ```

3. Запуск сервера:
   ```bash
   # Windows
   build\Debug\RediCraftServer.exe
   
   # Unix-like системы
   ./build/RediCraftServer
   ```

4. Использование клиента в вашем Minecraft плагине:
   ```java
   // Синхронное использование
   RedicraftClient client = new RedicraftClient();
   client.connect("localhost", 7379);
   client.set("player:Sparky:money", "1000");
   String money = client.get("player:Sparky:money");
   long kills = client.incr("player:Sparky:kills");
   
   // Операции с хешами
   client.hset("player:Sparky:info", "name", "Andriy");
   client.hset("player:Sparky:info", "level", "15");
   String name = client.hget("player:Sparky:info", "name");
   
   // Операции со списками
   client.lpush("recent_players", "Sparky", "John", "Jane");
   String lastPlayer = client.rpop("recent_players");
   
   // Установка срока действия
   client.set("temp_key", "temp_value");
   client.expire("temp_key", 60); // Истекает через 60 секунд
   long ttl = client.ttl("temp_key"); // Получить время до истечения
   
   // Асинхронное использование
   CompletableFuture<String> futureMoney = client.getAsync("player:Sparky:money");
   futureMoney.thenAccept(money -> System.out.println("Деньги: " + money));
   
   // Пул соединений
   ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);
   RedicraftClient pooledClient = pool.getConnection();
   // ... использование клиента ...
   pool.returnConnection(pooledClient);
   ```

### Запуск демонстрации

Для запуска полной демонстрации функциональности RediCraft:

**Windows PowerShell:**
```powershell
.\demo.ps1
```

**Windows Command Prompt:**
```cmd
demo.bat
```

Демонстрация выполнит:
1. Запуск сервера RediCraft
2. Выполнение синхронных операций клиента
3. Выполнение асинхронных операций клиента
4. Запуск тестов производительности
5. Отображение результатов

### GitHub Actions

Этот проект использует GitHub Actions для непрерывной интеграции и доставки:

- **Build and Test**: Автоматически собирает и тестирует C++ сервер и Java клиент при каждом push и pull request
- **Code Quality**: Выполняет проверку кода и сканирование на уязвимости
- **Release**: Создает релизы GitHub при отправке тегов

## Українська

Зовнішній in-memory key-value сервер для плагінів Minecraft.

### 🎉 Поточний статус: Фаза 3 завершена!

Всі компоненти були успішно реалізовані з повноцінними можливостями розподіленої системи:

1. **RediCraft-Server** - високопродуктивне сховище на C++ ✅
2. **RediCraft-API** - клієнтська бібліотека на Java/Kotlin для плагінів Minecraft ✅

### Структура проекту

- `RediCraft-Server/` - основна реалізація сервера на C++
- `RediCraft-API/` - клієнтська бібліотека на Java
- `Guide/` - Повна документація (див. нижче)
- `STATUS.md` - поточний статус реалізації

### Дорожня карта розробки

#### ✅ Фаза 1: MVP з одним вузлом (Завершена)
- ✅ Базовий TCP-сервер на C++
- ✅ Проста реалізація протоколу (PING, SET, GET)
- ✅ Потокобезпечне сховище
- ✅ Базовий Java-клієнт з з'єднаннями через сокети

#### ✅ Фаза 2: Розширений функціонал (Завершена)
- ✅ Додаткові команди (INCR, DECR, INCRBY)
- ✅ Додаткові команди (HSET, HGET, HGETALL)
- ✅ Додаткові команди (LPUSH, RPOP, LRANGE)
- ✅ Додаткові команди (EXPIRE, TTL)
- ✅ Асинхронні методи в Java-клієнті
- ✅ Пул з'єднань

#### ✅ Фаза 3: Розподілена система (Завершена)
- ✅ Реплікація мастер-слейв ✅
- ✅ Підтримка шардингу/кластеризації ✅
- ✅ Збереження даних на диск ✅

### Повна документація

Детальна документація доступна в папці [Guide](Guide/):

- [README.md](Guide/README.md) - Основна документація
- [ARCHITECTURE.md](Guide/ARCHITECTURE.md) - Детальна архітектурна документація
- [API_REFERENCE.md](Guide/API_REFERENCE.md) - Повний довідник API
- [USAGE.md](Guide/USAGE.md) - Посібник з використання з прикладами для Minecraft плагінів
- [DEVELOPER.md](Guide/DEVELOPER.md) - Посібник розробника для участі в проекті
- [TROUBLESHOOTING.md](Guide/TROUBLESHOOTING.md) - Посібник з усунення несправностей
- [CHANGELOG.md](Guide/CHANGELOG.md) - Журнал змін проекту
- [BENCHMARKING.md](Guide/BENCHMARKING.md) - Посібник з тестування продуктивності
- [SECURITY.md](Guide/SECURITY.md) - Посібник з безпеки

### Швидкий старт

1. Збірка сервера:
   ```bash
   cd RediCraft-Server
   cmake -B build
   cmake --build build
   ```

2. Збірка клієнта:
   ```bash
   cd RediCraft-API
   mvn clean package
   ```

3. Запуск сервера:
   ```bash
   # Windows
   build\Debug\RediCraftServer.exe
   
   # Unix-like системи
   ./build/RediCraftServer
   ```

4. Використання клієнта у вашому Minecraft плагіні:
   ```java
   // Синхронне використання
   RedicraftClient client = new RedicraftClient();
   client.connect("localhost", 7379);
   client.set("player:Sparky:money", "1000");
   String money = client.get("player:Sparky:money");
   long kills = client.incr("player:Sparky:kills");
   
   // Операції з хешами
   client.hset("player:Sparky:info", "name", "Andriy");
   client.hset("player:Sparky:info", "level", "15");
   String name = client.hget("player:Sparky:info", "name");
   
   // Операції зі списками
   client.lpush("recent_players", "Sparky", "John", "Jane");
   String lastPlayer = client.rpop("recent_players");
   
   // Встановлення терміну дії
   client.set("temp_key", "temp_value");
   client.expire("temp_key", 60); // Термін дії 60 секунд
   long ttl = client.ttl("temp_key"); // Отримати час до закінчення терміну дії
   
   // Асинхронне використання
   CompletableFuture<String> futureMoney = client.getAsync("player:Sparky:money");
   futureMoney.thenAccept(money -> System.out.println("Гроші: " + money));
   
   // Пул з'єднань
   ConnectionPool pool = new ConnectionPool("localhost", 7379, 10);
   RedicraftClient pooledClient = pool.getConnection();
   // ... використання клієнта ...
   pool.returnConnection(pooledClient);
   ```

### Запуск демонстрації

Для запуску повної демонстрації функціональності RediCraft:

**Windows PowerShell:**
```powershell
.\demo.ps1
```

**Windows Command Prompt:**
```cmd
demo.bat
```

Демонстрація виконає:
1. Запуск сервера RediCraft
2. Виконання синхронних операцій клієнта
3. Виконання асинхронних операцій клієнта
4. Запуск тестів продуктивності
5. Відображення результатів

### GitHub Actions

Цей проект використовує GitHub Actions для безперервної інтеграції та доставки:

- **Build and Test**: Автоматично збирає та тестує C++ сервер і Java клієнт при кожному push та pull request
- **Code Quality**: Виконує перевірку коду та сканування на вразливості
- **Release**: Створює релізи GitHub при надсиланні тегів