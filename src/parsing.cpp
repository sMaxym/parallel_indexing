#include "./../header/parsing.h"

void index_parallel(concur_queue<std::string> &input, concur_queue<WORD_MAP> &output)
{
    while (input.get_size())
        parse(input.pop(), output);
}

void parse(const std::string &text, concur_queue<WORD_MAP> &output)
{
    std::string word;
    std::map<std::string, size_t> vocabulary;
    boost::locale::boundary::ssegment_index myMap(boost::locale::boundary::word,text.begin(),text.end());
    myMap.rule(boost::locale::boundary::word_letters);
    for (const auto &x: myMap)
    {
        word = boost::locale::normalize(std::string(x), boost::locale::norm_default);
        word = boost::locale::fold_case(word);
        vocabulary[word]++;
    }
    output.push(vocabulary);
}

void partition(const std::string &data, size_t n_parts, std::vector<std::string> &res)
{
    if (data.size() < n_parts)
        throw std::invalid_argument("Size of an input string less than the amount of partitions");
    const std::string delims = " \n\r\t";
    size_t data_size = data.size();
    size_t part_len = data_size / n_parts,
            index = 0, cur_len;
    int residual = static_cast<int>(data_size % n_parts);
    char cur_symb;
    for (size_t part = 1; part <= n_parts; ++part)
    {
        cur_len = part_len;
        if (residual-- > 0)
            cur_len++;
        while (true)
        {
            if (index + cur_len >= data_size)
                break;
            cur_symb = data.at(index + cur_len - 1);
            if (delims.find(cur_symb) != std::string::npos)
                break;
            cur_len++;
        }
        res.emplace_back(data.substr(index, cur_len));
        index += cur_len;
        if (index >= data_size - 1)
            break;
    }
}

void merge_counter(concur_queue<WORD_MAP> &counter)
{
    WORD_MAP cur_words, merge_words;
    while (counter.get_size() > 1)
    {
        cur_words = counter.pop();
        merge_words = counter.pop();
        for (const auto &[key, value]: merge_words)
            cur_words[key] += value;
        counter.push(cur_words);
    }
}

void merge_parallel(concur_queue<WORD_MAP> &counter, std::atomic<long long> &blocks_left)
{
    std::pair<WORD_MAP, WORD_MAP> to_merge;
    WORD_MAP cur_words;
    while (blocks_left)
    {
        blocks_left--;
        to_merge = counter.pop2();
        cur_words = to_merge.first;
        for (const auto &[key, value]: to_merge.second)
            cur_words[key] += value;
        counter.push(cur_words);
    }
}










