# Search Engine Project

Проект поискового движка для обработки текстовых документов.

## Технологии
- C++17
- nlohmann/json для работы с JSON
- Google Test для модульного тестирования
- CMake для сборки

## Запуск проекта

1. Клонируйте репозиторий:
```bash
git clone <repo-url>
cd search_engine
```

2. Соберите проект:
```bash
mkdir build && cd build
cmake .. && cmake --build .
```

3. Запустите приложение:
```bash
./bin/search_engine
```

4. Для запуска тестов:
```bash
cd build
ctest
```

## Структура проекта
- `config.json` - конфигурация движка
- `requests.json` - поисковые запросы
- `answers.json` - результаты поиска
- `resources/` - текстовые документы для поиска
