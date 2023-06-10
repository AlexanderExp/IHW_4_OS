#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[256];

    // Создание UDP сокета
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Ошибка при создании сокета");
        exit(1);
    }

    // Инициализация сервера
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_aton(SERVER_IP, &server_addr.sin_addr) == 0) {
        perror("Ошибка при преобразовании IP-адреса");
        exit(1);
    }

    while (1) {
        // Отправка запроса на получение информации о количестве мест
        strcpy(buffer, "get-occupancy");

        // Отправка запроса на сервер
        if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            perror("Ошибка при отправке запроса на сервер");
            exit(1);
        }

        // Получение ответа от сервера
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL) < 0) {
            perror("Ошибка при получении ответа от сервера");
            exit(1);
        }

        // Вывод полученной информации о количестве мест
        printf("Количество свободных мест в гостинице: %s\n", buffer);

        sleep(1);// Задержка в 1 секунду
    }

    close(sockfd);

    return 0;
}
