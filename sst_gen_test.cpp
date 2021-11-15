#include <iostream>
#include "SSTGen.h"
#include "MergeChecker.h"

int main() {
    int blocks_in_one_file = 5000000 / 312;
    int sst_file_num = 1;
    std::cout << "there are " << blocks_in_one_file << " blocks in one file." << std::endl;
    std::string suffix = ".sst";
    for (int i = 0; i < sst_file_num; i++) {
        std::string file_name = std::to_string(i) + suffix;
        std::cout << "creating file: " << file_name << std::endl;
        SST_gen::SSTBuilder temp(blocks_in_one_file, file_name);

        SST_gen::SSTChecker checker_of_temp(file_name, temp.first_key, temp.end_key);
        std::cout << checker_of_temp.check() << std::endl;

    }
    return 0;
}
