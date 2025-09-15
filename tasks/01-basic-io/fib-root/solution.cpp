#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <unistd.h>
#include <vector>

static inline bool is_hex(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

static inline uint64_t hex_val(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    return 10 + (c - 'A');
}

static std::string to_hex_signed(long long v) {
    if (v == 0) {
        return "0";
    }
    bool neg = v < 0;
    unsigned long long u = neg ? -(unsigned long long)v : (unsigned long long)v;
    char buf[32];
    int i = 0;
    while (u) {
        int d = u & 15;
        buf[i++] = d < 10 ? ('0' + d) : ('a' + (d - 10));
        u >>= 4;
    }
    std::string s;
    s.reserve(i + (neg ? 1 : 0));
    if (neg) {
        s.push_back('-');
    }
    while (i--) {
        s.push_back(buf[i]);
    }
    return s;
}

int main() {
    std::vector<uint64_t> fibs;
    fibs.push_back(1);
    fibs.push_back(1);
    const int64_t LIM = std::numeric_limits<int64_t>::max();
    while (true) {
        uint64_t a = fibs[fibs.size() - 2], b = fibs.back();
        if (a > (uint64_t)LIM - b) {
            break;
        }
        fibs.push_back(a + b);
    }

    const size_t BUFSZ = 1 << 15;
    char buf[BUFSZ];
    uint64_t cur = 0;
    bool in = false;
    std::string out;

    ssize_t n;
    while ((n = read(STDIN_FILENO, buf, BUFSZ)) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            char c = buf[i];
            if (is_hex(c)) {
                in = true;
                cur = (cur << 4) | hex_val(c);
            } else {
                if (in) {
                    long long idx =
                        (long long)(std::upper_bound(fibs.begin(), fibs.end(),
                                                     cur) -
                                    fibs.begin()) -
                        1;
                    if (cur == 0) {
                        idx = -1;
                    }
                    out += to_hex_signed(idx);
                    out.push_back('\n');
                    cur = 0;
                    in = false;
                }
            }
        }
    }
    if (in) {
        long long idx =
            (long long)(std::upper_bound(fibs.begin(), fibs.end(), cur) -
                        fibs.begin()) -
            1;
        if (cur == 0) {
            idx = -1;
        }
        out += to_hex_signed(idx);
        out.push_back('\n');
    }
    if (!out.empty() && out.back() != '\n') {
        out.push_back('\n');
    }
    write(STDOUT_FILENO, out.data(), out.size());
    return 0;
}
