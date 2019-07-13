#include "data.hpp"
#include <charconv>
#include <filesystem>
#include <cstdlib>
#include "fmem.h"

void printrec(Record &a, int i) {
    printf("%4s №%d\n", a.name.data(), i);
    printf("%d %d %d\n", a.size, a.header, a.flags);
    for (SubRecord &v : a.subrecords) {
        printf("%4s: %03d | %.*s\n", v.name.data(), v.size, v.size, v.data.data());
    }
    printf("\n");
}

bool loadFromFileTree(std::vector<Record> &records,
                      const std::filesystem::path &path) {
    std::filesystem::directory_entry direct(path);
    bool o = true;

    for (int i = 0; o;) {
        std::filesystem::directory_iterator directory_iterator(direct);
        o = false;
        for (const std::filesystem::directory_entry &v : directory_iterator) {
            if (!v.is_directory())
                continue;

            std::filesystem::path pathB = v.path() / std::to_string(i);
            if (!std::filesystem::exists(pathB))
                continue;

            o = true;

            FILE *pathC = fopen(pathB.string().data(), "r");
            if (pathC == NULL)
                return false;

            for (; !feof(pathC);) {
                std::string name;
                int size;
                int header;
                int flags;
                std::vector<SubRecord> subrec;

                name.reserve(5);

                switch (fscanf(pathC, "%4s", name.data())) {
                    case 1:
                        break;
                    case EOF:
                        continue;
                    default:
                        printf("ERROR [read] (name): %s\n", name.data());
                        return false;
                }

                if (int v = fscanf(pathC, "%d", &size); v != 1) {
                    printf("ERROR [read] (size): %s %d\n", name.data(), v);
                    return false;
                }

                if (fscanf(pathC, "%d", &header) != 1) {
                    printf("ERROR [read] (flags1): %s\n", name.data());
                    return false;
                }

                if (fscanf(pathC, "%d", &flags) != 1) {
                    printf("ERROR [read] (flags2): %s\n", name.data());
                    return false;
                }

                for (int sumssize = 0; !feof(pathC) && sumssize < size;) {
                    std::string sname(5, 0);
                    int ssize = 0;

                    if (fscanf(pathC, "%4s", sname.data()) != 1) {
                        printf("ERROR [read] (:name): %s\n", name.data());
                        return false;
                    }

                    if (fscanf(pathC, "%d", &ssize) != 1) {
                        printf("ERROR [read] (:size): %s\n", name.data());
                        return false;
                    }

                    fseek(pathC, 1, SEEK_CUR);

                    sumssize += 8;
                    std::string sdata(ssize, 0);

                    if (long start = ftell(pathC),
                                res = fread(sdata.data(), 1, ssize, pathC);
                            res != ssize) {
                        printf("ERROR [read] (:data): \"%s\" ret=%ld %ld-%ld errno=%d\n",
                               sname.data(), res, start, ftell(pathC), errno);
                        return false;
                    }

                    sumssize += ssize;
                    subrec.emplace_back(sname, ssize, sdata);
                }

                records.emplace_back(name, size, header, flags, subrec);
            }

            fclose(pathC);
            i++;
        }
    }
    return true;
}

bool loadFromFileTreeHuman(std::vector<Record> &records,
                           std::filesystem::path &path) {
    return human_pack(records, path);
}

void subsubsearch(char *buffer, size_t size, Record &current) {
    fgets(buffer, size, stdin);
    std::string setName = buffer;
    setName.resize(setName.size() - 1);

    for (SubRecord &v : current.subrecords) {
        if (setName == v.name) {
            fgets(buffer, size, stdin);
            std::string newData = buffer;
            newData.resize(newData.size() - 1);

            v.data = newData;
            v.size = newData.size();
            current.size = 0;
            for (SubRecord &v : current.subrecords) {
                current.size += v.size;
                current.size += 8;
            }

            break;
        }
    }
}

bool subsearch(std::vector<std::vector<Record>::iterator> &results) {
    char buffer[128];

    int number = 0;
    while (true) {
        printf("выберите один из результатов поиска\nили введите -1 для выхода из "
               "режима поиска\n");

        char *res = fgets(buffer, sizeof(buffer), stdin);

        if (res == NULL) {
            return true;
        }

        std::from_chars_result parseResult =
                std::from_chars(buffer, buffer + sizeof(buffer), number);

        if (parseResult.ec != std::errc()) {
            return true;
        }

        if (number == -1) {
            return false;
        }

        if (number < 1 || number > (ssize_t) results.size()) {
            printf("неверное значение\n");
            continue;
        }

        std::vector<Record>::iterator it = results[number - 1];
        Record &current = *it;
        printrec(current, number);

        while (true) {
            fgets(buffer, sizeof(buffer), stdin);
            std::string command = buffer;
            command.resize(command.size() - 1);
            if (command == "set") {
                subsubsearch(buffer, sizeof(buffer), current);

                printrec(current, number);
                break;
            } else if (command == "Q" || command == "Quit" || command == "q" ||
                       command == "quit") {
                printf("Выход из режима поиска...\n");
                return false;
            } else
                continue;
        }
    }
}

void search(const std::string &nameB, std::vector<Record> &records) {
    printf("поиск...\n");

    std::vector<std::vector<Record>::iterator> results;
    for (std::vector<Record>::iterator it = records.begin(); it != records.end();
         it++) {
        Record &v = *it;

        for (SubRecord &a : v.subrecords) {
            if (a.data.find(nameB) != std::string::npos) {
                if (a.name == "NAME" || a.name == "HEDR" || a.name == "MAST" ||
                    a.name == "STRV" || a.name == "FNAM" || a.name == "DESC" ||
                    a.name == "ANAM" || a.name == "NPCS" || a.name == "ITEX" ||
                    a.name == "PTEX" || a.name == "CVFX" || a.name == "BVFX" ||
                    a.name == "HVFX" || a.name == "AVFX" || a.name == "BNAM" ||
                    a.name == "TNAM" || a.name == "ENAM" || a.name == "SCRI" ||
                    a.name == "CNAM" || a.name == "INAM" || a.name == "RGNN" ||
                    a.name == "XSOL" || a.name == "SNAM" || a.name == "ONAM" ||
                    a.name == "RNAM" || a.name == "DNAM" || a.name == "SCVR" ||
                    a.name == "DATA" || a.name == "KNAM" || a.name == "INFO") {

                    results.push_back(it);

                    printrec(v, results.size());
                }
            }
        }
    }

    subsearch(results);
}

bool tryB(std::vector<Record> &records, std::string filepath) {
    std::filesystem::path path(filepath);
    if (path.extension().string() == ".esm" ||
        std::filesystem::exists(filepath + ".esm")) {
        FILE *mw = fopen(filepath.data(), "rb");
        if (mw == NULL) {
            printf("ошибка\n");
            return false;
        }

        while (!feof(mw)) {
            int size, header, flags, ssize;
            std::string word(4, 0);
            std::vector<SubRecord> subrec;
            std::string sname(4, 0);

            switch (fread((void *) word.data(), 1, 4, mw)) {
                case 4:
                    break;
                default:
                    if (feof(mw))
                        continue;
                    else
                        return false;
            }

            if (fread(&size, 4, 1, mw) != 1) {
                return false;
            }
            if (fread(&header, 4, 1, mw) != 1) {
                return false;
            }
            if (fread(&flags, 4, 1, mw) != 1) {
                return false;
            }

            long a = ftell(mw);
            while (ftell(mw) - a < size) {
                fread((void *) sname.data(), 1, 4, mw);
                if (fread(&ssize, 4, 1, mw) != 1) {
                    return false;
                }
                std::string sdata(ssize, 0);
                fread((void *) sdata.data(), ssize, 1, mw);

                subrec.emplace_back(sname, ssize, sdata);
            }
            records.emplace_back(word, size, header, flags, subrec);
        }

        return true;
    } else if (std::filesystem::is_directory(path) &&
               std::filesystem::exists(filepath)) {

        std::filesystem::directory_entry direct(path);
        bool allLong = true, all4 = true;
        std::filesystem::directory_iterator directory_iterator(direct);

        for (const std::filesystem::directory_entry &child : directory_iterator) {
            if (!child.is_directory())
                continue;
            size_t size = child.path().filename().stem().string().size();
            if (size <= 4)
                allLong = false;
            if (size != 4)
                all4 = false;

            if (!allLong && !all4)
                break;
        }

        if (all4) {
            printf("Primary file tree format detected.\n");
            if (loadFromFileTree(records, path))
                return true;
        }
        if (allLong) {
            printf("Human-readable file tree format detected.\n");
            if (loadFromFileTreeHuman(records, path))
                return true;
        }
    }
    return false;
}

int main(int argc, char **argv) {
    char *buffer = new char[128];
    bool res = false;
    std::vector<Record> records;

    if (argc == 2) {
        std::string filepath = argv[1];
        res = tryB(records, filepath);
    }

    while (!res) {
        printf("введите путь к файлу или папке\n");
        fgets(buffer, 128, stdin);
        std::string filepath = buffer;
        filepath.resize(filepath.size() - 1);
        std::filesystem::path path(filepath);
        res = tryB(records, filepath);
        if (!res) {
            printf("путь введен неверно\n");
        }
    }

    printf("%zu records loaded.\n", records.size());

    std::string command;
    while (true) {
        fgets(buffer, 128, stdin);
        command = buffer;
        command.resize(command.size() - 1);
        if (command == "search") {
            fgets(buffer, 128, stdin);
            std::string nameC = buffer;
            nameC.resize(nameC.size() - 1);
            search(nameC, records);
        } else if (command == "unbox") {
            printf("Введите путь для распаковки\n");

            fgets(buffer, 128, stdin);
            std::string nameC = buffer;
            nameC.resize(nameC.size() - 1);

            std::filesystem::path rootExtractPath(".");
            rootExtractPath /= nameC;

            printf("Распаковка в %s...\n", rootExtractPath.string().data());

            if (!std::filesystem::exists(rootExtractPath))
                std::filesystem::create_directory(rootExtractPath);

            int i = 0;
            for (Record &v : records) {
                std::filesystem::path recordPath = rootExtractPath / v.name;
                if (!std::filesystem::exists(recordPath))
                    std::filesystem::create_directory(recordPath);

                std::filesystem::path recordDataPath = recordPath / std::to_string(i);
                std::string path = recordDataPath.string();

                FILE *rar = fopen(path.data(), "w");
                if (!rar) {
                    printf("Ошибка: невозможно открыть %s", path.data());
                    break;
                }

                fprintf(rar, "%s %d %d %d\n", v.name.data(), v.size, v.header, v.flags);
                for (SubRecord &a : v.subrecords) {
                    fprintf(rar, "%s %d ", a.name.data(), a.size);
                    fwrite(a.data.data(), 1, a.size, rar);
                    fprintf(rar, "\n");
                }
                fclose(rar);
                i++;
            }
            printf("Распаковано\n");
        } else if (command == "humanize") {
            printf("Введите путь для распаковки в человекочитаемый формат\n");

            fgets(buffer, 128, stdin);
            std::string nameC = buffer;
            nameC.resize(nameC.size() - 1);

            std::filesystem::path rootExtractPath(".");
            rootExtractPath /= nameC;

            printf("Распаковка в %s...\n", rootExtractPath.string().data());

            if (!std::filesystem::exists(rootExtractPath))
                std::filesystem::create_directory(rootExtractPath);

            int i = 0;
            for (Record &v : records) {
                std::filesystem::path recordPath = rootExtractPath / (std::to_string(i) + v.name);
                if (!std::filesystem::exists(recordPath))
                    std::filesystem::create_directory(recordPath);

                std::filesystem::path recordDataPath = recordPath / (v.name + ".txt");
                std::string path = recordDataPath.string();

                FILE *output = fopen(path.data(), "w");
                if (!output) {
                    printf("Ошибка: невозможно открыть %s", path.data());
                    break;
                }

                fprintf(output, "%s\n%d\n%d\n%d", v.name.data(), v.size, v.header, v.flags);

                fmem mem;
                int k = 0;
                for (SubRecord &a : v.subrecords) {
                    fmem_init(&mem);

                    FILE *file = fmem_open(&mem, "rw+");
                    fwrite(a.data.data(), 1, a.size, file);
                    fflush(file);
                    fseek(file, 0, SEEK_SET);

                    human_unpack(v, a, recordPath, k, file, output);

                    fclose(file);
                    fmem_term(&mem);
                    k++;
                }

                fclose(output);
                i++;

                if (i % 1000 == 0) {
                    printf("%d/%zu обработано.\n", i, records.size());
                }
            }

            printf("Распаковано\n");
        } else if (command == "save") {
            printf("Введите путь сохранения\n");

            fgets(buffer, 128, stdin);
            std::string nameC = buffer;
            nameC.resize(nameC.size() - 1);

            printf("Сохранение...\n");
            FILE *esm = fopen(nameC.data(), "w");
            if (!esm) {
                printf("Failed to open %s", nameC.data());
                break;
            }

            for (Record &v : records) {
                fwrite(v.name.data(), 1, 4, esm);
                fwrite(&v.size, 4, 1, esm);
                fwrite(&v.header, 4, 1, esm);
                fwrite(&v.flags, 4, 1, esm);

                for (SubRecord &a : v.subrecords) {
                    fwrite(a.name.data(), 1, 4, esm);
                    fwrite(&a.size, 4, 1, esm);
                    fwrite(a.data.data(), 1, a.size, esm);
                }
            }

            fclose(esm);
            printf("Сохранено\n");
        } else if (command == "Q" || command == "Quit" || command == "q" ||
                   command == "quit") {
            printf("Выход...\n");
            break;
        } else {
            printf("Неизвестная команда: \n");
        }
    }

    delete[] buffer;
    return 0;
}
