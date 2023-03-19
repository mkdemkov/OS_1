#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 5000

void reverse_string(char *str) {
int len = strlen(str);
for (int i = 0; i < len / 2; i++) {
char tmp = str[i];
str[i] = str[len - 1 - i];
str[len - 1 - i] = tmp;
}
}

int main(int argc, char *argv[]) {
int fd1, fd2, fd3;
char buffer[BUFFER_SIZE];

// Открытие входного файла
fd1 = open(argv[1], O_RDONLY);
if (fd1 == -1) {
perror("Не удалось открыть файл");
exit(EXIT_FAILURE);
}

// Создание именованного канала между первым и вторым процессами
if (mkfifo("pipe1", 0666) == -1) {
perror("Не удалось создать именованный канал");
exit(EXIT_FAILURE);
}

// Создание именованного канала между вторым и третьим процессами
if (mkfifo("pipe2", 0666) == -1) {
perror("Не удалось создать именованный канал");
exit(EXIT_FAILURE);
}

// Создание первого процесса
pid_t pid1 = fork();
if (pid1 == 0) {
// Первый процесс читает данные из входного файла и записывает их в первый именованный канал
fd1 = open(argv[1], O_RDONLY);
fd2 = open("pipe1", O_WRONLY);
while (read(fd1, buffer, BUFFER_SIZE)) {
write(fd2, buffer, BUFFER_SIZE);
}
close(fd1);
close(fd2);
exit(EXIT_SUCCESS);
} else if (pid1 == -1) {
perror("Не удалось создать процесс");
exit(EXIT_FAILURE);
}

// Создание второго процесса
pid_t pid2 = fork();
if (pid2 == 0) {
// Второй процесс считывает данные из первого именованного канала, обрабатывает их и записывает результат во второй именованный канал
fd2 = open("pipe1", O_RDONLY);
fd3 = open("pipe2", O_WRONLY);
while (read(fd2, buffer, BUFFER_SIZE)) {
// Обработка данных (например, переворот строки)
reverse_string(buffer);
write(fd3, buffer, BUFFER_SIZE);
}
close(fd2);
close(fd3);
exit(EXIT_SUCCESS);
} else if (pid2 == -1) {
perror("Не удалось создать процесс");
exit(EXIT_FAILURE);
}
// Создание третьего процесса
pid_t pid3 = fork();
if (pid3 == 0) {
// Третий процесс считывает данные из второго именованного канала и записывает их в выходной файл
fd3 = open("pipe2", O_RDONLY);
fd1 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
while (read(fd3, buffer, BUFFER_SIZE)) {
write(fd1, buffer, BUFFER_SIZE);
}
close(fd1);
close(fd3);
exit(EXIT_SUCCESS);
} else if (pid3 == -1) {
perror("Не удалось создать процесс");
exit(EXIT_FAILURE);
}

// Ожидание завершения дочерних процессов
wait(NULL);
wait(NULL);
wait(NULL);

// Удаление именованных каналов
unlink("pipe1");
unlink("pipe2");

return 0;
}