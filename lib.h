#ifndef INF_THEORY_LIB_H
#define INF_THEORY_LIB_H

#include <vector>

using namespace std;

static vector<uint8_t> ones{1, 2, 4, 8, 16, 32, 64, 128};


struct Node {
    size_t sum;
    Node *left;
    Node *right;
    uint8_t *id;

    bool has_two_kids() {
        return left != nullptr && right != nullptr;
    }

    void add_child(Node *child) {
        if (left == nullptr) {
            left = child;
        } else if (right == nullptr) {
            right = child;
        } else {
            throw runtime_error("sdc");
        }
    }
//1111111110011101100101001011100110010011100100110010010000111000000111011110111100100001110100111001100100100000111101000011101110100111010011010011001001111001001100100111001100100111001001100100101101000011010001101011110000011110111111110011001000011011100100000011110100001111010011001001110010000101000110000
    Node(uint8_t *id, size_t sum, Node *left, Node *right) : id(id), sum(sum), left(left), right(right) {}
};

struct code {
    size_t len{};
    vector<uint8_t> data;

    code(size_t len, const vector<uint8_t> &data) : len(len), data(data) {}

    code() = default;

    void add_bit(int bit) {
        if (len % 8 == 0) {
            data.push_back(0);
        }
        data[len / 8] ^= bit << len % 8;
        len++;
    }

    void remove_bit() {
        if (len % 8 == 1) {
            data.pop_back();
        }
        data[len / 8] &= 255 ^ (1 << len % 8);
        len--;
    }
};


struct bit_buffer {
    size_t pos = 0;
    size_t read_pos = 0;
    vector<uint8_t> data;

    void add_bits(const code &c) {
        for (size_t i = 0; i < c.len; ++i) {
            if (pos % 8 == 0) {
                data.push_back(0);
            }
            data[pos / 8] ^= (((c.data[i / 8] & ones[i % 8]) >> (i % 8)) << (pos % 8));
            pos++;
        }
    }

    int read_bit() {
        int res = (data[read_pos / 8] & (1 << (read_pos % 8))) == 0 ? 0 : 1;
        read_pos++;
        return res;
    }
};

void print_tree(Node *root) {
    if (root->left != nullptr && root->right != nullptr) {
        cout << 1;
        print_tree(root->left);
        print_tree(root->right);
    } else {
        cout << 0;
    }
}

#endif //INF_THEORY_LIB_H
