#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <functional>
#include <thread>

#include "./../header/cfg.h"
#include "./../header/concurqueue.h"
#include "./../header/dcomp.h"
#include "./../header/parsing.h"
#include "./../header/timer.h"

template<typename Key, typename Value>
std::multimap<Key, Value> swap_map(const std::map<Value, Key>& values);

int main(int argc, const char* argv[])
{
    long long time_reading, time_indexing, time_writing;
    configuration_t config;
    std::string data = "", block;
    std::vector<std::string> indexing_blocks;
    concur_queue<std::string> input_blocks;
    concur_queue<WORD_MAP> counter;
    WORD_MAP cur_words;

    try
    {
        config = init(argc, argv);
    } catch (std::runtime_error &e)
    {
        std::cout << "Runtime exception: " << e.what() << std::endl;
        return 1;
    }

    boost::locale::generator gen;
    std::locale loc = gen("en_US.UTF-8");
    std::locale::global(loc);
    std::cout.imbue(loc);


    // READING AND DECOMPRESSING ----------------------------

    auto start_time_stamp = get_current_time_fenced();
    std::ifstream raw_file(config.in_file, std::ios::binary);
    auto buffer = [&raw_file] {
                    std::ostringstream ss{};
                    ss << raw_file.rdbuf();
                    return ss.str();
                } ();
    raw_file.close();

    try
    {
        decompress(buffer, data);
    } catch (std::runtime_error &e)
    {
        std::cout << "Error while trying to decompress input archive" << std::endl;
        return 1;
    }

    time_reading = to_us(get_current_time_fenced() - start_time_stamp);

    // INDEXING ---------------------------------------------

    start_time_stamp = get_current_time_fenced();
    partition(data, config.threads, indexing_blocks);
    for (const auto &block: indexing_blocks)
        input_blocks.push(block);


    std::vector<std::thread> index_threads;
    std::atomic<long long> blocks_left = input_blocks.get_size() - 1;
    for (size_t thread_id = 0; thread_id < config.threads; ++thread_id)
    {
        index_threads.emplace_back(index_parallel, std::ref(input_blocks), std::ref(counter));
        index_threads.emplace_back(merge_parallel, std::ref(counter), std::ref(blocks_left));
    }
    for (auto& th: index_threads)
    {
        th.join();
    }
    cur_words = counter.pop();

    time_indexing = to_us(get_current_time_fenced() - start_time_stamp);

    // WRITING ----------------------------------------------

    start_time_stamp = get_current_time_fenced();

    std::ofstream output(config.out_file1, std::fstream::out);
    for (const auto& [key, value]: cur_words)
        output << std::setw(20) << std::left << key <<
                  std::setw(20) << std::left << value << std::endl;
    output.close();

    output.open(config.out_file2, std::fstream::out);
    std::multimap<size_t, std::string> words_by_amount = swap_map(cur_words);
    for (auto it = words_by_amount.rbegin(); it != words_by_amount.rend(); ++it)
        output << std::setw(20) << std::left << it->second <<
                  std::setw(20) << std::left << it->first << std::endl;
    output.close();

    time_writing = to_us(get_current_time_fenced() - start_time_stamp);

    // ------------------------------------------------------

    std::cout << "Loading:\t" << time_reading * sec_factor << std::endl <<
                 "Analyzing:\t" << time_indexing * sec_factor << std::endl <<
                 "Total:\t\t" << (time_reading + time_indexing + time_writing) * sec_factor << std::endl;

    return 0;
}

template<typename Key, typename Value>
std::multimap<Key, Value> swap_map(const std::map<Value, Key>& values)
{
    std::multimap<Key, Value> result;
    for (const auto& [key, value]: values)
        result.insert(std::pair<Key, Value>(value, key));
    return result;
}
