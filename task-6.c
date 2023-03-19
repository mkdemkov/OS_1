#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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
if (argc != 3) {
printf("Использование: %s [входной файл] [выходной файл]\n", argv[0]);
exit(1);
}
char* input_file = argv[1];
char* output_file = argv[2];

int fd[2];
pid_t pid;
char buffer[BUFFER_SIZE];

if (pipe(fd) == -1) { // Создание неименованного канала
perror("Ошибка при создании канала");
exit(1);
}

pid = fork(); // Создание дочернего процесса
if (pid < 0) {
perror("Ошибка при создании дочернего процесса");
exit(1);
} else if (pid == 0) { // Дочерний процесс
close(fd[0]); // Закрытие чтения канала
FILE* fp = fopen(input_file, "r");
if (fp == NULL) {
perror("Ошибка при открытии входного файла");
exit(1);
}
while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
reverse_string(buffer);
write(fd[1], buffer, strlen(buffer)+1); // Запись в канал
}
fclose(fp);
close(fd[1]); // Закрытие записи канала
exit(0);
} else { // Родительский процесс
close(fd[1]); // Закрытие записи канала
wait(NULL); // Ожидание завершения дочернего процесса
FILE* fp = fopen(output_file, "w");
if (fp == NULL) {
perror("Ошибка при открытии выходного файла");
exit(1);
}
while (read(fd[0], buffer, BUFFER_SIZE) > 0) { // Чтение из канала
fprintf(fp, "%s", buffer);
}
fclose(fp);
close(fd[0]); // Закрытие чтения канала
exit(0);
}
}