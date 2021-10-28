//
// Created by supermt on 29/10/2021.
//
#pragma once

#include <stdio.h>
#include <assert.h>
#include "SSTGen.h"

namespace SST_gen {
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


}
