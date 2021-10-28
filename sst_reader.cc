//
// Created by supermt on 29/10/2021.
//

#include "SST_reader.h"

namespace SST_gen {

    SST_reader::SST_reader(std::string filename) {
        opened_file = fopen(filename.c_str(), "r");
//        memset(read_buffer, 's', BLOCK_SIZE);
//        size_t read_bytes = fread(read_buffer, sizeof(char), BLOCK_SIZE, opened_file);
//        // this is the head block
//        assert(read_bytes != 0);
        block_index = 0;
    }

    inline std::string ExtractFirstBufferKey(char *read_buffer) {
        int first_key_start = BLOCK_SIZE - SST_gen::BlockBuilder::key_len;
        return std::string(read_buffer + first_key_start,
                           SST_gen::BlockBuilder::key_len);

    }

    inline std::string ExtractLastBufferKey(char *read_buffer) {
        int last_key_start = BLOCK_SIZE - SST_gen::BlockBuilder::key_len * SST_gen::BlockBuilder::entry_count;
        std::string result = std::string(read_buffer + last_key_start,
                                         SST_gen::BlockBuilder::key_len);
        return result;
    }

    bool SST_reader::check_block() {
        std::string current_block_start = ExtractFirstBufferKey(read_buffer);
        std::string current_block_end = ExtractLastBufferKey(read_buffer);

        bool check_result;
        if (current_block_start > current_block_end) return false;
        if (last_block_end > current_block_start) return false;

        return true;
    }

    bool SST_reader::next_block() {
        size_t read_bytes = fread(read_buffer, sizeof(char), BLOCK_SIZE, opened_file);
        // this is the head block
        if (block_index == 0) {
            first_key = ExtractFirstBufferKey(read_buffer);
        }
        if (read_bytes == 0) {
            end_key = ExtractLastBufferKey(read_buffer);
        }
        block_index++;
        return read_bytes != 0;
    }
}