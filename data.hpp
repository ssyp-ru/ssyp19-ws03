#pragma once

#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <cstdio>

struct SubRecord {
    std::string name;
    int size;
    std::string data;

    SubRecord(std::string name, int size, std::string data)
            : name(std::move(name)), size(size), data(std::move(data)) {}
};

struct Record {
    std::string name;
    int size;
    int header;
    int flags;
    std::vector<SubRecord> subrecords;

    Record(std::string name, int size, int header, int flags,
           std::vector<SubRecord> subrecords)
            : name(std::move(name)), size(size), header(header), flags(flags), subrecords(std::move(subrecords)) {}
};

bool
human_unpack(Record &nowRecord,
             SubRecord &nowSubRecord,
             std::filesystem::path recordPath,
             int k,
             FILE *file,
             FILE *output);

bool human_pack(std::vector<Record> &records,
                std::filesystem::path flex);