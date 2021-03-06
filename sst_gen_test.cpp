#include <iostream>
#include "SSTGen.h"
#include <gflags/gflags.h>
#include "MergeChecker.h"

using gflags::ParseCommandLineFlags;
DEFINE_string(overlap_mode, "full", "the overlapping mode of files,"
                                    "candidates: \n"
                                    "none(1234,5678)\n"
                                    "full(1357,2468)\n"
                                    "random(fully random)\n");
DEFINE_int32(num_files, 2, "number of generated files");
DEFINE_string(start_value_list, "1,2", "");
DEFINE_int32(entries, 5000000, "number of entries in one single file");


int main(int argc, char **argv) {
    ParseCommandLineFlags(&argc, &argv, false);

    int blocks_in_one_file = FLAGS_entries / 312;
    int sst_file_num = FLAGS_num_files;

    std::string s = FLAGS_start_value_list;
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    std::vector<int> start_value;

    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
        start_value.push_back(std::stoi(token));
    }
    start_value.push_back(std::stoi(s));
    std::cout << "there are " << blocks_in_one_file << " blocks in one file." << std::endl;
    std::string suffix = ".sst";

    SST_gen::OverlapMode mode = SST_gen::kNone;
    std::cout << FLAGS_overlap_mode << std::endl;
    if (FLAGS_overlap_mode == "full") {
        mode = SST_gen::kFull;
    } else if (FLAGS_overlap_mode == "none") {
        mode = SST_gen::kNone;
    } else if (FLAGS_overlap_mode == "random") {
        mode = SST_gen::kRandom;
    }

    sst_file_num = start_value.size();

    if (mode == SST_gen::kNone) {
        start_value.clear();
        for (int i = 0; i < FLAGS_num_files; i++) {
            start_value.push_back(i * FLAGS_entries);
        }
    } else if (mode == SST_gen::kFull) {
        for (int i = sst_file_num; i < FLAGS_num_files; i++) {
            start_value.push_back(i);
        }
    } else {
        for (int i = sst_file_num; i < FLAGS_num_files; i++) {
            start_value.push_back(i + rand() % FLAGS_num_files + 1);
        }
    }


    for (int i = 0; i < FLAGS_num_files; i++) {
        std::string file_name = std::to_string(i) + suffix;
        std::cout << "creating file: " << file_name << std::endl;
        SST_gen::SSTBuilder temp(blocks_in_one_file, file_name, start_value[i], mode);
//        SST_gen::SSTChecker checker_of_temp(file_name, temp.first_key, temp.end_key);
    }
    return 0;
}
