# Var6 — Система шифрования и стеганографии

Курсовая работа по дисциплине «Технологии и методы программирования» (ТаМП).

**Студент:** Ахроров Шерзодбек Туймурод угли
**Группа:** 251-354
**Вариант:** 6

---

## Вариант задания

| Параметр | Значение |
|---|---|
| Шифрование | RSA |
| Хеширование | SHA-1 |
| Численный метод | Метод Ньютона |
| Стеганография | Внедрение сообщения в музыкальный файл (WAV) |

---

## Стек технологий

- **Язык:** C++17
- **Фреймворк:** Qt 6
- **Сеть:** QTcpSocket (клиент-сервер, JSON-протокол)
- **БД:** SQLite
- **Тесты:** Qt Test
- **Документация:** Doxygen

---

## Архитектура

Система построена по клиент-серверной архитектуре:

- **Сервер** (`Var6Server`) — консольное приложение, слушает порт 9000, обрабатывает JSON-запросы, хранит пользователей в SQLite, реализует все алгоритмы варианта
- **Клиент** (`Var6Client`) — GUI-приложение на Qt Widgets, подключается к серверу через TCP, предоставляет интерфейс для всех функций

Подробное описание архитектуры, диаграммы классов и Use-Case — в [Wiki](../../wiki).

---

## Функционал

| Модуль | Описание |
|---|---|
| **RSA** | Генерация ключей, шифрование и дешифрование текста |
| **SHA-1** | Вычисление хеша строки, верификация |
| **Метод Ньютона** | Нахождение корня уравнения с историей итераций |
| **WAV-стеганография** | Внедрение и извлечение текстового сообщения из аудиофайла методом LSB |
| **Авторизация** | Регистрация и вход (пароли хранятся как SHA-1 хеш) |
| **Роли** | user / admin — администратор управляет пользователями |

---

## Структура репозитория

```
Sherzod6Var/
├── README.md
├── Doxyfile                  — конфигурация Doxygen
├── Specification.docx        — спецификация требований (SRS)
├── TestStrategy.xlsx         — стратегия тестирования, тест-план, чек-лист
├── TestCases_Defects.xlsx    — test cases и журнал дефектов
├── docs/                     — сгенерированная Doxygen-документация (html)
├── Var6Server/               — серверная часть (Qt Console)
│   ├── Var6Server.pro
│   ├── Dockerfile
│   └── src/
├── Var6Client/                — клиентская часть (Qt Widgets GUI)
│   ├── Var6Client.pro
│   └── src/
└── Var6Tests/                 — Unit-тесты (Qt Test)
    ├── Var6Tests.pro
    └── src/
```

---

## Структура веток Git

| Ветка | Назначение |
|---|---|
| `main` | Стабильная рабочая версия |
| `dev` | Основная ветка разработки |
| `feature/*` | Временные ветки под конкретные задачи |
| `docs` | Документация, диаграммы, спецификация |

---

## Сборка и запуск

### Зависимости

- Qt 6 (Core, Network, Sql, Widgets, Test)
- Компилятор C++17 (GCC/MinGW)

### Сервер

```bash
cd Var6Server
mkdir build && cd build
qmake6 ..          # или qmake на Windows
make -j$(nproc)
./Var6Server
```

При первом запуске автоматически создаётся администратор:
`login: admin`, `password: admin123`

### Клиент

Открыть `Var6Client/Var6Client.pro` в Qt Creator → Run.

### Docker (сервер)

```bash
cd Var6Server
docker build -t var6server .
docker run -p 9000:9000 var6server
```

### Unit-тесты

```bash
cd Var6Tests
mkdir build && cd build
qmake6 ..
make -j$(nproc)
./Var6Tests
```

### Doxygen-документация

```bash
doxygen Doxyfile
# результат в docs/html/index.html
```

---

## Протокол взаимодействия

Каждое сообщение — строка JSON, завершённая `\n`.

**Запрос:**
```json
{ "action": "<действие>", "<параметр>": "<значение>" }
```

**Ответ:**
```json
{ "status": "ok"|"error", "message": "<текст>" }
```

Полный список действий и параметров — в [Specification.docx](Specification.docx).

---

## Диаграммы

![UML Server](https://raw.githubusercontent.com/usherzodbek86-lang/Sherzod6Var/docs/UML_Server.jpg)
![UML Client](https://raw.githubusercontent.com/usherzodbek86-lang/Sherzod6Var/docs/UML_Client.jpg)
![UseCase](https://raw.githubusercontent.com/usherzodbek86-lang/Sherzod6Var/docs/UseCase.jpg)
