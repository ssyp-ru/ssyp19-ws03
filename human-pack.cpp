#include "data.hpp"
#include <iostream>
#include <filesystem>
#include <charconv>
#include "fmem.h"

using namespace std;

void division(FILE *toAdd, FILE *superAdd, int strings) {
    char *buffer = new char[256];
    int stringLength = 0;
    for (int m = 0; m < strings + 1; ++m) {
        int stringCount;
        fscanf(toAdd, "%d:", &stringCount);
        fgets(buffer, 256, toAdd);
        string bufferString = buffer;
        stringLength = bufferString.size();
        if (buffer[0] != '\n') {
            fwrite(buffer, 1, stringLength - 1, superAdd);
        } else {
            fwrite("\0", 1, 1, superAdd);
        }
    }
}

bool human_pack(std::vector<Record> &records, std::filesystem::path flex) {
    fmem mem;
    fmem_init(&mem);

    filesystem::directory_entry rofl(flex);
    filesystem::directory_iterator counter(rofl);
    filesystem::directory_iterator uWu(rofl);
    int c = 0;
    string hh;

    for (const filesystem::directory_entry &child : counter) {
        c++;
    }
    vector<filesystem::directory_entry> directoryVector(c);
    for (const filesystem::directory_entry &child : uWu) {
        int result;
        string nof = child.path().filename().string();
        from_chars_result tt = from_chars(nof.data(), nof.data() + nof.size(), result);
        if (tt.ec == std::errc()) {
            directoryVector[result] = child;
        }
    }

    for (int i = 0; i < c; i++) {
        filesystem::directory_entry child = directoryVector[i];
        filesystem::directory_iterator subCounter(child);
        filesystem::directory_iterator WuW(child);
        int p = 0;
        string nameOfRecord;
        for (const filesystem::directory_entry &child : subCounter) {
            p++;
        }
        vector<filesystem::directory_entry> subDirectoryVector(p);
        for (const filesystem::directory_entry &child : WuW) {
            if (!child.is_directory()) {
                subDirectoryVector[0] = child;
            } else {
                int result;
                string nof = child.path().filename().string();
                from_chars_result tt = from_chars(nof.data(), nof.data() + nof.size(), result);
                if (tt.ec == std::errc()) {
                    subDirectoryVector[result + 1] = child;
                }
            }
        }

        int sizeOfRecord;
        int header;
        int flags;
        {
            filesystem::directory_entry subchild = subDirectoryVector[0];
            std::string subchildPath = subchild.path().string();
            FILE *file = fopen(subchildPath.c_str(), "r");
            if (!file) {
                printf("Failed to open %s", subchildPath.c_str());
                return false;
            }

            char nameOfRecordArr[5];
            fscanf(file, "%4s\n%d\n%d\n%d", nameOfRecordArr, &sizeOfRecord, &header, &flags);
            nameOfRecord = nameOfRecordArr;
            fclose(file);
        }
        std::vector<SubRecord> subrecords;

        for (int j = 1; j < p; j++) {
            filesystem::directory_entry subchild = subDirectoryVector[j];
            filesystem::directory_iterator newDirectoryIterator(subchild);
            for (const filesystem::directory_entry filechild : newDirectoryIterator) {
                std::string filechildPath = filechild.path().string();
                FILE *file = fopen(filechildPath.c_str(), "r");
                if (!file) {
                    printf("Failed to open %s", filechildPath.c_str());
                    return false;
                }

                string nameOfSubRecord = filechild.path().filename().string();
                char subrecordName[5];
                int subRecordSize;
                fscanf(file, "%4s\n%d", subrecordName, &subRecordSize);
                FILE *input = fmem_open(&mem, "w+");
                fwrite(subrecordName, 1, 4, input);
                fwrite(&subRecordSize, 4, 1, input);
                string nameOfSubRecordString = subrecordName;
                if (nameOfSubRecordString == "HEDR") {
                    float ver;
                    int Unkn;
                    int NuRe;
                    int strings;
                    fscanf(file, "\n%f\n%d\n", &ver, &Unkn);
                    fwrite(&ver, 4, 1, input);
                    fwrite(&Unkn, 4, 1, input);
                    fscanf(file, "$%d", &strings);
                    division(file, input, strings);
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                    fscanf(file, "\n%d", &NuRe);
                    fwrite(&NuRe, 4, 1, input);
                } else if (nameOfSubRecordString == "NAME") {
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                } else if (nameOfSubRecordString == "STRV") {
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                } else if (nameOfSubRecordString == "INTV" &&
                           (nameOfRecord != "LEVI" && nameOfRecord != "LEVC") ||
                           (nameOfSubRecordString == "INDX" &&
                            (nameOfRecord == "SKIL" || nameOfRecord == "MGEF" ||
                             nameOfRecord == "LEVC" || nameOfRecord == "LEVI" ||
                             nameOfRecord == "FADT" || nameOfRecord == "CELL")) ||
                           nameOfSubRecordString == "NAM9" ||
                           nameOfSubRecordString == "FLAG" || nameOfSubRecordString == "NAM0" ||
                           nameOfSubRecordString == "NAM5" || nameOfRecord == "CLDT" ||
                           nameOfSubRecordString == "FRMR" || nameOfSubRecordString == "DELE" ||
                           nameOfSubRecordString == "SPDT" ||
                           (nameOfSubRecordString == "NPDT" && nameOfRecord == "CREA") ||
                           (nameOfSubRecordString == "DATA" &&
                            (nameOfRecord == "CELL" || nameOfRecord == "SNDG" ||
                             nameOfRecord == "PGRD"))) {
                    int intvInt;
                    for (int l = 0; l < subRecordSize / 4; ++l) {
                        fscanf(file, "\n%d", &intvInt);
                        fwrite(&intvInt, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "FLTV" || nameOfSubRecordString == "DODT" ||
                           nameOfSubRecordString == "WHGT" ||
                           (nameOfSubRecordString == "CNDT" && nameOfRecord == "CONT") ||
                           nameOfSubRecordString == "XSCL" ||
                           (nameOfSubRecordString == "DATA" && nameOfRecord == "CELL")) {
                    float FlVa;
                    for (int l = 0; l < subRecordSize / 4; ++l) {
                        fscanf(file, "\n%f", &FlVa);
                        fwrite(&FlVa, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "NPCS" || nameOfSubRecordString == "FNAM" ||
                           nameOfSubRecordString == "ANAM" ||
                           nameOfSubRecordString == "DESC" || nameOfSubRecordString == "ITEX" ||
                           nameOfSubRecordString == "PTEX" ||
                           nameOfSubRecordString == "CVFX" || nameOfSubRecordString == "BVFX" ||
                           nameOfSubRecordString == "HVFX" ||
                           nameOfSubRecordString == "AVFX" || nameOfSubRecordString == "CSND" ||
                           nameOfSubRecordString == "BSND" ||
                           nameOfSubRecordString == "HSND" || nameOfSubRecordString == "ASND" ||
                           (nameOfSubRecordString == "SCVR" && nameOfRecord == "SCPT") ||
                           nameOfSubRecordString == "SCDT" || nameOfSubRecordString == "SCTX" ||
                           nameOfSubRecordString == "BNAM" ||
                           nameOfSubRecordString == "TNAM" ||
                           (nameOfSubRecordString == "DATA" && nameOfRecord == "LTEX") ||
                           nameOfSubRecordString == "MODL" || nameOfSubRecordString == "SCIP" ||
                           nameOfSubRecordString == "SCRI" ||
                           (nameOfSubRecordString == "ENAM" &&
                            (nameOfRecord == "WEAP" || nameOfRecord == "ARMO" ||
                             nameOfRecord == "CLOT" ||
                             nameOfRecord == "BOOK")) ||
                           nameOfSubRecordString == "SCPT" ||
                           nameOfSubRecordString == "KNAM" || nameOfSubRecordString == "DNAM" ||
                           nameOfSubRecordString == "TEXT" ||
                           (nameOfSubRecordString == "CNAM" &&
                            (nameOfRecord == "NPC_" || nameOfRecord == "CREA" ||
                             nameOfRecord == "ARMO" || nameOfRecord == "CLOT" ||
                             nameOfRecord == "LEVC" || nameOfRecord == "SNDG")) ||
                           nameOfSubRecordString == "INAM" ||
                           (nameOfSubRecordString == "CNDT" && nameOfRecord == "NPC_" ||
                            nameOfRecord == "INFO") ||
                           nameOfSubRecordString == "RGNN" ||
                           nameOfSubRecordString == "XSOL" ||
                           (nameOfSubRecordString == "SNAM" && nameOfRecord != "REGN") ||
                           nameOfSubRecordString == "PNAM" ||
                           (nameOfSubRecordString == "NNAM" && nameOfRecord == "INFO") ||
                           nameOfSubRecordString == "ONAM" ||
                           (nameOfSubRecordString == "RNAM" &&
                            (nameOfRecord == "INFO" || nameOfRecord == "FACT" ||
                             nameOfRecord == "NPC_"))) {
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                } else if (nameOfSubRecordString == "RADT") {
                    int CLDA[30];
                    float UwU[4];
                    int wUwU;
                    for (int p = 0; p < 30; p++) {
                        fscanf(file, "\n%d", &CLDA[p]);
                        fwrite(&CLDA[p], 4, 1, input);
                    }
                    for (int p = 0; p < 4; p++) {
                        fscanf(file, "\n%f", &UwU[p]);
                        fwrite(&UwU[p], 4, 1, input);
                    }
                    fscanf(file, "\n%d", &wUwU);
                    fwrite(&wUwU, 4, 1, input);
                } else if (nameOfSubRecordString == "DATA" && nameOfRecord != "DIAL") {
                    if (nameOfRecord == "SOUN") {
                        char Volume;
                        char MinRange;
                        char MaxRange;

                        fscanf(file, "\n%c\n%c\n%c",
                               &Volume, &MinRange, &MaxRange);
                        fwrite(&Volume, 1, 1, input);
                        fwrite(&MinRange, 1, 1, input);
                        fwrite(&MaxRange, 1, 1, input);
                    }
                    if (nameOfRecord == "LEVI" || nameOfRecord == "LEVC" ||
                        nameOfRecord == "LAND") {
                        int firstInt;
                        fscanf(file, "\n%d", &firstInt);
                        fwrite(&firstInt, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "SKDT") {
                    int intvInt1, intvInt2;
                    float mas[4];
                    fscanf(file, "\n%d\n%d", &intvInt1, &intvInt2);
                    for (int p = 0; p < 4; p++) {

                        fscanf(file, "\n%f", &mas[p]);
                    }
                    fwrite(&intvInt1, 4, 1, input);
                    fwrite(&intvInt2, 4, 1, input);
                    fwrite(mas, 4, 4, input);
                } else if (nameOfSubRecordString == "MEDT") {
                    int SpSc;
                    float EfDa;
                    fscanf(file, "\n%d", &SpSc);
                    fwrite(&SpSc, 4, 1, input);
                    fscanf(file, "\n%f", &EfDa);
                    fwrite(&EfDa, 4, 1, input);
                    for (int p = 0; p < 4; p++) {
                        fscanf(file, "\n%d", &SpSc);
                        fwrite(&SpSc, 4, 1, input);
                    }
                    for (int p = 0; p < 3; p++) {
                        fscanf(file, "\n%f", &EfDa);
                        fwrite(&EfDa, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "SCHD") {
                    int jojo;
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                    for (int p = 0; p < 5; p++) {
                        fscanf(file, "\n%d", &jojo);
                        fwrite(&jojo, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "SKDT" ||
                           (nameOfSubRecordString == "CNAM" && (nameOfRecord == "REGN" || nameOfRecord == "CELL")) ||
                           (nameOfSubRecordString == "SNAM" && nameOfRecord == "REGN") ||
                           nameOfSubRecordString == "WEAT" || nameOfSubRecordString == "BYDT" ||
                           (nameOfSubRecordString == "NNAM" && nameOfRecord != "INFO") ||
                           nameOfSubRecordString == "VNML" ||
                           (nameOfSubRecordString == "DATA" && nameOfRecord == "DIAL")) {
                    char mas[subRecordSize];
                    for (int p = 0; p < subRecordSize; p++) {
                        fscanf(file, "\n%c", &mas[p]);
                        fwrite(&mas[p], 1, 1, input);
                    }
                } else if (nameOfSubRecordString == "MCDT") {
                    float intvInt1;
                    int mas[2];
                    fscanf(file, "\n%f", &intvInt1);
                    fwrite(&intvInt1, 4, 1, input);
                    for (int p = 0; p < 2; ++p) {
                        fscanf(file, "\n%d", &mas[p]);
                        fwrite(&mas[p], 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "WPDT") {
                    float firstFloat;
                    int firstInt;
                    short firstShorts;
                    char firstByte;
                    fscanf(file, "\n%f", &firstFloat);
                    fwrite(&firstFloat, 4, 1, input);
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                    for (int p = 0; p < 2; p++) {
                        fscanf(file, "\n%hd", &firstShorts);
                        fwrite(&firstShorts, 2, 1, input);
                    }
                    for (int p = 0; p < 2; p++) {
                        fscanf(file, "\n%f", &firstFloat);
                        fwrite(&firstFloat, 4, 1, input);
                    }
                    fscanf(file, "\n%hd", &firstShorts);
                    fwrite(&firstShorts, 2, 1, input);
                    for (int p = 0; p < 6; ++p) {
                        fscanf(file, "\n%c", &firstByte);
                        fwrite(&firstByte, 1, 1, input);
                    }
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                } else if (nameOfSubRecordString == "NPCO") {
                    int intvInt1;
                    char mas[32];
                    fscanf(file, "%d", &intvInt1);
                    fwrite(&intvInt1, 4, 1, input);
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                } else if (nameOfSubRecordString == "LHDT") {
                    int intvInt1;
                    float Fl;
                    char b;
                    fscanf(file, "\n%f", &Fl);
                    fwrite(&Fl, 4, 1, input);
                    for (int p = 0; p < 3; ++p) {
                        fscanf(file, "\n%d", &intvInt1);
                        fwrite(&intvInt1, 4, 1, input);
                    }
                    for (int p = 0; p < 4; ++p) {
                        fscanf(file, "\n%c", &b);
                        fwrite(&b, 1, 1, input);
                    }
                    fscanf(file, "\n%d", &intvInt1);
                    fwrite(&intvInt1, 4, 1, input);
                } else if (nameOfSubRecordString == "ENDT") {
                    int firstInt;
                    for (int p = 0; p < 4; ++p) {
                        fscanf(file, "\n%d", &firstInt);
                        fwrite(&firstInt, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "ENAM") {
                    if (nameOfRecord == "SPEL" || nameOfRecord == "ENCH" ||
                        nameOfRecord == "ALCH") {
                        int firstInt;
                        short firstsho;
                        char firby;
                        fscanf(file, "\n%hd", &firstsho);
                        fwrite(&firstsho, 2, 1, input);
                        for (int p = 0; p < 2; ++p) {
                            fscanf(file, "\n%c", &firby);
                            fwrite(&firby, 1, 1, input);
                        }
                        for (int p = 0; p < 5; ++p) {
                            fscanf(file, "\n%d", &firstInt);
                            fwrite(&firstInt, 4, 1, input);
                        }
                    }
                } else if (nameOfSubRecordString == "NPDT" && nameOfRecord == "NPC_") {
                    int firstInt;
                    short firstsho;
                    char firby;
                    fscanf(file, "\n%hd", &firstsho);
                    fwrite(&firstsho, 2, 1, input);
                    if (subRecordSize == 52) {
                        for (int p = 0; p < 36; ++p) {
                            fscanf(file, "\n%c", &firby);
                            fwrite(&firby, 1, 1, input);
                        }
                        for (int p = 0; p < 3; ++p) {
                            fscanf(file, "\n%hd", &firstsho);
                            fwrite(&firstsho, 2, 1, input);
                        }
                        for (int p = 0; p < 4; ++p) {
                            fscanf(file, "\n%c", &firby);
                            fwrite(&firby, 1, 1, input);
                        }
                    }
                    if (subRecordSize == 12) {
                        for (int p = 0; p < 6; ++p) {
                            fscanf(file, "\n%c", &firby);
                            fwrite(&firby, 1, 1, input);
                        }
                    }
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                } else if (nameOfSubRecordString == "AIDT") {
                    int firstInt;
                    char firby;
                    for (int p = 0; p < 8; ++p) {
                        fscanf(file, "\n%c", &firby);
                        fwrite(&firby, 1, 1, input);
                    }
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                } else if (nameOfSubRecordString == "AI_W") {
                    short fisho;
                    char firby;
                    char Idle[8];
                    for (int p = 0; p < 2; ++p) {
                        fscanf(file, "\n%hd", &fisho);
                        fwrite(&fisho, 2, 1, input);
                    }
                    fscanf(file, "\n%c", &firby);
                    fwrite(&firby, 1, 1, input);
                    for (int p = 0; p < 8; p++) {
                        fscanf(file, "\n%c", &Idle[p]);
                        fwrite(&Idle[p], 1, 1, input);
                    }
                    fscanf(file, "\n%c", &firby);
                    fwrite(&firby, 1, 1, input);
                } else if (nameOfSubRecordString == "AI_T") {
                    int a;
                    float b;
                    for (int p = 0; p < 3; p++) {
                        fscanf(file, "\n%f", &b);
                        fwrite(&b, 4, 1, input);
                    }
                    fscanf(file, "\n%d", &a);
                    fwrite(&a, 4, 1, input);
                } else if (nameOfSubRecordString == "AI_F" || nameOfSubRecordString == "AI_E") {
                    short a;
                    float b;
                    for (int p = 0; p < 3; p++) {
                        fscanf(file, "\n%f", &b);
                        fwrite(&b, 4, 1, input);
                    }
                    fscanf(file, "\n%hd", &a);
                    fwrite(&a, 2, 1, input);
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                    fscanf(file, "\n%hd", &a);
                    fwrite(&a, 2, 1, input);
                } else if (nameOfSubRecordString == "AI_A") {
                    char firby;
                    int strings;
                    fscanf(file, "\n$%d", &strings);
                    division(file, input, strings);
                    fscanf(file, "\n%c", &firby);
                    fwrite(&firby, 1, 1, input);
                } else if (nameOfSubRecordString == "DODT") {
                    float b;
                    for (int p = 0; p < 6; p++) {
                        fscanf(file, "\n%f", &b);
                        fwrite(&b, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "XSCL") {
                    float b;
                    fscanf(file, "\n%f", &b);
                    fwrite(&b, 4, 1, input);
                } else if (nameOfSubRecordString == "AODT") {
                    int firstInt;
                    float firstfl;
                    fscanf(file, "\n%d\n%f", &firstInt, &firstfl);
                    fwrite(&firstInt, 4, 1, input);
                    fwrite(&firstfl, 4, 1, input);
                    for (int p = 0; p < 4; ++p) {
                        fscanf(file, "\n%d", &firstInt);
                        fwrite(&firstInt, 4, 1, input);
                    }
                } else if ((nameOfSubRecordString == "INDX" &&
                            (nameOfRecord == "ARMO" || nameOfRecord == "CLOT")) ||
                           nameOfSubRecordString == "UNAM") {
                    char firstByte;
                    for (int p = 0; p < subRecordSize; p++) {
                        fscanf(file, "\n%c", &firstByte);
                        fwrite(&firstByte, 1, 1, input);
                    }
                } else if (nameOfSubRecordString == "CTDT") {
                    int firstInt;
                    float firstFloat;
                    short firstShort;
                    fscanf(file, "- %d\n%f", &firstInt, &firstFloat);
                    fwrite(&firstInt, 4, 1, input);
                    fwrite(&firstFloat, 4, 1, input);
                    for (int p = 0; p < 2; ++p) {
                        fscanf(file, "\n%hd", &firstShort);
                        fwrite(&firstShort, 2, 1, input);
                    }
                } else if (nameOfSubRecordString == "RIDT") {
                    int firstInt;
                    float firstfl;
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstfl, 4, 1, input);
                    for (int p = 0; p < 2; ++p) {
                        fscanf(file, "\n%d", &firstInt);
                        fwrite(&firstInt, 4, 1, input);
                    }
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstfl, 4, 1, input);
                } else if (nameOfSubRecordString == "AADT") {
                    int firstInt;
                    float firstfl;
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                    for (int p = 0; p < 2; ++p) {
                        fscanf(file, "\n%f", &firstfl);
                        fwrite(&firstfl, 4, 1, input);
                    }
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                } else if (nameOfSubRecordString == "LKDT" || nameOfSubRecordString == "PBDT") {
                    int firstInt;
                    float firstfl;
                    fscanf(file, "\n%d", &firstInt);
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstInt, 4, 1, input);
                    fwrite(&firstfl, 4, 1, input);
                    fscanf(file, "\n%d", &firstInt);
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstInt, 4, 1, input);
                    fwrite(&firstfl, 4, 1, input);
                } else if (nameOfSubRecordString == "BKDT") {
                    int firstInt;
                    float firstfl;
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstfl, 4, 1, input);
                    for (int p = 0; p < 4; p++) {
                        fscanf(file, "\n%d", &firstInt);
                        fwrite(&firstInt, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "IRDT") {
                    int firstInt;
                    int mas[4];
                    float firstfl;
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstfl, 4, 1, input);
                    fscanf(file, "\n%d", &firstInt);
                    fwrite(&firstInt, 4, 1, input);
                    for (int k = 0; k < 3; k++) {
                        fscanf(file, "\n%d", mas);
                        fwrite(mas, 4, 4, input);
                    }
                } else if (nameOfSubRecordString == "ALDT") {
                    int firstInt;
                    float firstfl;
                    fscanf(file, "\n%f", &firstfl);
                    fwrite(&firstfl, 4, 1, input);
                    for (int p = 0; p < 2; p++) {
                        fscanf(file, "\n%d", &firstInt);
                        fwrite(&firstInt, 4, 1, input);
                    }
                } else if (nameOfSubRecordString == "INTV") {
                    short CLDA;
                    fscanf(file, "\n%hd", &CLDA);
                    fwrite(&CLDA, 2, 1, input);
                } else if (nameOfSubRecordString == "AMBI") {
                    int intvInt1;
                    float Fl;
                    for (int p = 0; p < 3; ++p) {
                        fscanf(file, "\n%d", &intvInt1);
                        fwrite(&intvInt1, 4, 1, input);
                    }
                    fscanf(file, "\n%f", &Fl);
                    fwrite(&Fl, 4, 1, input);
                } else if (nameOfSubRecordString == "VHGT") {
                    float fl;
                    char b;
                    short sh;
                    int con = 65 * 65 + 1;
                    fscanf(file, "\n%f", &fl);
                    fwrite(&fl, 4, 1, input);
                    for (int p = 0; p < con; p++) {
                        fscanf(file, "\n%c", &b);
                        fwrite(&b, 1, 1, input);
                    }
                    fscanf(file, "\n%hd", &sh);
                    fwrite(&sh, 2, 1, input);
                } else if (nameOfSubRecordString == "WNAM" || nameOfSubRecordString == "VTEX" ||
                           nameOfSubRecordString == "QSTN" || nameOfSubRecordString == "QSTF" ||
                           nameOfSubRecordString == "QSTR") {
                    char b;
                    for (int p = 0; p < subRecordSize; p++) {
                        fscanf(file, "\n%c", &b);
                        fwrite(&b, 1, 1, input);
                    }
                } else if (nameOfSubRecordString == "VCLR") {
                    char b;
                    for (int p = 0; p < subRecordSize; ++p) {
                        fscanf(file, "\n%c", &b);
                        fwrite(&b, 1, 1, input);
                    }
                } else if (nameOfSubRecordString == "SCVR") {
                    char b;
                    short s;
                    for (int p = 0; p < 2; ++p) {
                        fscanf(file, "\n%c", &b);
                        fwrite(&b, 1, 1, input);
                    }
                    fscanf(file, "\n%hd", &s);
                    fwrite(&s, 2, 1, input);
                    for (int p = 0; p < subRecordSize - 4; ++p) {
                        fscanf(file, "\n%c", &b);
                        fwrite(&b, 1, 1, input);
                    }
                } else {
                    char DeStr[subRecordSize + 1];
                    DeStr[subRecordSize] = 0;
                    fscanf(file, "\n%.*s", subRecordSize, DeStr);
                    fwrite(DeStr, 1, subRecordSize, input);
                }
                fclose(file);

                char dataStorage[subRecordSize + 1];

                fseek(input, 0, SEEK_SET);
                fread(dataStorage, 1, subRecordSize, input);

                std::string data(dataStorage, subRecordSize);
                subrecords.emplace_back(nameOfSubRecordString, subRecordSize, data);

                fclose(input);
            }
        }

        Record &record = records.emplace_back(nameOfRecord, sizeOfRecord, header, flags, subrecords);
    }

    fmem_term(&mem);
    return true;
}
