# Демков Михаил Кириллович. БПИ212. Вариант 9
## Описание задания
**Разработать программу, которая «переворачивает на месте» заданную ASCII-строку символов (не копируя строку в другой буфер).**
## 4 балла
### Общая схема решаемой задачи:

Программа использует три процесса, которые связаны между собой через неименованные каналы.

**Первый процесс (process1) читает данные из входного файла** с помощью системного вызова read() и передает их через неименованный канал второму процессу (process2).

**Второй процесс (process2) переворачивает прочитанные данные на месте**, не копируя их в другой буфер. Затем он передает измененные данные через неименованный канал третьему процессу (process3).

**Третий процесс (process3) записывает полученные данные в выходной файл** с помощью системного вызова write().

Схематически связи между процессами и файлами выглядят следующим образом:

![Не удалось открыть скрин. Нужный скрин находится в source/schema-4.jpg](https://github.com/mkdemkov/OS_1/blob/main/source/schema-4.JPG)

где **stdin** и **stdout** - стандартный ввод и вывод, соответственно, **pipe1** и **pipe2** - неименованные каналы, **read()** и **write()** - системные вызовы чтения и записи данных.

### Тестирование программы
Файлы с входными/выходными данными находятся в папке tests/mark_4. 

Компиляция обычным **gcc task-4.c -o ex4.out**. Запуск исполняемого файла **./ex4.out tests/mark_4/input.txt tests/mark_4/output.txt**

Было проведено 5 тестов. Результаты:
* **Тест1**
    * _Ввод_ -> Operating systems course is not that easy
    * _Вывод_ -> ysae taht ton si esruoc smetsys gnitarepO
* **Тест2**
    * _Ввод_ -> Lorem ipsum ipsum Lorem
    * _Вывод_ -> meroL muspi muspi meroL
* **Тест3**
    * _Ввод_ -> abba aabbaa aba aabbaa abba
    * _Вывод_ -> abba aabbaa aba aabbaa abba
* **Тест4**
    * _Ввод_ -> I love studying
    * _Вывод_ -> gniyduts evol I
* **Тест5**
    * _Ввод_ -> dawd awd awuidaw do awd awdaw aw dawddawdawd
    * _Вывод_ -> dwadwaddwad wa wadwa dwa od wadiuwa dwa dwad