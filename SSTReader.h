//
// Created by supermt on 29/10/2021.
//
#pragma once

#include <stdio.h>
#include <assert.h>
#include "SSTGen.h"

namespace SST_gen {
    class SSTReader {
    public:
        SSTReader(std::string filename);

        ~SSTReader() {
            fclose(opened_file);
        }

        bool entry_at(int position, std::string *key, std::string *value);

        bool next_block();

        bool check_block();

        int num_blocks();

        int num_entries();

        std::string first_key;
        std::string end_key;

    private:
        FILE *opened_file;
        std::string file_name;
        char read_buffer[BLOCK_SIZE];
        int block_index = 0;
        bool finished;
        uint64_t file_size;
        std::string last_block_end = "";
    };


}
