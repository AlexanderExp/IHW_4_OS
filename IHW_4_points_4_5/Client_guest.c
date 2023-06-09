#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define NUM_CLIENTS 1000
#define MAX_OCCUPIED_ROOMS 30
#define CHECKOUT_DELAY_SECONDS 1

pthread_mutex_t mutex;         // Мьютекс для синхронизации доступа к общему количеству занятых номеров
pthread_cond_t cond;           // Условная переменная для ожидания освобождения номера
int occupied_rooms = 0;        // Количество занятых номеров
int clients_in_hotel = 0;      // Количество клиентов, заселившихся в гостиницу

void *client_thread(void *arg) {
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
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_aton(SERVER_IP, &server_addr.sin_addr) == 0) {
        perror("Ошибка при преобразовании IP-адреса");
        exit(1);
    }

    // Отправка запроса на заселение
    strcpy(buffer, "check-in");

    // Попытка заселиться
    while (1) {
        // Проверка, доступно ли место в гостинице
        pthread_mutex_lock(&mutex);
        if (occupied_rooms < MAX_OCCUPIED_ROOMS) {
            occupied_rooms++;
            clients_in_hotel++;
            pthread_mutex_unlock(&mutex);

            if (sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Ошибка при отправке запроса на заселение");
                exit(1);
            }

            printf("Клиент заселен в номер\n");
            srand(time(NULL));
            sleep(rand() % 5 + 1);  // Задержка для выселения

            pthread_mutex_lock(&mutex);
            occupied_rooms--;
            clients_in_hotel--;
            pthread_cond_signal(&cond);  // Сигнал для клиентов в очереди
            pthread_mutex_unlock(&mutex);

            break;
        } else {
            pthread_mutex_unlock(&mutex);

            // Нет свободных номеров - встаем в очередь
            printf("Нет свободных номеров. Клиент встает в очередь\n");

            // Ожидание освобождения номера
            pthread_mutex_lock(&mutex);
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }

    close(sockfd);

    return NULL;
}

int main() {
    pthread_t threads[NUM_CLIENTS];
    int i;

    // Инициализация мьютекса и условной переменной
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Создание 1000 потоков-клиентов
    for (i = 0; i < NUM_CLIENTS; i++) {
        if (pthread_create(&threads[i], NULL, client_thread, NULL) != 0) {
            perror("Ошибка при создании потока");
            exit(1);
        }
    }

    // Ожидание завершения всех потоков
    for (i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Уничтожение мьютекса и условной переменной
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("Количество клиентов, заселившихся в гостиницу: %d\n", NUM_CLIENTS);

    return 0;
}
