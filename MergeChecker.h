//
// Created by supermt on 29/10/2021.
//
#pragma once

#include "SSTGen.h"
#include "SSTReader.h"

namespace SST_gen {
    class SSTChecker {
    public:
        SSTChecker(std::string target_file, std::string first_key, std::string last_key);

        bool check();

    private:
        SSTReader file_results;
        std::string target_start;
        std::string target_end;
        std::string file_start;
        std::string file_end;
    };

};// end of namsepace

