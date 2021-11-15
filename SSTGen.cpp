//
// Created by supermt on 13/10/2021.
//

#include <unistd.h>
#include <algorithm>
#include "SSTGen.h"

using namespace std;
namespace SST_gen {

    std::string random_key(uint64_t v, const int key_len) {

        std::string key(16, 'k');
        char *start = const_cast<char *>(key.data());
        char *pos = start;
        int bytes_to_fill = std::min(key_len - static_cast<int>(pos - start), 8);
        if (port::kLittleEndian) {
            for (int i = 0; i < bytes_to_fill; ++i) {
                pos[i] = (v >> ((bytes_to_fill - i - 1) << 3)) & 0xFF;
            }
        } else {
            memcpy(pos, static_cast<void *>(&v), bytes_to_fill);
        }
        pos += bytes_to_fill;
        if (key_len > pos - start) {
            memset(pos, '0', key_len - (pos - start));
        }
        return key;
    }

    void BlockBuilder::FillHeader() {
        memset(buffer, 'h', header_len);
        left_offset += header_len;
    }

    void BlockBuilder::AddEntry(std::pair<std::string, std::string>) {
        return;
        // dummy function
    }

    void BlockBuilder::FillValuePart() {
        // dummy function, just fill 312 * 10 bytes of data;
        memset(buffer + left_offset, 'v', single_block_entry_count * value_len);
    }

    void BlockBuilder::AddKey(std::string input) {
        right_offset += key_len;
        buffer_key_part_ptr = &buffer[BLOCK_SIZE - right_offset];
        memcpy(buffer_key_part_ptr, input.c_str(), key_len);
    }

    void RandomStringGen::generate() {
        for (int i = 0; i < str_num_; i++) {
            buffer_pool[i] = random_key(next_intkey(), key_len_);
        }
    }

    void RandomStringGen::sort_buffer() {
        sort(buffer_pool.begin(), buffer_pool.end());
    }

    uint64_t RandomStringGen::next_intkey() {
        next_key += rand() % 10;
        return next_key;
    }

}