//
// Created by supermt on 11/15/21.
//

#include <iostream>
#include "SSTGen.h"
#include "MergeChecker.h"
#include <gflags/gflags.h>
#include <fnmatch.h>
#include <ftw.h>
#include <cstdlib>


#define SST_SUFFIX "*.sst"

DEFINE_string(path_name, "./", "Define the file store place for the system");
DEFINE_bool(show_work_progress, true, "Default is true, and will print out all results");
DEFINE_uint64(access_time, 1000000l, "How many random access operations in the workload");
DEFINE_uint64(seed, 0, "Random seed");
DECLARE_bool(help);

std::vector<std::string> sst_files;

static int explore(const char *fpath,
                   const struct stat *sb,
                   int typeflag) {
    if (typeflag == FTW_F)  ///< it's a file
    {
        if (fnmatch(SST_SUFFIX, fpath, FNM_CASEFOLD) == 0) ///< it's a .sst file
        {
            sst_files.emplace_back(fpath);
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    ftw(FLAGS_path_name.c_str(), explore, 8);
    if (FLAGS_show_work_progress) {
        for (const auto &file_name: sst_files) {
            std::cout << file_name << std::endl;
        }
    }
    if (sst_files.empty()) {
        std::cout << "error! There's no sst files in the dir, use sst_gen_test to generate at least one" << std::endl;
    }
    uint64_t access_in_each_files = FLAGS_access_time / sst_files.size() + 1;
    srand(FLAGS_seed);

    std::vector<std::pair<std::string, uint64_t>> access_results;
    for (const auto &file_name: sst_files) {
        SST_gen::SSTReader reader(file_name);
        std::pair<std::string, uint64_t> file_access_stat(file_name, 0);
        uint64_t success = 0;
        int access_position = 0;
        int rand_limit = reader.num_entries();
        std::string key, value;
        bool get;
        for (uint64_t i = 0; i < access_in_each_files; i++) {
            access_position = rand() % rand_limit;
            get = reader.entry_at(access_position, &key, &value);
            if (get) success++;
        }
        file_access_stat.second = success;
        access_results.push_back(file_access_stat);
    }
    if (FLAGS_show_work_progress) {
        for (auto stat_entry: access_results) {
            std::cout << stat_entry.first << " founded entries: " << stat_entry.second << " from "
                      << access_in_each_files << std::endl;
        }
    }

    return 0;

}