//
// Created by supermt on 29/10/2021.
//

#include "SSTReader.h"
#include <sys/stat.h>
#include <fstream>

namespace SST_gen {

    SSTReader::SSTReader(std::string filename) {
        this->file_name = filename;
        opened_file = fopen(filename.c_str(), "r");
//        memset(read_buffer, 's', BLOCK_SIZE);
//        size_t read_bytes = fread(read_buffer, sizeof(char), BLOCK_SIZE, opened_file);
//        // this is the head block
//        assert(read_bytes != 0);
        block_index = 0;

        std::streampos begin, end;
        std::ifstream temp(file_name);
        begin = temp.tellg();
        temp.seekg(0, std::ios::end);
        end = temp.tellg();
        file_size = end - begin;
    }

    inline std::string ExtractFirstBufferKey(char *read_buffer) {
        int first_key_start = BLOCK_SIZE - SST_gen::BlockBuilder::key_len;
        return std::string(read_buffer + first_key_start,
                           SST_gen::BlockBuilder::key_len);

    }

    inline std::string ExtractLastBufferKey(char *read_buffer) {
        int last_key_start =
                BLOCK_SIZE - SST_gen::BlockBuilder::key_len * SST_gen::BlockBuilder::single_block_entry_count;
        std::string result = std::string(read_buffer + last_key_start,
                                         SST_gen::BlockBuilder::key_len);
        return result;
    }

    bool SSTReader::check_block() {
        std::string current_block_start = ExtractFirstBufferKey(read_buffer);
        std::string current_block_end = ExtractLastBufferKey(read_buffer);

        bool check_result;
        if (current_block_start > current_block_end) return false;
        if (last_block_end > current_block_start) return false;

        return true;
    }

    bool SSTReader::next_block() {
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

    bool SSTReader::entry_at(int position, std::string *key, std::string *value) {
        int block_no = position / BlockBuilder::single_block_entry_count;
        int offset = position % BlockBuilder::single_block_entry_count;
        uint64_t key_offset = BLOCK_SIZE * (block_no + 1) - offset * BlockBuilder::key_len;
        uint64_t value_offset = BLOCK_SIZE * (block_no) + offset * BlockBuilder::value_len;
        const int buffer_size = std::max(BlockBuilder::key_len, BlockBuilder::value_len);
        char buffer[buffer_size];
        fseek(opened_file, key_offset, SEEK_SET);
        memset(buffer, 0, buffer_size);
        if (fread(buffer, sizeof(char), BlockBuilder::key_len, opened_file) == 0) return false;
        key->clear();
        key->append(buffer);
        fseek(opened_file, value_offset, SEEK_SET);
        if (fread(buffer, sizeof(char), BlockBuilder::value_len, opened_file) == 0) return false;
        memset(buffer, 0, buffer_size);
        value->clear();
        value->append(buffer);
        return true;
    }

    int SSTReader::num_blocks() {
        return (int) file_size / BLOCK_SIZE;
    }

    int SSTReader::num_entries() {
        return num_blocks() * BlockBuilder::single_block_entry_count;
    }
}