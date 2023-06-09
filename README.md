# IHW_4_OS

### Фролов Александр Сергеевич БПИ217

### Вариант 12

### Работа выполнялась на 8 баллов

Задача о гостинице – 1. 
В гостинице 30 номеров, гости гостиницы снимают номер на одни или несколько суток. Если в гостинице нет свободных номеров, гости не уходят, а устраиваются на рядом с гостиницей на скамейках и ждут, пока любой номеров не освободится. Создать приложение, моделирующее работу гостиницы. Сервер — это гостиница. Прибывающие гости могут порождаться отдельным клиентом. Другой клиент — это скамейки, образующие очередь ожидающих гостей.

### Задача на 4-5 баллов

##### Сервер (Server_hotel.c):

Сервер в данном коде представляет собой приложение гостиницы. Он прослушивает соединения от клиентов и обрабатывает их запросы на заселение и выселение. Вот описание работы сервера:

1) Создание UDP сокета:

Создается сокет с использованием функции socket() с параметрами AF_INET и SOCK_DGRAM.

2) Инициализация сервера:

• Структура server_addr инициализируется нулями и заполняется необходимыми параметрами, включая порт PORT и адрес INADDR_ANY.

3) Привязка сокета к адресу сервера:

• Сокет привязывается к адресу сервера с помощью функции bind(). Если привязка не удалась, сервер выводит сообщение об ошибке и завершает работу.

4) Ожидание клиентов:

• Сервер находится в бесконечном цикле while (1), ожидая запросов от клиентов.

5) Прием сообщения от клиента:

• Сервер использует функцию recvfrom() для приема сообщения от клиента. Прием происходит в буфер buffer.

6) Обработка запроса клиента:

• Сервер проверяет содержимое буфера buffer для определения типа запроса клиента.

7) Заселение клиента:

• Если запрос клиента соответствует "check-in" (заселение), сервер проверяет наличие свободных номеров (occupied_rooms < MAX_ROOMS).

• Если есть свободные номера, сервер отправляет подтверждение клиенту с помощью функции sendto(). Подтверждение содержит сообщение "OK".

• Количество занятых номеров occupied_rooms увеличивается, и сервер выводит сообщение о заселении клиента.

• Если нет свободных номеров, сервер отправляет отказ клиенту с помощью функции sendto(). Отказ содержит сообщение "No rooms available".

• Сервер выводит сообщение о отказе заселения клиента.

8) Выселение клиента:

• Если запрос клиента соответствует "check-out" (выселение), сервер освобождает номер.

• Количество занятых номеров occupied_rooms уменьшается, и сервер выводит сообщение об освобождении номера.

9) Зацикленность сервера:

• После обработки запроса сервер возвращается к шагу 5 и ожидает новых запросов от клиентов.

10) Завершение работы сервера:

• После выхода из бесконечного цикла сервер закрывает сокет и завершает работу.


##### Клиент (Client_guest.c):
