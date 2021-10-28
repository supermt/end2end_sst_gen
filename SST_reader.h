//
// Created by supermt on 29/10/2021.
//

#ifndef END2END_SST_GEN_SST_READER_H
#define END2END_SST_GEN_SST_READER_H

#include <stdio.h>
#include <assert.h>
#include "SSTGen.h"

namespace sst_gen {
    class SST_reader {
    public:
        SST_reader(std::string filename);

        bool next_block();

        bool check_block();

        std::string first_key;
        std::string end_key;

    private:
        FILE *opened_file;
        char read_buffer[BLOCK_SIZE];
        int block_index = 0;
        bool finished;

        std::string last_block_end = "";
    };

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

        return std::string(read_buffer + last_key_start,
                           SST_gen::BlockBuilder::key_len);
    }

    bool SST_reader::check_block() {
        std::string current_block_start = ExtractFirstBufferKey(read_buffer);
        std::string current_block_end = ExtractLastBufferKey(read_buffer);

        bool check_result;
        if (current_block_start > current_block_end) return false;
        if (last_block_end < current_block_start) return false;

        return true;
    }

    bool SST_reader::next_block() {

        memset(read_buffer, 's', BLOCK_SIZE);
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


#endif //END2END_SST_GEN_SST_READER_H
