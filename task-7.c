#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUF_SIZE 5000 // размер буферов для хранения вводимых данных и результатов обработки

int main(int argc, char *argv[]) {

// Проверяем количество аргументов командной строки
if (argc != 3) {
fprintf(stderr, "Использование: %s <входной файл> <выходной файл>\n", argv[0]);
exit(EXIT_FAILURE);
}

// Создаем именованный канал для обмена данными между процессами
if (mkfifo("myfifo", 0666) == -1) {
perror("mkfifo");
exit(EXIT_FAILURE);
}

// Создаем первый дочерний процесс
pid_t pid1;
if ((pid1 = fork()) < 0) {
perror("fork");
exit(EXIT_FAILURE);
}
else if (pid1 == 0) {
// Код для первого процесса

// Открываем входной файл на чтение
int in_fd = open(argv[1], O_RDONLY);
if (in_fd == -1) {
perror("open");
exit(EXIT_FAILURE);
}

// Открываем именованный канал на запись
int fifo_fd = open("myfifo", O_WRONLY);
if (fifo_fd == -1) {
perror("open");
exit(EXIT_FAILURE);
}

// Читаем данные из входного файла и передаем их второму процессу через именованный канал
char buffer[BUF_SIZE];
int num_read;
while ((num_read = read(in_fd, buffer, BUF_SIZE)) > 0) {
if (write(fifo_fd, buffer, num_read) == -1) {
perror("write");
exit(EXIT_FAILURE);
}
}

// Закрываем файлы и каналы
close(in_fd);
close(fifo_fd);
exit(EXIT_SUCCESS);
}
else {
// Код для родительского процесса

// Создаем второй дочерний процесс
pid_t pid2;
if ((pid2 = fork()) < 0) {
perror("fork");
exit(EXIT_FAILURE);
}
else if (pid2 == 0) {
// Код для второго процесса

// Открываем именованный канал на чтение
int fifo_fd = open("myfifo", O_RDONLY);
if (fifo_fd == -1) {
perror("open");
exit(EXIT_FAILURE);
}

// Читаем данные из именованного канала, переворачиваем строку на месте и отправляем результат обратно в первый процесс
char buffer[BUF_SIZE];
int num_read;
while ((num_read = read(fifo_fd, buffer, BUF_SIZE)) > 0) {
for (int i = 0, j = num_read - 1; i < j; i++, j--) {
char temp = buffer[i];
buffer[i] = buffer[j];
buffer[j] = temp;
}

if (write(fifo_fd, buffer, num_read) == -1) {
perror("write");
exit(EXIT_FAILURE);
}
}

// Закрываем канал
close(fifo_fd);
exit(EXIT_SUCCESS);
}
else {
// Код для родительского процесса

// Открываем выходной файл на запись
int out_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
if (out_fd == -1) {
perror("open");
exit(EXIT_FAILURE);
}

// Открываем именованный канал на чтение
int fifo_fd = open("myfifo", O_RDONLY);
if (fifo_fd == -1) {
perror("open");
exit(EXIT_FAILURE);
}

// Читаем данные из именованного канала и записываем их в выходной файл
char buffer[BUF_SIZE];
int num_read;
while ((num_read = read(fifo_fd, buffer, BUF_SIZE)) > 0) {
if (write(out_fd, buffer, num_read) == -1) {
perror("write");
exit(EXIT_FAILURE);
}
}

// Закрываем файлы и каналы
close(out_fd);
close(fifo_fd);
unlink("myfifo");
exit(EXIT_SUCCESS);
}
}

return 0;
}