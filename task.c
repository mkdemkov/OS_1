#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 5000 // размер буфера

int main(int argc, char *argv[])
{
    int input_fd, output_fd, fd1[2], fd2[2], nbytes;
    pid_t pid1, pid2;
    char buffer[BUFFER_SIZE];

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(1);
    }

    // открыть входной файл для чтения и проверить ошибки
    if ((input_fd = open(argv[1], O_RDONLY)) == -1)
    {
        perror("open");
        exit(1);
    }

    // создать первый неименованный канал и проверить ошибки
    if (pipe(fd1) == -1)
    {
        perror("pipe");
        exit(1);
    }

    // создать второй неименованный канал и проверить ошибки
    if (pipe(fd2) == -1)
    {
        perror("pipe");
        exit(1);
    }

    // создать первый дочерний процесс и проверить ошибки
    if ((pid1 = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0)
    {                  // код для первого дочернего процесса
        close(fd1[0]); // закрыть ненужный конец первого канала (чтение)

        // читать данные из входного файла и записывать в первый канал
        ssize_t n;
        while ((n = read(input_fd, buffer, BUFFER_SIZE)) > 0)
        {
            if (write(fd1[1], buffer, n) == -1)
            {
                perror("write");
                exit(1);
            }
        }

        if (n == -1)
        {
            perror("read");
            exit(1);
        }

        close(fd1[1]);   // закрыть запись первого канала
        close(input_fd); // закрыть входной файл
        exit(0);         // завершить первый дочерний процесс
    }

    // создать второй дочерний процесс и проверить ошибки
    if ((pid2 = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0)
    {                  // код для второго дочернего процесса
        close(fd1[1]); // закрыть ненужный конец первого канала (запись)
        close(fd2[0]); // закрыть ненужный конец второго канала (чтение)

        // читать данные из первого канала и переворачивать буфер на месте
        nbytes = read(fd1[0], buffer, BUFFER_SIZE);
        if (nbytes <= 0)
        {
            perror("read");
            exit(1);
        }
        // перевернуть буфер на месте
        char *start = buffer;
        char *end = buffer + nbytes - 1;
        while (start < end)
        {
            char temp = *start;
            *start++ = *end;
            *end-- = temp;
        }
        // записать перевернутые данные во второй канал
        if (write(fd2[1], buffer, nbytes) == -1)
        {
            perror("write");
            exit(1);
        }

        close(fd1[0]); // закрыть чтение первого канала
        close(fd2[1]); // закрыть запись второго канала
        exit(0);       // завершить второй дочерний процесс
    }

    close(fd1[0]); // закрыть чтение первого канала в родительском процессе
    close(fd1[1]); // закрыть запись первого канала в родительском процессе

    close(input_fd); // закрыть входной файл в родительском процессе

    // открыть выходной файл для записи и проверить ошибки
    if ((output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1)
    {
        perror("open");
        exit(1);
    }

    // создать третий дочерний процесс и проверить ошибки
    if ((pid2 = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0)
    {                  // код для третьего дочернего процесса
        close(fd2[1]); // закрыть ненужный конец второго канала (запись)

        // читать данные из второго канала и записывать в выходной файл
        ssize_t n;
        while ((n = read(fd2[0], buffer, BUFFER_SIZE)) > 0)
        {
            if (write(output_fd, buffer, n) == -1)
            {
                perror("write");
                exit(1);
            }
        }

        if (n == -1)
        {
            perror("read");
            exit(1);
        }

        close(fd2[0]);    // закрыть чтение второго канала
        close(output_fd); // закрыть выходной файл
        exit(0);          // завершить третий дочерний процесс
    }

    close(fd2[0]);    // закрыть чтение второго канала в родительском процессе
    close(fd2[1]);    // закрыть запись второго канала в родительском процессе
    close(output_fd); // закрыть выходной файл в родительском процессе

    // ждать завершения всех дочерних процессов
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}