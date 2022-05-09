//
// Created by supermt on 13/10/2021.
//
#include <vector>
#include <string>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <iomanip>

#pragma once
#ifndef PLATFORM_IS_LITTLE_ENDIAN
#define PLATFORM_IS_LITTLE_ENDIAN (__BYTE_ORDER == __LITTLE_ENDIAN)
#endif

#define BLOCK_SIZE 8192
namespace port {
    constexpr bool kLittleEndian = PLATFORM_IS_LITTLE_ENDIAN;
}// end namespace

inline std::string ToHex(const std::string &s, bool upper_case = true) {
    std::ostringstream ret;

    for (std::string::size_type i = 0; i < s.length(); ++i)
        ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase)
            << (int) s[i];

    return ret.str();
}

namespace SST_gen {

    enum OverlapMode : int {
        kNone = 0,
        kFull,
        kRandom,
    };

    class RandomStringGen {
        // since the strings in one single SST should be fully sorted
        int key_len_ = 0;
        int str_num_ = 0;
        uint64_t next_key = 0;
        std::vector<std::string> buffer_pool;
        int popped_keys;
        int start_value_;
        OverlapMode overlap_;


        void generate();

        void sort_buffer();

        uint64_t next_intkey();

    public:

        RandomStringGen(int key_len, int str_num) : RandomStringGen(key_len, str_num, 0, kRandom) {}

        RandomStringGen(int key_len, int str_num, int start_value, OverlapMode mode) : key_len_(key_len),
                                                                                       str_num_(str_num),
                                                                                       buffer_pool(str_num),
                                                                                       popped_keys(0),
                                                                                       start_value_(start_value),
                                                                                       overlap_(mode) {
            std::cout << "generating" << std::endl;
            generate();
            std::cout << "sorting" << std::endl;
            sort_buffer();
            std::cout << "first ten keys" << std::endl;
            for (int i = 0; i < 10; i++) {
                std::cout << ToHex(buffer_pool[i]) << "," << std::endl;
            }
        }

        std::string get_next() {
            return buffer_pool[popped_keys++];
        }

    };

    class BlockBuilder {

        void AddEntry(std::pair<std::string, std::string>);

        void FillValuePart();

        void FillHeader();

        int left_offset = 0;
        int right_offset = 0;
        char buffer[BLOCK_SIZE];
        char *buffer_key_part_ptr;
    public:

        void AddKey(std::string input);

        const static int header_len = 64;
        const static int key_len = 16;
        const static int value_len = 10;
        const static int single_block_entry_count = 312;

        BlockBuilder() : left_offset(0), right_offset(0) {
            memset(buffer, 's', BLOCK_SIZE);
            buffer_key_part_ptr = &buffer[BLOCK_SIZE - right_offset];
            FillHeader();
            FillValuePart();
        }

        int WriteToFile(FILE *target_file) {
            size_t result = fwrite(buffer, sizeof(char), BLOCK_SIZE, target_file);
            return result >= 0 ? 0 : -1;
        }
    };

    class SSTBuilder {
        std::vector<BlockBuilder> blocks;
        std::string file_name_;
        RandomStringGen key_builder;
        FILE *target_file;
    public:
        std::string first_key;
        std::string end_key;

        SSTBuilder(int block_num, std::string &file_name, int start_key, OverlapMode mode) : blocks(block_num),
                                                                                             file_name_(file_name),
                                                                                             key_builder(
                                                                                                     BlockBuilder::key_len,
                                                                                                     block_num *
                                                                                                     BlockBuilder::single_block_entry_count,
                                                                                                     start_key, mode) {
            target_file = fopen(file_name_.c_str(), "w");

            int write_result = 0;
            int block_count = 0;
            bool first_generated = true;
            std::string next_key;
            for (auto block: blocks) {
                block_count++;
                for (int i = 0; i < BlockBuilder::single_block_entry_count; i++) {
                    next_key = key_builder.get_next();
                    if (first_generated) first_key = next_key;
                    first_generated = false;
                    block.AddKey(next_key);
                }
                write_result = block.WriteToFile(target_file);
                assert(write_result != -1);
            }
            end_key = next_key;
            fflush(target_file);
            fclose(target_file);
        }
    };
}// end namespace
