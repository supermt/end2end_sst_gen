//
// Created by supermt on 13/10/2021.
//
#include <vector>
#include <string>
#include <assert.h>
#include <cstring>
#include <iostream>

#ifndef END2END_SST_GEN_SSTGEN_H
#define END2END_SST_GEN_SSTGEN_H

#ifndef PLATFORM_IS_LITTLE_ENDIAN
#define PLATFORM_IS_LITTLE_ENDIAN (__BYTE_ORDER == __LITTLE_ENDIAN)
#endif

#define BLOCK_SIZE 8192
namespace port {
    constexpr bool kLittleEndian = PLATFORM_IS_LITTLE_ENDIAN;
}// end namespace
namespace SST_gen {

    class RandomStringGen {
        // since the strings in one single SST should be fully sorted
        int key_len_;
        int str_num_;
        uint64_t next_key;
        std::vector<std::string> buffer_pool;
        int popped_keys;

        void generate();

        void sort_buffer();

        uint64_t next_intkey();

    public:
        RandomStringGen(int key_len, int str_num) : key_len_(key_len), str_num_(str_num), buffer_pool(str_num),
                                                    popped_keys(0) {
            std::cout << "generating" << std::endl;
            generate();
//            std::cout << "sorting" << std::endl;
//            sort_buffer();
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
        const static int entry_count = 312;

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
        SSTBuilder(int block_num, std::string &file_name) : blocks(block_num), file_name_(file_name),
                                                            key_builder(BlockBuilder::key_len,
                                                                        block_num * BlockBuilder::entry_count) {
            target_file = fopen(file_name_.c_str(), "w");

            int write_result = 0;
            int block_count = 0;
            for (auto block: blocks) {
                block_count++;
                for (int i = 0; i < BlockBuilder::entry_count; i++) {
                    block.AddKey(key_builder.get_next());
                }
                write_result = block.WriteToFile(target_file);
                assert(write_result != -1);
            }
            fflush(target_file);
            fclose(target_file);
        }
    };

}// end namespace


#endif //END2END_SST_GEN_SSTGEN_H
