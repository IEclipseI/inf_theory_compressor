#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <fstream>
#include <algorithm>
#include "lib.h"


using namespace std;

const char *readFile(const string &filename) {
    return new char;
}

string convertToString(const vector<uint8_t> &data) {
    string a;
    for (unsigned char i: data) {
        a.push_back((char) i);
    }
    return a;
}

string convertNumbersToString(const vector<uint8_t> &data) {
    string a;
    for (unsigned char i: data) {
        a.append(to_string(i)).append(" ");
    }
    return a;
}

vector<uint8_t> convertToUintVector(const string &data) {
    vector<uint8_t> a;
    for (unsigned char i: data) {
        a.push_back((char) i);
    }
    return a;
}

void print_shifts(const vector<uint8_t> &data, size_t len, vector<size_t> &shifts) {
    cout << "SHIFTS\n";
    for (const auto &shift: shifts) {
        string a;
        for (int i = 0; i < len; ++i) {
            a.push_back((char) data[(i + shift) % len]);
        }
        cout << a << endl;
    }
}

pair<const vector<uint8_t>, size_t> bwt(const vector<uint8_t> &data) {
    size_t len = data.size();

    vector<size_t> indexes = vector<size_t>(len);
    for (int i = 0; i < len; ++i) {
        indexes[i] = i;
    }

    vector<uint8_t> data_cpy(data);
    for (int i = 0; i < len; ++i) {
        data_cpy.push_back(data[i]);
    }

    sort(indexes.begin(), indexes.end(), [&data_cpy, len](size_t a, size_t b) {
        for (int i = 0; i < len; ++i) {
            if (data_cpy[a + i] != data_cpy[b + i]) {
                return data_cpy[a + i] < data_cpy[b + i];
            }
        }
        return false;
    });

//    print_shifts(data, len, indexes);
//    for (int i = 0; i < len; ++i) {
//        cout << indexes[i] << " ";
//    }
//    cout << "\n";

    vector<uint8_t> result(len);
    size_t zero_pos = 0;
    for (int i = 0; i < len; ++i) {
        result[i] = data[(indexes[i] + len - 1) % len];
        if (indexes[i] == 0) {
            zero_pos = i;
        }
    }
    return {result, zero_pos};
}


vector<uint8_t> mft(const vector<uint8_t> &input, const vector<uint8_t> &sorted_alph) {
    vector<uint8_t> current_indexes(sorted_alph.size());
    unordered_map<uint8_t, size_t> pos;

    for (size_t i = 0; i < sorted_alph.size(); ++i) {
        current_indexes[i] = sorted_alph[i];
        pos[sorted_alph[i]] = i;
    }
    vector<uint8_t> result(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        uint8_t cur = input[i];
        uint8_t prev_pos = pos[cur];
        result[i] = prev_pos;
        for (int j = prev_pos - 1; j >= 0; --j) {
            swap(current_indexes[j], current_indexes[j + 1]);
            pos[current_indexes[j + 1]] = j + 1;
        }
        pos[cur] = 0;
    }

    return result;
}

vector<uint8_t> from_mft(const vector<uint8_t> &input, const vector<uint8_t> &sorted_alph) {
    vector<uint8_t> current_indexes(sorted_alph.size());
    unordered_map<uint8_t, size_t> pos;

    for (size_t i = 0; i < sorted_alph.size(); ++i) {
        current_indexes[i] = sorted_alph[i];
        pos[i] = i;
    }
    vector<uint8_t> result(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        uint8_t cur = input[i];
        result[i] = current_indexes[cur];
        for (int j = cur - 1; j >= 0; --j) {
            swap(current_indexes[j], current_indexes[j + 1]);
//            pos[current_indexes[j + 1]] = j + 1;
        }
//        pos[cur] = 0;
    }

    return result;
}


vector<uint8_t> from_bwt(const vector<uint8_t> &input, size_t initial_pos, const vector<uint8_t> &sorted_alph) {
    vector<uint8_t> result(input.size());

    unordered_map<uint8_t, size_t> count_prefix;
    vector<uint8_t> freq(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        freq[i] = count_prefix[input[i]];
        count_prefix[input[i]]++;
    }

    unordered_map<uint8_t, size_t> count_less;
    for (size_t i = 1; i < sorted_alph.size(); ++i) {
        count_less[sorted_alph[i]] = count_less[sorted_alph[i - 1]] + count_prefix[sorted_alph[i - 1]];
    }
    result[input.size() - 1] = input[initial_pos];

    for (signed long i = input.size() - 2; i >= 0; --i) {
        initial_pos = freq[initial_pos] + count_less[input[initial_pos]];
        result[i] = input[initial_pos];
    }

    return result;
}

void build_code_rec(Node *root, unordered_map<uint8_t, code> &mapp, code &code) {
    if (root->id == nullptr) {
        code.add_bit(0);
        build_code_rec(root->left, mapp, code);
        code.remove_bit();
        code.add_bit(1);
        build_code_rec(root->right, mapp, code);
        code.remove_bit();
    } else {
        mapp[*(root->id)] = code;
    }
}

unordered_map<uint8_t, code> build_code(Node *root) {
    unordered_map<uint8_t, code> res;

    code c;

    c.add_bit(0);
    build_code_rec(root->left, res, c);
    c.remove_bit();
    c.add_bit(1);
    build_code_rec(root->right, res, c);
    c.remove_bit();
    return res;
}

pair<pair<bit_buffer, Node *>, size_t> huffman(const vector<uint8_t> &data) {
    unordered_map<uint8_t, size_t> freq;
    for (size_t i = 0; i < data.size(); ++i) {
        freq[data[i]]++;
    }
    multimap<size_t, Node *> nodes;
    for (const auto &pair: freq) {
        nodes.insert({pair.second, new Node(new uint8_t(pair.first), pair.second, nullptr, nullptr)});
    }
    while (nodes.size() > 1) {
        pair <
        const unsigned long, Node *> pair1 = nodes.begin().operator*();
        nodes.erase(nodes.begin());
        pair <
        const unsigned long, Node *> pair2 = nodes.begin().operator*();
        nodes.erase(nodes.begin());
        Node *new_node = new Node(nullptr, pair1.first + pair2.first, pair1.second, pair2.second);
        nodes.insert({new_node->sum, new_node});
    }

    Node *root = nodes.begin()->second;
//    print_tree(root);
    unordered_map<uint8_t, code> coded = build_code(root);

    bit_buffer res;
    for (const auto &item: data) {
        res.add_bits(coded[item]);
    }
    return {{res, root}, freq.size()};
}

void out_tree_rec(ofstream &out, Node *root, vector<uint8_t> &syms) {
    if (root->id == nullptr) {
        char one = 1;
        out.write(&one, 1);
        out_tree_rec(out, root->left, syms);
        out_tree_rec(out, root->right, syms);
    } else {
        char zero = 0;
        out.write(&zero, 1);
        syms.push_back(*root->id);
    }
}

void out_tree(ofstream &out, Node *root) {
    vector<uint8_t> syms;
    out_tree_rec(out, root, syms);
    out.write((char *) syms.data(), syms.size());
}

Node *read_tree(char *data, unsigned long alph_size) {
    vector<uint8_t> syms;
    vector<Node *> nodes;
    vector<Node *> stack;

    for (int i = 0; i < alph_size * 2 - 1; ++i) {
        if (data[i] == 0) {
            Node *child = new Node(nullptr, 0, nullptr, nullptr);
            Node *parent = stack[stack.size() - 1];
            parent->add_child(child);
            while (stack[stack.size() - 1]->has_two_kids()) {
                if (stack.size() == 1) {
                    break;
                }
                Node *kid = stack[stack.size() - 1];
                stack.pop_back();

                Node *par = stack[stack.size() - 1];
                par->add_child(kid);
            }
            nodes.push_back(child);
        } else if (data[i] == 1) {
            Node *new_node = new Node(nullptr, 0, nullptr, nullptr);
            stack.push_back(new_node);
        } else {
            throw runtime_error("asd");
        }
    }
    data = data + alph_size * 2 - 1;
    for (int i = 0; i < alph_size; ++i) {
        nodes[i]->id = new uint8_t(data[i]);
    }
    return stack[0];
}

int main(int argc, char *argv[]) {
    //coding
    if (argc < 2) {
        cout << "coder.exe input_file <output_file>" << endl;
        return 0;
    }

    std::ifstream in(argv[1], std::ios_base::binary);
    vector<uint8_t> input((std::istreambuf_iterator<char>(in)),
                           std::istreambuf_iterator<char>());
//
//    const vector<uint8_t> input = convertToUintVector("abacaba");

    set<uint8_t> alph;
    for (const auto &item: input) {
        alph.insert(item);
    }
    vector<uint8_t> sorted_alph(alph.begin(), alph.end());


    const pair<const vector<uint8_t>, size_t> bwt_result = bwt(input);
//    cout << convertToString(bwt_result.first) << ", " << bwt_result.second << endl;
//
    const vector<uint8_t> &mft_result = mft(bwt_result.first, sorted_alph);
//    cout << convertNumbersToString(mft_result) << endl;


    pair<pair<bit_buffer, Node *>, size_t> b = huffman(mft_result);

//    print_tree(b.first.second);

    string outf(argv[1]);

    if (argc >= 3) {
        outf = string(argv[2]);
    } else {
        outf.append(".coded");
    }

    ofstream out(outf, std::ios_base::binary);

    out.write((char *) &(b.first.first.pos), sizeof(b.first.first.pos));
    out.write((char *) (b.first.first.data.data()), sizeof(uint8_t) * b.first.first.data.size());
    size_t alph_size = b.second;
    out.write((char *) &alph_size, sizeof(size_t));

    size_t init_alph_size = sorted_alph.size();
    out.write((char *) &init_alph_size, sizeof(size_t));

    out_tree(out, b.first.second);
    out.write((char *) &(bwt_result.second), sizeof(bwt_result.second));


    out.write((char *) sorted_alph.data(), sorted_alph.size());


    out.close();
    return 0;
}