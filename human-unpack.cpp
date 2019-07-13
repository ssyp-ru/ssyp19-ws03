#include "data.hpp"
#include <filesystem>
#include <cassert>

using namespace std;

void division(FILE *toAdd, char added[], int lenght) {
    int stringsCount = 0;
    int iter = 0;

    for (int i = 0; i < lenght; i++) {
        if (added[i] == '\0') {
            stringsCount++;
        }
    }
    fprintf(toAdd, "\n$%d\n", stringsCount);
    for (int j = 0; j < stringsCount + 1; ++j) {
        fprintf(toAdd, "%d:", j);
        iter += fprintf(toAdd, "%s", added + iter);
        fprintf(toAdd, "\n");
    }
}

void generateBitmapImage(unsigned char *image, int height, int width, const char *const imageFileName);

bool BmpFile_SaveScreenshot(
        const byte *pBits,
        const char *const sFileName,
        int width, int height) {
    assert(pBits != NULL);
    assert(sFileName != NULL);

    const int bytesPerPixel = 3; /// red, green, blue
    unsigned char image[height][width][bytesPerPixel];

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j][2] = (unsigned char) (*(pBits++)); ///red
            image[i][j][1] = (unsigned char) (*(pBits++)); ///green
            image[i][j][0] = (unsigned char) (*(pBits++)); ///blue
        }
    }

    generateBitmapImage((unsigned char *) image, height, width, sFileName);
    return true;
}

bool human_unpack(Record &nowRecord, SubRecord &nowSubRecord, filesystem::path recordPath, int k, FILE *file, FILE *output) {
    filesystem::path subsuperPath = recordPath / (to_string(k) + nowSubRecord.name);

    //create *SubRecord name* folder if they are existing
    create_directory(subsuperPath);
    string subfilePath = (subsuperPath / (nowSubRecord.name + ".txt")).string();
    FILE *suboutput = fopen(subfilePath.c_str(), "w");
    if (!suboutput) {
        printf("Failed to open %s", subfilePath.c_str());
        return false;
    }

    if (nowSubRecord.name == "HEDR") {
        float ver;
        int Unkn;
        char CoNa[32];
        char FiDe[256];
        int NuRe;
        if (fread(&ver, 4, 1, file) != 1) {
            fclose(suboutput);
            return false;
        }
        if (fread(&Unkn, 4, 1, file) != 1) {
            fclose(suboutput);
            return false;
        }
        if (fread(CoNa, 1, 32, file) != 32) {
            fclose(suboutput);
            return false;
        }
        if (fread(FiDe, 1, 256, file) != 256) {
            fclose(suboutput);
            return false;
        }
        if (fread(&NuRe, 4, 1, file) != 1) {
            fclose(suboutput);
            return false;
        }
        fprintf(suboutput, "%s\n%d\n%f\n%d", "HEDR", nowSubRecord.size, ver, Unkn);
        division(suboutput, CoNa, 32);
        division(suboutput, FiDe, 256);
        fprintf(suboutput, "\n%d", NuRe);
    } else if (nowSubRecord.name == "NAME") {
        char IDStr[nowSubRecord.size + 1];
        IDStr[nowSubRecord.size] = 0;
        if (fread(IDStr, 1, nowSubRecord.size, file) != nowSubRecord.size) {
            fclose(suboutput);
            return false;
        }
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        division(suboutput, IDStr, nowSubRecord.size);
    } else if (nowSubRecord.name == "STRV") {
        char IDStr[nowSubRecord.size];
        IDStr[nowSubRecord.size] = 0;
        if (fread(IDStr, 1, nowSubRecord.size, file) != nowSubRecord.size) {
            fclose(suboutput);
            return false;
        }
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        division(suboutput, IDStr, nowSubRecord.size);
    } else if (nowSubRecord.name == "INTV" && (nowRecord.name != "LEVI" && nowRecord.name != "LEVC") ||
               (nowSubRecord.name == "INDX" &&
                (nowRecord.name == "SKIL" || nowRecord.name == "MGEF" ||
                 nowRecord.name == "LEVC" || nowRecord.name == "LEVI" || nowRecord.name == "CELL")) ||
               nowSubRecord.name == "NAM9" ||
               nowSubRecord.name == "FLAG" || nowSubRecord.name == "NAM0" || nowSubRecord.name == "NAM5" ||
               nowSubRecord.name == "FRMR" || nowSubRecord.name == "DELE" ||
               nowSubRecord.name == "SPDT" || (nowSubRecord.name == "NPDT" && nowRecord.name == "CREA") ||
               (nowSubRecord.name == "DATA" &&
                (nowRecord.name == "CELL" || nowRecord.name == "SNDG" || nowRecord.name == "PGRD"))) {
        int intvInt;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int l = 0; l < nowSubRecord.size / 4; ++l) {
            if (fread(&intvInt, 4, 1, file) != 1) {
                fclose(suboutput);
                return false;
            }
            fprintf(suboutput, "\n%d", intvInt);
        }
    } else if (nowSubRecord.name == "FLTV" || nowSubRecord.name == "DODT" || nowSubRecord.name == "WHGT" ||
               (nowSubRecord.name == "CNDT" && nowRecord.name == "CONT") || nowSubRecord.name == "XSCL" ||
               (nowSubRecord.name == "DATA" && nowRecord.name == "CELL")) {
        float FlVa;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int l = 0; l < nowSubRecord.size / 4; ++l) {
            if (fread(&FlVa, 4, 1, file) != 1) {
                fclose(suboutput);
                return false;
            }
            fprintf(suboutput, "\n%f", FlVa);
        }
    } else if (nowSubRecord.name == "CLDT") {
        int CLDA[nowSubRecord.size / 4];
        fread(&CLDA, 1, nowSubRecord.size, file);
        fprintf(suboutput, "%s\n%d", "CLDT", nowSubRecord.size);
        for (int i = 0; i < nowSubRecord.size / 4; i++) {
            fprintf(suboutput, "\n%d", CLDA[i]);
        }
    } else if ((nowSubRecord.name == "RNAM" && (nowRecord.name == "FACT" || nowRecord.name == "NPC_")) ||
               nowSubRecord.name == "NPCS" || nowSubRecord.name == "FNAM" || nowSubRecord.name == "ANAM" ||
               nowSubRecord.name == "DESC" || nowSubRecord.name == "ITEX" || nowSubRecord.name == "PTEX" ||
               nowSubRecord.name == "CVFX" || nowSubRecord.name == "BVFX" || nowSubRecord.name == "HVFX" ||
               nowSubRecord.name == "AVFX" || nowSubRecord.name == "CSND" || nowSubRecord.name == "BSND" ||
               nowSubRecord.name == "HSND" || nowSubRecord.name == "ASND" ||
               (nowSubRecord.name == "SCVR" && nowRecord.name == "SCPT") ||
               nowSubRecord.name == "SCDT" || nowSubRecord.name == "SCTX" || nowSubRecord.name == "BNAM" ||
               nowSubRecord.name == "TNAM" || (nowSubRecord.name == "DATA" && nowRecord.name == "LTEX") ||
               nowSubRecord.name == "MODL" || nowSubRecord.name == "SCIP" || nowSubRecord.name == "SCRI" ||
               (nowSubRecord.name == "ENAM" &&
                (nowRecord.name == "WEAP" || nowRecord.name == "ARMO" || nowRecord.name == "CLOT" ||
                 nowRecord.name == "BOOK")) ||
               nowSubRecord.name == "SCPT" ||
               nowSubRecord.name == "KNAM" || nowSubRecord.name == "DNAM" || nowSubRecord.name == "TEXT" ||
               (nowSubRecord.name == "CNAM" && (nowRecord.name == "NPC_" || nowRecord.name == "CREA" ||
                                                nowRecord.name == "ARMO" || nowRecord.name == "CLOT" ||
                                                nowRecord.name == "LEVC" || nowRecord.name == "SNDG")) ||
               nowSubRecord.name == "INAM" ||
               (nowSubRecord.name == "CNDT" && nowRecord.name == "NPC_" || nowRecord.name == "INFO") ||
               nowSubRecord.name == "RGNN" ||
               nowSubRecord.name == "XSOL" || (nowSubRecord.name == "SNAM" && nowRecord.name != "REGN") ||
               nowSubRecord.name == "PNAM" || (nowSubRecord.name == "NNAM" && nowRecord.name == "INFO") ||
               nowSubRecord.name == "ONAM" || (nowSubRecord.name == "RNAM" && nowRecord.name == "INFO")) {
        char DeStr[nowSubRecord.size];
        fread(DeStr, 1, nowSubRecord.size, file);
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        division(suboutput, DeStr, nowSubRecord.size);
    } else if (nowSubRecord.name == "FADT") {
        int CLDA[nowSubRecord.size / 4];
        fread(&CLDA, 1, nowSubRecord.size, file);
        fprintf(suboutput, "%s\n%d", "FADT", nowSubRecord.size);
        for (int p = 0; p < nowSubRecord.size / 4; p++) {
            fprintf(suboutput, "\n%d", CLDA[p]);
        }
    } else if (nowSubRecord.name == "RADT") {
        int CLDA[30];
        float UwU[4];
        int wUwU;
        fprintf(suboutput, "%s\n%d", "RADT", nowSubRecord.size);
        for (int p = 0; p < 30; p++) {
            fread(&CLDA[p], 4, 1, file);
            fprintf(suboutput, "\n%d", CLDA[p]);
        }
        for (int p = 0; p < 4; p++) {
            fread(&UwU[p], 4, 1, file);
            fprintf(suboutput, "\n%f", UwU[p]);
        }
        fread(&wUwU, 4, 1, file);
        fprintf(suboutput, "\n%d", wUwU);
    } else if (nowSubRecord.name == "DATA" && nowRecord.name != "DIAL") {
        if (nowRecord.name == "SOUN") {
            byte Volume;
            byte MinRange;
            byte MaxRange;
            fread(&Volume, 1, 1, file);
            fread(&MinRange, 1, 1, file);
            fread(&MaxRange, 1, 1, file);

            fprintf(suboutput, "%s\n%d\n%c\n%c\n%c", "DATA", nowSubRecord.size, Volume, MinRange, MaxRange);
        }
        if (nowRecord.name == "LEVI" || nowRecord.name == "LEVC" || nowRecord.name == "LAND") {
            int firstInt;
            fread(&firstInt, 4, 1, file);

            fprintf(suboutput, "%s\n%d\n%d", "DATA", nowSubRecord.size, firstInt);
        }
    } else if (nowSubRecord.name == "SKDT") {
        int intvInt1, intvInt2;
        float mas[4];
        fread(&intvInt1, 4, 1, file);
        fread(&intvInt2, 4, 1, file);
        fread(mas, 4, 4, file);
        fprintf(suboutput, "%s\n%d\n%d\n%d", nowSubRecord.name.c_str(), nowSubRecord.size, intvInt1, intvInt2);
        for (int p = 0; p < 4; p++) {

            fprintf(suboutput, "\n%f", mas[p]);
        }
    } else if (nowSubRecord.name == "MEDT") {
        int SpSc;
        float EfDa;
        fread(&SpSc, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%d", nowSubRecord.name.c_str(), nowSubRecord.size, SpSc);
        fread(&EfDa, 4, 1, file);
        fprintf(suboutput, "\n%f", EfDa);
        for (int p = 0; p < 4; p++) {
            fread(&SpSc, 4, 1, file);
            fprintf(suboutput, "\n%d", SpSc);
        }
        for (int p = 0; p < 3; p++) {
            fread(&EfDa, 4, 1, file);
            fprintf(suboutput, "\n%f", EfDa);
        }
    } else if (nowSubRecord.name == "SCHD") {
        char chach[32];
        int jojo;
        fread(chach, 1, 32, file);
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        division(suboutput, chach, 32);
        for (int p = 0; p < 5; p++) {
            fread(&jojo, 4, 1, file);
            fprintf(suboutput, "\n%d", jojo);
        }
    } else if (nowSubRecord.name == "SKDT" ||
               (nowSubRecord.name == "CNAM" && (nowRecord.name == "REGN" || nowRecord.name == "CELL")) ||
               (nowSubRecord.name == "SNAM" && nowRecord.name == "REGN") ||
               nowSubRecord.name == "WEAT" || nowSubRecord.name == "BYDT" ||
               (nowSubRecord.name == "NNAM" && nowRecord.name != "INFO") ||
               nowSubRecord.name == "VNML" || (nowSubRecord.name == "DATA" && nowRecord.name == "DIAL")) {
        byte mas[nowSubRecord.size];
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < nowSubRecord.size; p++) {
            fread(&mas[p], 1, 1, file);
            fprintf(suboutput, "\n%c", mas[p]);
        }
    } else if (nowSubRecord.name == "MCDT") {
        float intvInt1;
        int mas[2];
        fread(&intvInt1, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, intvInt1);
        for (int p = 0; p < 2; ++p) {
            fread(&mas[p], 4, 1, file);
            fprintf(suboutput, "\n%d", mas[p]);
        }
    } else if (nowSubRecord.name == "WPDT") {
        float firstFloat;
        int firstInt;
        short firstShorts;
        byte firstByte;
        fread(&firstFloat, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, firstFloat);
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
        for (int p = 0; p < 2; p++) {
            fread(&firstShorts, 2, 1, file);
            fprintf(suboutput, "\n%hd", firstShorts);
        }
        for (int p = 0; p < 2; p++) {
            fread(&firstFloat, 4, 1, file);
            fprintf(suboutput, "\n%f", firstFloat);
        }
        fread(&firstShorts, 2, 1, file);
        fprintf(suboutput, "\n%hd", firstShorts);
        for (int p = 0; p < 6; ++p) {
            fread(&firstByte, 1, 1, file);
            fprintf(suboutput, "\n%c", firstByte);
        }
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
    } else if (nowSubRecord.name == "NPCO") {
        int intvInt1;
        char mas[32];
        fread(&intvInt1, 4, 1, file);
        fread(mas, 1, 32, file);

        fprintf(suboutput, "%s\n%d\n%d", nowSubRecord.name.c_str(), nowSubRecord.size, intvInt1);
        division(suboutput, mas, 32);
    } else if (nowSubRecord.name == "LHDT") {
        int intvInt1;
        float Fl;
        byte b;
        fread(&Fl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, Fl);
        for (int p = 0; p < 3; ++p) {
            fread(&intvInt1, 4, 1, file);
            fprintf(suboutput, "\n%d", intvInt1);
        }
        for (int p = 0; p < 4; ++p) {
            fread(&b, 1, 1, file);
            fprintf(suboutput, "\n%c", b);
        }
        fread(&intvInt1, 4, 1, file);
        fprintf(suboutput, "\n%d", intvInt1);
    } else if (nowSubRecord.name == "ENDT") {
        int firstInt;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 4; ++p) {
            fread(&firstInt, 4, 1, file);
            fprintf(suboutput, "\n%d", firstInt);
        }
    } else if (nowSubRecord.name == "ENAM") {
        if (nowRecord.name == "SPEL" || nowRecord.name == "ENCH" || nowRecord.name == "ALCH") {
            int firstInt;
            short firstsho;
            byte firby;
            fread(&firstsho, 2, 1, file);
            fprintf(suboutput, "%s\n%d\n%hd", nowSubRecord.name.c_str(), nowSubRecord.size, firstsho);
            for (int p = 0; p < 2; ++p) {
                fread(&firby, 1, 1, file);
                fprintf(suboutput, "\n%c", firby);
            }
            for (int p = 0; p < 5; ++p) {
                fread(&firstInt, 4, 1, file);
                fprintf(suboutput, "\n%d", firstInt);
            }
        }
    } else if (nowSubRecord.name == "NPDT" && nowRecord.name == "NPC_") {
        int firstInt;
        short firstsho;
        byte firby;
        fread(&firstsho, 2, 1, file);
        fprintf(suboutput, "%s\n%d\n%hd", nowSubRecord.name.c_str(), nowSubRecord.size, firstsho);
        if (nowSubRecord.size == 52) {
            for (int p = 0; p < 36; ++p) {
                fread(&firby, 1, 1, file);
                fprintf(suboutput, "\n%c", firby);
            }
            for (int p = 0; p < 3; ++p) {
                fread(&firstsho, 2, 1, file);
                fprintf(suboutput, "\n%hd", firstsho);
            }
            for (int p = 0; p < 4; ++p) {
                fread(&firby, 1, 1, file);
                fprintf(suboutput, "\n%c", firby);
            }
        }
        if (nowSubRecord.size == 12) {
            for (int p = 0; p < 6; ++p) {
                fread(&firby, 1, 1, file);
                fprintf(suboutput, "\n%c", firby);
            }
        }
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
    } else if (nowSubRecord.name == "AIDT") {
        int firstInt;
        byte firby;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 8; ++p) {
            fread(&firby, 1, 1, file);
            fprintf(suboutput, "\n%c", firby);
        }
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
    } else if (nowSubRecord.name == "AI_W") {
        short fisho;
        byte firby;
        byte Idle[8];
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 2; ++p) {
            fread(&fisho, 2, 1, file);
            fprintf(suboutput, "\n%hd", fisho);
        }
        fread(&firby, 1, 1, file);
        fprintf(suboutput, "\n%c", firby);
        for (int p = 0; p < 8; p++) {
            fread(&Idle[p], 1, 1, file);
            fprintf(suboutput, "\n%c", Idle[p]);
        }
        fread(&firby, 1, 1, file);
        fprintf(suboutput, "\n%c", firby);
    } else if (nowSubRecord.name == "AI_T") {
        int a;
        float b;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 3; p++) {
            fread(&b, 4, 1, file);
            fprintf(suboutput, "\n%f", b);
        }
        fread(&a, 4, 1, file);
        fprintf(suboutput, "\n%d", a);
    } else if (nowSubRecord.name == "AI_F" || nowSubRecord.name == "AI_E") {
        short a;
        float b;
        char mas[32];
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 3; p++) {
            fread(&b, 4, 1, file);
            fprintf(suboutput, "\n%f", b);
        }
        fread(&a, 2, 1, file);
        fprintf(suboutput, "\n%hd", a);
        fread(mas, 1, 32, file);
        division(suboutput, mas, 32);
        fread(&a, 2, 1, file);
        fprintf(suboutput, "\n%hd", a);
    } else if (nowSubRecord.name == "AI_A") {
        char mas[32];
        byte firby;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        fread(mas, 1, 32, file);
        division(suboutput, mas, 32);
        fread(&firby, 1, 1, file);
        fprintf(suboutput, "\n%c", firby);
    } else if (nowSubRecord.name == "DODT") {
        float b;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 6; p++) {
            fread(&b, 4, 1, file);
            fprintf(suboutput, "\n%f", b);
        }
    } else if (nowSubRecord.name == "XSCL") {
        float b;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        fread(&b, 4, 1, file);
        fprintf(suboutput, "\n%f", b);
    } else if (nowSubRecord.name == "AODT") {
        int firstInt;
        float firstfl;
        fread(&firstInt, 4, 1, file);
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, firstInt, firstfl);
        for (int p = 0; p < 4; ++p) {
            fread(&firstInt, 4, 1, file);
            fprintf(suboutput, "\n%d", firstInt);
        }
    } else if ((nowSubRecord.name == "INDX" && (nowRecord.name == "ARMO" || nowRecord.name == "CLOT")) ||
               nowSubRecord.name == "UNAM") {
        byte firstByte;
        fprintf(suboutput, "%s\n%d", "INDX", nowSubRecord.size);
        for (int p = 0; p < nowSubRecord.size; p++) {
            fread(&firstByte, 1, 1, file);
            fprintf(suboutput, "\n%c", firstByte);
        }
    } else if (nowSubRecord.name == "CTDT") {
        int firstInt;
        float firstFloat;
        short firstShort;
        fread(&firstInt, 4, 1, file);
        fread(&firstFloat, 4, 1, file);

        fprintf(suboutput, "%s\n%d\n%d\n%f", "CTDT", nowSubRecord.size, firstInt, firstFloat);

        for (int p = 0; p < 2; ++p) {
            fread(&firstShort, 2, 1, file);

            fprintf(suboutput, "\n%hd", firstShort);
        }
    } else if (nowSubRecord.name == "RIDT") {
        int firstInt;
        float firstfl;
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, firstfl);
        for (int p = 0; p < 2; ++p) {
            fread(&firstInt, 4, 1, file);
            fprintf(suboutput, "\n%d", firstInt);
        }
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "\n%f", firstfl);
    } else if (nowSubRecord.name == "AADT") {
        int firstInt;
        float firstfl;
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%d", nowSubRecord.name.c_str(), nowSubRecord.size, firstInt);
        for (int p = 0; p < 2; ++p) {
            fread(&firstfl, 4, 1, file);
            fprintf(suboutput, "\n%f", firstfl);
        }
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
    } else if (nowSubRecord.name == "LKDT" || nowSubRecord.name == "PBDT") {
        int firstInt;
        float firstfl;
        fread(&firstInt, 4, 1, file);
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%d", nowSubRecord.name.c_str(), nowSubRecord.size, firstInt);
        fprintf(suboutput, "\n%f", firstfl);
        fread(&firstInt, 4, 1, file);
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
        fprintf(suboutput, "\n%f", firstfl);
    } else if (nowSubRecord.name == "BKDT") {
        int firstInt;
        float firstfl;
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, firstfl);
        for (int p = 0; p < 4; p++) {
            fread(&firstInt, 4, 1, file);
            fprintf(suboutput, "\n%d", firstInt);
        }
    } else if (nowSubRecord.name == "IRDT") {
        int firstInt;
        int mas[4];
        float firstfl;
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, firstfl);
        fread(&firstInt, 4, 1, file);
        fprintf(suboutput, "\n%d", firstInt);
        for (int k = 0; k < 3; k++) {
            fread(mas, 4, 4, file);
            for (int p = 0; p < 4; p++) {
                fprintf(suboutput, "\n%d", mas[p]);
            }
        }
    } else if (nowSubRecord.name == "ALDT") {
        int firstInt;
        float firstfl;
        fread(&firstfl, 4, 1, file);
        fprintf(suboutput, "%s\n%d\n%f", nowSubRecord.name.c_str(), nowSubRecord.size, firstfl);
        for (int p = 0; p < 2; p++) {
            fread(&firstInt, 4, 1, file);
            fprintf(suboutput, "\n%d", firstInt);
        }
    } else if (nowSubRecord.name == "INTV") {
        short CLDA;
        fread(&CLDA, 2, 1, file);
        fprintf(suboutput, "%s\n%d\n%hd", "INTV", nowSubRecord.size, CLDA);
    } else if (nowSubRecord.name == "AMBI") {
        int intvInt1;
        float Fl;
        fprintf(suboutput, "%s\n%d", nowSubRecord.name.c_str(), nowSubRecord.size);
        for (int p = 0; p < 3; ++p) {
            fread(&intvInt1, 4, 1, file);
            fprintf(suboutput, "\n%d", intvInt1);
        }
        fread(&Fl, 4, 1, file);
        fprintf(suboutput, "\n%f", Fl);
    } else if (nowSubRecord.name == "VHGT") {
        float fl;
        byte b;
        short sh;
        int con = 65 * 65 + 1;
        fprintf(suboutput, "%s\n%d", "VHGT", nowSubRecord.size);
        fread(&fl, 4, 1, file);
        fprintf(suboutput, "\n%f", fl);
        for (int p = 0; p < con; p++) {
            fread(&b, 1, 1, file);
            fprintf(suboutput, "\n%c", b);
        }
        fread(&sh, 2, 1, file);
        fprintf(suboutput, "\n%hd", sh);
    } else if (nowSubRecord.name == "WNAM" || nowSubRecord.name == "VTEX" || nowSubRecord.name == "QSTN" ||
               nowSubRecord.name == "QSTF" || nowSubRecord.name == "QSTR") {
        byte b;
        fprintf(suboutput, "%s\n%d", "WNAM", nowSubRecord.size);
        for (int p = 0; p < nowSubRecord.size; p++) {
            fread(&b, 1, 1, file);
            fprintf(suboutput, "\n%c", b);
        }
    } else if (nowSubRecord.name == "VCLR") {
        byte b[nowSubRecord.size];
        fread(b, 1, nowSubRecord.size, file);
        for (int p = 0; p < nowSubRecord.size; ++p) {
            fprintf(suboutput, "\n%c", b[p]);
        }
        string pepyao = subsuperPath / "VCLR.bmp";
        BmpFile_SaveScreenshot(b, pepyao.c_str(), 65, 65);
    } else if (nowSubRecord.name == "SCVR") {
        byte b;
        short s;
        fprintf(suboutput, "%s\n%d", "SCVR", nowSubRecord.size);
        for (int p = 0; p < 2; ++p) {
            fread(&b, 1, 1, file);
            fprintf(suboutput, "\n%c", b);
        }
        fread(&s, 2, 1, file);
        fprintf(suboutput, "\n%hd", s);
        for (int p = 0; p < nowSubRecord.size - 4; ++p) {
            fread(&b, 1, 1, file);
            fprintf(suboutput, "\n%c", b);
        }
    } else {
        char DeStr[nowSubRecord.size + 1];
        DeStr[nowSubRecord.size] = 0;
        fread(DeStr, 1, nowSubRecord.size, file);
        fprintf(output, "%s\n%d\n%s", nowSubRecord.name.c_str(), nowSubRecord.size, DeStr);
    }

    fclose(suboutput);
    return true;
}
