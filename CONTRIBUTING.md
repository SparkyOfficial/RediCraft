# Contributing to RediCraft / Вклад в RediCraft

## English

Thank you for your interest in contributing to RediCraft! This document provides guidelines and information to help you contribute effectively.

### About RediCraft

RediCraft is a specialized distributed key-value store designed specifically for Minecraft plugin development. It's not a general-purpose database but rather a tailored solution for game server applications with the following unique characteristics:

1. **Minecraft-Centric Design**: Commands and data structures are optimized for common Minecraft use cases like player data, inventories, and leaderboards.
2. **Low Latency Requirements**: Game servers require extremely fast response times, often under 1ms for simple operations.
3. **Simple Protocol**: Uses a Redis-like text protocol that's easy to implement in any Minecraft plugin language.
4. **Distributed Architecture**: Supports master-slave replication and clustering for large-scale deployments.

### Code of Conduct

By participating in this project, you agree to abide by our Code of Conduct. Please treat all contributors and users with respect and professionalism.

### How to Contribute

#### Reporting Bugs

If you find a bug, please create an issue on GitHub with:
1. A clear and descriptive title
2. Steps to reproduce the bug
3. Expected behavior
4. Actual behavior
5. Environment information (OS, compiler version, etc.)

#### Suggesting Enhancements

To suggest a new feature or enhancement:
1. Check if there's already an issue or pull request for your idea
2. Create a new issue describing the enhancement
3. Explain why this enhancement would be useful for Minecraft plugin developers

#### Code Contributions

1. Fork the repository
2. Create a new branch for your feature or bug fix
3. Make your changes
4. Add tests if applicable
5. Ensure all tests pass
6. Submit a pull request

### Development Setup

#### Prerequisites

- CMake 3.10+
- C++17 compatible compiler
- JDK 8+
- Maven 3.6+

#### Building the Project

```bash
# Build the C++ server
cd RediCraft-Server
cmake -B build
cmake --build build

# Build the Java client
cd RediCraft-API
mvn clean package
```

### Coding Standards

#### C++ Code

- Follow the existing code style
- Use meaningful variable and function names
- Add comments in Ukrainian with author name "Андрій Будильников"
- Keep comments minimal and casual
- Write documentation in Russian, Ukrainian, and English

#### Java Code

- Follow standard Java naming conventions
- Use clear and descriptive names
- Add Javadoc comments for public APIs
- Follow the existing code patterns

### Special Considerations for RediCraft

#### Performance is Critical

RediCraft is designed for extremely low-latency operations. When contributing code:

1. Avoid unnecessary memory allocations in hot paths
2. Use efficient data structures (unordered_map for O(1) average access)
3. Minimize lock contention in multi-threaded operations
4. Profile performance impact of your changes

#### Minecraft Plugin Compatibility

RediCraft is used in Minecraft plugins, which means:

1. The Java client should work with older Java versions (8+)
2. Dependencies should be minimal to avoid conflicts
3. Error handling should be clear and informative for plugin developers
4. Connection management should be robust to handle network issues in game servers

#### Protocol Compatibility

RediCraft uses a Redis-like protocol but with some differences:

1. Commands are case-insensitive
2. Some Redis commands may not be implemented
3. Some custom commands may be added for Minecraft-specific use cases
4. Always test protocol changes with the Java client

### Testing

- Add unit tests for new functionality
- Ensure all existing tests pass before submitting
- Write clear, focused tests
- Test both success and failure cases

### Documentation

- Update documentation when making changes to APIs
- Write documentation in all three languages: English, Russian, and Ukrainian
- Keep documentation clear and concise
- Include examples for new features

### Pull Request Process

1. Ensure your code follows the coding standards
2. Add tests for new functionality
3. Update documentation as needed
4. Describe your changes in the pull request
5. Reference any related issues

### Questions?

If you have any questions about contributing, feel free to create an issue or contact the maintainers.

Thank you for contributing to RediCraft!

## Русский

Спасибо за ваш интерес к вкладу в RediCraft! Этот документ содержит рекомендации и информацию, которые помогут вам эффективно внести свой вклад.

### О RediCraft

RediCraft - это специализированное распределенное хранилище ключ-значение, разработанное специально для разработки плагинов Minecraft. Это не универсальная база данных, а адаптированное решение для игровых серверов со следующими уникальными характеристиками:

1. **Ориентация на Minecraft**: Команды и структуры данных оптимизированы для распространенных случаев использования Minecraft, таких как данные игроков, инвентари и таблицы лидеров.
2. **Требования к низкой задержке**: Игровые серверы требуют чрезвычайно быстрого времени отклика, часто менее 1 мс для простых операций.
3. **Простой протокол**: Использует текстовый протокол, подобный Redis, который легко реализовать на любом языке плагинов Minecraft.
4. **Распределенная архитектура**: Поддерживает репликацию master-slave и кластеризацию для крупномасштабных развертываний.

### Кодекс поведения

Участвуя в этом проекте, вы соглашаетесь соблюдать наш Кодекс поведения. Пожалуйста, относитесь ко всем участникам и пользователям с уважением и профессионализмом.

### Как внести вклад

#### Сообщения об ошибках

Если вы нашли ошибку, пожалуйста, создайте проблему на GitHub с:
1. Понятным и описательным заголовком
2. Шагами для воспроизведения ошибки
3. Ожидаемым поведением
4. Фактическим поведением
5. Информацией о среде (ОС, версия компилятора и т. д.)

#### Предложения по улучшению

Чтобы предложить новую функцию или улучшение:
1. Проверьте, нет ли уже проблемы или запроса на включение для вашей идеи
2. Создайте новую проблему с описанием улучшения
3. Объясните, почему это улучшение будет полезно для разработчиков плагинов Minecraft

#### Вклад в код

1. Форкните репозиторий
2. Создайте новую ветку для вашей функции или исправления ошибки
3. Внесите изменения
4. Добавьте тесты, если это применимо
5. Убедитесь, что все тесты проходят
6. Отправьте запрос на включение

### Настройка разработки

#### Необходимые условия

- CMake 3.10+
- Компилятор, совместимый с C++17
- JDK 8+
- Maven 3.6+

#### Сборка проекта

```bash
# Сборка C++ сервера
cd RediCraft-Server
cmake -B build
cmake --build build

# Сборка Java клиента
cd RediCraft-API
mvn clean package
```

### Стандарты кодирования

#### C++ код

- Следуйте существующему стилю кода
- Используйте осмысленные имена переменных и функций
- Добавляйте комментарии на украинском языке с указанием автора "Андрій Будильников"
- Делайте комментарии минимальными и неформальными
- Пишите документацию на русском, украинском и английском языках

#### Java код

- Следуйте стандартным соглашениям об именовании Java
- Используйте четкие и описательные имена
- Добавляйте комментарии Javadoc для публичных API
- Следуйте существующим шаблонам кода

### Особые соображения для RediCraft

#### Производительность критична

RediCraft разработан для операций с чрезвычайно низкой задержкой. При внесении вклада в код:

1. Избегайте ненужных выделений памяти в горячих путях
2. Используйте эффективные структуры данных (unordered_map для O(1) в среднем доступе)
3. Минимизируйте конкуренцию блокировок в многопоточных операциях
4. Профилируйте влияние производительности ваших изменений

#### Совместимость с плагинами Minecraft

RediCraft используется в плагинах Minecraft, что означает:

1. Java клиент должен работать со старыми версиями Java (8+)
2. Зависимости должны быть минимальными, чтобы избежать конфликтов
3. Обработка ошибок должна быть четкой и информативной для разработчиков плагинов
4. Управление соединениями должно быть надежным для обработки сетевых проблем на игровых серверах

#### Совместимость протокола

RediCraft использует протокол, подобный Redis, но с некоторыми отличиями:

1. Команды нечувствительны к регистру
2. Некоторые команды Redis могут не быть реализованы
3. Некоторые пользовательские команды могут быть добавлены для случаев использования, специфичных для Minecraft
4. Всегда тестируйте изменения протокола с Java клиентом

### Тестирование

- Добавляйте модульные тесты для новой функциональности
- Убедитесь, что все существующие тесты проходят перед отправкой
- Пишите четкие, сфокусированные тесты
- Тестируйте как успешные, так и неудачные случаи

### Документация

- Обновляйте документацию при внесении изменений в API
- Пишите документацию на всех трех языках: английском, русском и украинском
- Делайте документацию четкой и краткой
- Включайте примеры для новых функций

### Процесс запроса на включение

1. Убедитесь, что ваш код соответствует стандартам кодирования
2. Добавьте тесты для новой функциональности
3. При необходимости обновите документацию
4. Опишите свои изменения в запросе на включение
5. Ссылайтесь на соответствующие проблемы

### Вопросы?

Если у вас есть какие-либо вопросы о вкладе, не стесняйтесь создавать проблему или связаться с сопровождающими.

Спасибо за ваш вклад в RediCraft!