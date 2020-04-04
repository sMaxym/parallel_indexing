#include "./../header/text_edit.h"

void partition(const std::string &data, size_t n_parts, std::vector<std::string> &res)
{
    if (data.size() < n_parts)
        throw std::invalid_argument("Size of an input string less than the amount of partitions");
    const std::string delims = " \n\r\t";
    size_t part_len = data.size() / n_parts,
           residual = data.size() % n_parts,
            index = 0, cur_len;
    for (size_t part = 1; part <= n_parts; ++part)
    {
        cur_len = part_len;
        if (residual--)
            cur_len++;
        res.emplace_back(data.substr(index, cur_len));
        index += cur_len;
    }
}
