#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "./../header/cfg.h"
#include "./../header/concurqueue.h"
#include "./../header/dcomp.h"
#include "./../header/text_edit.h"
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
    partition(data, config.threads * 2, indexing_blocks);

    concur_queue<std::string> a;
    for (const auto &val: indexing_blocks)
        a.push(val);

    concur_queue<WORD_MAP> counter;



    boost::locale::generator gen;
    // Create system default locale
    std::locale loc=gen("en_US.UTF-8");
    //????? Make it system global
    std::locale::global(loc);
    std::cout.imbue(loc); //?? (making standard locale for cout)

    size_t tick = 0;
    for (std::string &block: indexing_blocks)
    {
        std::cout << tick << std::endl;
        parse(block, counter);
        tick++;
    }

    while (counter.get_size())
    {
        std::cout << "New map" << std::endl << std::endl;
        WORD_MAP counter_part = counter.pop();
        for (const auto& [key, value]: counter_part)
        {
            std::cout << key << " " << value << std::endl;
        }
    }

    return 0;
}
