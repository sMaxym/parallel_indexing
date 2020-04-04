#include "./../header/dcomp.h"

void decompress(const std::string &bin, std::string &res)
{
    int status;
    struct archive *arch = archive_read_new();
    struct archive_entry *entry;

    status = archive_read_support_filter_all(arch);
    archive_read_support_format_raw(arch);
    archive_read_support_format_all(arch);

    status = archive_read_open_memory(arch, bin.c_str(), bin.size());
    if (status != ARCHIVE_OK)
        throw std::runtime_error("Cannot find archive data in memory");

    const void *buff;
    size_t size;
    int64_t offset;
    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK)
    {
        do
        {
            status = archive_read_data_block(arch, &buff, &size, &offset);
            if (status == ARCHIVE_EOF)
                break;
            if (status != ARCHIVE_OK)
                throw std::runtime_error("Error while decompressing archive");
            res.append(static_cast<const char*>(buff), size);
        } while (true);
    }

    archive_read_close(arch);
    archive_read_free(arch);
}
