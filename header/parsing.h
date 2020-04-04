#ifndef INDEXING_PARSING_H
#define INDEXING_PARSING_H

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include "concurqueue.h"

void parse(const std::string &text, concur_queue<WORD_MAP> &output);

#endif //INDEXING_PARSING_H
