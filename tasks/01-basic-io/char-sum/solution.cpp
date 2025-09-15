#include <unistd.h>

int main() {
    constexpr size_t BUFSZ = 4096;
    char buf[BUFSZ];

    unsigned long long sum = 0;
    while (true) {
        ssize_t n = read(STDIN_FILENO, buf, BUFSZ);
        if (n <= 0) {
            break;
        }
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char ch = static_cast<unsigned char>(buf[i]);
            if (ch >= '0' && ch <= '9') {
                sum += static_cast<unsigned long long>(ch - '0');
            }
        }
    }

    char out[64];
    int len = 0;
    if (sum == 0) {
        out[len++] = '0';
    } else {
        char tmp[64];
        int t = 0;
        while (sum > 0) {
            tmp[t++] = static_cast<char>('0' + (sum % 10));
            sum /= 10;
        }
        while (t > 0) {
            out[len++] = tmp[--t];
        }
    }
    out[len++] = '\n';

    int written = 0;
    while (written < len) {
        ssize_t w = write(STDOUT_FILENO, out + written, len - written);
        if (w <= 0) {
            break;
        }
        written += static_cast<int>(w);
    }
    return 0;
}
