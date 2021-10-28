//
// Created by supermt on 29/10/2021.
//

#include "MergeChecker.h"
#include "SSTGen.h"
#include "SST_reader.h"

namespace sst_gen {

    bool Checker::check() {
        bool checker_result;
        int count = 0;
        while (file_results.next_block()) {
            checker_result = file_results.check_block();
            if (!checker_result) return false;
        }
        file_start = file_results.first_key;
        file_end = file_results.end_key;
        return (file_start == target_start && file_end == target_end);
    }

    Checker::Checker(std::string target_file, std::string first_key, std::string last_key) : file_results(target_file) {
        target_start = first_key;
        target_end = last_key;
    }


}

