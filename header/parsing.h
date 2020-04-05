#ifndef INDEXING_PARSING_H
#define INDEXING_PARSING_H

#include <string>
#include <vector>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include "concurqueue.h"

void index_parallel(concur_queue<std::string> &input, concur_queue<WORD_MAP> &output);
void parse(const std::string &text, concur_queue<WORD_MAP> &output);
void partition(const std::string &data, size_t n_parts, std::vector<std::string> &res);
void merge_counter(concur_queue<WORD_MAP> &counter);

#endif //INDEXING_PARSING_H
