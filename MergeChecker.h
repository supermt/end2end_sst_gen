//
// Created by supermt on 29/10/2021.
//
#ifndef END2END_SST_GEN_MERGECHECKER_H
#define END2END_SST_GEN_MERGECHECKER_H

#include "SSTGen.h"
#include "SST_reader.h"

namespace sst_gen {
    class Checker {
    public:
        Checker(std::string target_file, std::string first_key, std::string last_key);

        bool check();

    private:
        SST_reader file_results;
        std::string target_start;
        std::string target_end;
        std::string file_start;
        std::string file_end;
    };

};// end of namsepace

#endif //END2END_SST_GEN_MERGECHECKER_H
