#ifndef DCOMP_H
#define DCOMP_H

#include <string>
#include <stdexcept>
#include <archive.h>
#include <archive_entry.h>

#include <iostream>

constexpr size_t BUFFER_SIZE = 1024;

void decompress(const std::string &bin, std::string &res);

#endif // DCOMP_H
