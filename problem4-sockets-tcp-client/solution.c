#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Программе передаются два аргумента:
//   argv[1] — IPv4-адрес сервера в десятичной записи (например, "127.0.0.1")
//   argv[2] — номер порта
//
// Программа должна:
//   1. Установить TCP-соединение с указанным сервером.
//   2. В цикле читать со stdin целые знаковые числа в текстовом формате.
//   3. Отправлять каждое число на сервер в бинарном виде (int32, Little Endian).
//   4. Получать от сервера int32 LE в ответ и выводить его в stdout в текстовом виде.
//   5. Если сервер закрыл соединение — завершиться с кодом возврата 0.

static void encode_le32(int32_t value, unsigned char buf[4]) {
    uint32_t u = (uint32_t)value;
    buf[0] = (unsigned char)(u & 0xFF);
    buf[1] = (unsigned char)((u >> 8) & 0xFF);
    buf[2] = (unsigned char)((u >> 16) & 0xFF);
    buf[3] = (unsigned char)((u >> 24) & 0xFF);
}

static int32_t decode_le32(const unsigned char buf[4]) {
    uint32_t u = (uint32_t)buf[0]
               | ((uint32_t)buf[1] << 8)
               | ((uint32_t)buf[2] << 16)
               | ((uint32_t)buf[3] << 24);
    return (int32_t)u;
}

static int send_all(int fd, const unsigned char *buf, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, buf + sent, len - sent, 0);
        if (n <= 0) {
            return -1;
        }
        sent += (size_t)n;
    }
    return 0;
}

static int recv_all(int fd, unsigned char *buf, size_t len) {
    size_t got = 0;
    while (got < len) {
        ssize_t n = recv(fd, buf + got, len - got, 0);
        if (n == 0) {
            return 1;
        }
        if (n < 0) {
            return -1;
        }
        got += (size_t)n;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ipv4_addr> <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    if (inet_aton(argv[1], &addr.sin_addr) == 0) {
        fprintf(stderr, "Invalid address: %s\n", argv[1]);
        close(sock);
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    long value;
    while (scanf("%ld", &value) == 1) {
        unsigned char out[4];
        encode_le32((int32_t)value, out);
        if (send_all(sock, out, 4) < 0) {
            break;
        }

        unsigned char in[4];
        int r = recv_all(sock, in, 4);
        if (r != 0) {
            break;
        }

        int32_t resp = decode_le32(in);
        printf("%d\n", resp);
        fflush(stdout);
    }

    close(sock);
    return 0;
}
