#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8888
#define MAX_ROOMS 30

int occupied_rooms = 0;
int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buffer[256];

    // Создание UDP сокета
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при создании сокета");
        exit(1);
    }

    // Инициализация сервера
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Привязка сокета к адресу сервера
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при привязке сокета к адресу сервера");
        exit(1);
    }

    printf("Сервер гостиницы запущен. Ожидание гостей...\n");

    while (1) {
        client_len = sizeof(client_addr);

        // Прием сообщения от клиента
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len) < 0) {
            perror("Ошибка при приеме сообщения от клиента");
            exit(1);
        }

        printf("Получено сообщение от клиента: %s\n", buffer);

        // Обработка запроса клиента
        if (strncmp(buffer, "check-in", 8) == 0) {
            // Проверка наличия свободных номеров
            if (occupied_rooms < MAX_ROOMS) {
                // Номер есть - отправка подтверждения клиенту
                strcpy(buffer, "OK");
                if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, client_len) < 0) {
                    perror("Ошибка при отправке подтверждения клиенту");
                    exit(1);
                }

                occupied_rooms++;
                printf("Клиенту отправлено подтверждение о заселении\n");
            } else {
                // Нет свободных номеров - отправка отказа клиенту
                strcpy(buffer, "No rooms available");
                if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, client_len) < 0) {
                    perror("Ошибка при отправке отказа клиенту");
                    exit(1);
                }

                printf("Клиенту отправлен отказ о заселении\n");
            }
        } else if (strncmp(buffer, "check-out", 9) == 0) {
            // Освобождение номера
            occupied_rooms--;
            printf("Номер освобожден\n");
        }
    }

    close(sockfd);

    return 0;
}
