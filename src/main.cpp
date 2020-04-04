#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "./../header/cfg.h"
#include "./../header/concurqueue.h"
#include "./../header/dcomp.h"
#include "./../header/parsing.h"

int main(int argc, const char* argv[])
{
    if (argc > 2)
    {
        std::cout << "Argument error" << std::endl;
        return 1;
    }

    // naming config file
    std::string confFileName;
    if (argc == 2)
    {
        confFileName = argv[1];
    } else{
        confFileName = "../config.dat";
    }

    // attempt to open config file
    std::ifstream config_stream(confFileName);
    if(!config_stream.is_open()) {
        std::cerr << "Failed to open configuration file " << confFileName << std::endl;
        return 2;
    }

    // configuration process with data structure configuration
    // that contains inputFileName, outputFileName, number of threads
    configuration_t config {};
    try
    {
        config = read_conf(config_stream);
    }catch (std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 3;
    }

    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);
    std::cout.imbue(loc);


    std::ifstream raw_file(config.in_file, std::ios::binary);
    auto buffer = [&raw_file] {
                    std::ostringstream ss{};
                    ss << raw_file.rdbuf();
                    return ss.str();
                } ();
    raw_file.close();

    std::string data = "";

    try
    {
        decompress(buffer, data);
    } catch (std::runtime_error &e) {
        std::cout << "Error while trying to decompress input archive" << std::endl;
        return 1;
    }

    std::vector<std::string> indexing_blocks;
    partition(data, config.threads, indexing_blocks);

    concur_queue<std::string> input_blocks;
    for (const auto &block: indexing_blocks)
        input_blocks.push(block);

    concur_queue<WORD_MAP> counter;
    while (input_blocks.get_size())
        parse(input_blocks.pop(), counter);

    WORD_MAP cur_words, merge_words;
    while (counter.get_size() > 1)
    {
        cur_words = counter.pop();
        merge_words = counter.pop();
        for (const auto &[key, value]: merge_words)
            cur_words[key] += value;
        counter.push(cur_words);
    }

    cur_words = counter.pop();
    for (const auto &[key, value]: cur_words)
        std::cout << key << " " << value << std::endl;

    return 0;
}
