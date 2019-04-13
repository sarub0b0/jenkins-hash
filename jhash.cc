#include <stdio.h>
#include <stdint.h>

#include <list>

#define KEY_LENGTH 6

#define HASHSIZE (1 << 14)
#define HASHMASK (HASHSIZE - 1)

struct mac_addr {
    uint8_t addr[6];
};

struct mac_table {
    std::list<struct mac_addr> list;
};

inline bool operator==(const struct mac_addr &lhs, const struct mac_addr &rhs) {
    return lhs.addr[0] == rhs.addr[0] && lhs.addr[1] == rhs.addr[1] &&
           lhs.addr[2] == rhs.addr[2] && lhs.addr[3] == rhs.addr[3] &&
           lhs.addr[4] == rhs.addr[4] && lhs.addr[5] == rhs.addr[5];
}
inline bool operator!=(const struct mac_addr &lhs, const struct mac_addr &rhs) {
    return lhs.addr[0] != rhs.addr[0] || lhs.addr[1] != rhs.addr[1] ||
           lhs.addr[2] != rhs.addr[2] || lhs.addr[3] != rhs.addr[3] ||
           lhs.addr[4] != rhs.addr[4] || lhs.addr[5] != rhs.addr[5];
}

uint32_t jenkins_hash(const struct mac_addr &mac) {
    const uint8_t *key = mac.addr;
    uint32_t hash      = 0;

    for (int i = 0; i < KEY_LENGTH; i++) {
        hash += key[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash & HASHMASK;
}

static const struct mac_addr varid_addr[] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x03}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x04},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x05}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x06},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x07}, {0x00, 0x00, 0x00, 0x00, 0x00, 0x08},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x09}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x0a},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x0b}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x0c},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x0d}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x0e},
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x0f}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x00},
    {0x00, 0x00, 0x00, 0x02, 0x11, 0x01}, {0x00, 0x00, 0x00, 0x02, 0x11, 0x02},
    {0x00, 0x00, 0x00, 0x02, 0x11, 0x03}, {0x00, 0x00, 0x00, 0x02, 0x11, 0x04},
    {0x00, 0x00, 0x00, 0x02, 0x11, 0x05}, {0x00, 0x00, 0x00, 0x02, 0x11, 0x06},
    {0x00, 0x00, 0x00, 0x02, 0x11, 0x07}, {0x00, 0x00, 0x00, 0x02, 0x11, 0x08},
};

inline void print_macaddr(const struct mac_addr &mac) {
    printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
           mac.addr[0],
           mac.addr[1],
           mac.addr[2],
           mac.addr[3],
           mac.addr[4],
           mac.addr[5]);
}

int main(void) {

    struct mac_table mtable[HASHSIZE];

    printf("varid addr list\n");
    for (int i = 0; i < sizeof(varid_addr) / sizeof(struct mac_addr); i++) {
        printf("(%2d) ", i);
        print_macaddr(varid_addr[i]);
    }

    // learned mac addr
    for (auto &&mac : varid_addr) {
        uint32_t hash = jenkins_hash(mac);
        if (mtable[hash].list.empty()) {
            mtable[hash].list.push_back(mac);
        } else {
            bool hit = false;
            for (auto &&m : mtable[hash].list) {
                if (m == mac) {
                    hit = true;
                    break;
                };
            }
            if (!hit) mtable[hash].list.push_back(mac);
        }
    }

    // print mtable
    for (int i = 0; i < HASHSIZE; i++) {
        if (!mtable[i].list.empty()) {
            printf("hash=0x%04x\n", i);

            for (auto &&l : mtable[i].list) {
                printf("    ");
                print_macaddr(l);
            }
        }
    }

    return 0;
}
