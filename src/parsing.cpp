#include "./../header/parsing.h"

void parse(const std::string &text, concur_queue<WORD_MAP> &output)
{
    std::string word;
    std::map<std::string, size_t> vocabulary;
    boost::locale::boundary::ssegment_index myMap(boost::locale::boundary::word,text.begin(),text.end());
    myMap.rule(boost::locale::boundary::word_any);
    for (const auto &x: myMap)
    {
        word = boost::locale::normalize(std::string(x), boost::locale::norm_default);
        word = boost::locale::fold_case(word);
        vocabulary[word]++;
    }
    output.push(vocabulary);
}
