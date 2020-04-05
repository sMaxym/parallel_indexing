#ifndef CFG_H
#define CFG_H

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

// structure that remembers user preferences
struct configuration_t
{
    std::string in_file, out_file1, out_file2;
    int threads;
};

// exception structure when there is no tab in row
struct no_quotes_exception : public std::exception
{
    const char * what () const noexcept override
    {
        return "No quotes sign in string";
    }
};

// exception structure when there is no name in a row
struct invalid_name_exception : public std::exception
{
    const char * what () const noexcept override
    {
        return "Invalid name written";
    }
};

// exception structure when there is no symbol in line
struct no_line_exception : public std::exception
{
    const char * what () const noexcept override
    {
        return "Empty value in line written";
    }
};

configuration_t init(const int& argc, const char* argv[]);
configuration_t read_conf(std::istream& cf);

#endif // CFG_H
