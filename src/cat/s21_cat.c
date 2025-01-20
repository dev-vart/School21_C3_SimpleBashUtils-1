#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "table.h"

typedef struct {
  bool nonblank;
  bool endl;
  bool number;
  bool squeeze;
  bool tab;
  bool vnonPrintable;
  bool error;
} CatFlags;

CatFlags catReadFlags(int argc, char* argv[]);

void readFiles(int argc, char* argv[], CatFlags flags, const char* table[256],
               bool* nofiles);

void catFile(FILE* file, CatFlags flags, const char* table[]);

int main(int argc, char* argv[]) {
  const char* table[256];
  CatFlags flags = catReadFlags(argc, argv);
  bool nofiles = true;

  if (flags.error == 0) {
    setTable(table);

    if (flags.endl) setEndl(table);
    if (flags.tab) setTab(table);
    if (flags.vnonPrintable) setNonPrintable(table);

    readFiles(argc, argv, flags, table, &nofiles);

    if (nofiles) catFile(stdin, flags, table);
  }

  return 0;
}

CatFlags catReadFlags(int argc, char* argv[]) {
  struct option longOptions[] = {{"number-nonblank", 0, NULL, 'b'},
                                 {"number", 0, NULL, 'n'},
                                 {"squeeze-blank", 0, NULL, 's'},
                                 {NULL, 0, NULL, 0}};
  CatFlags flags = {0, 0, 0, 0, 0, 0, 0};
  int curFlag = 0;
  while ((curFlag = getopt_long(argc, argv, "bevEnstT", longOptions, NULL)) !=
         -1) {
    switch (curFlag) {
      case 'b':
        flags.nonblank = true;
        break;
      case 'e':
        flags.endl = true;
        flags.vnonPrintable = true;
        break;
      case 'v':
        flags.vnonPrintable = true;
        break;
      case 'E':
        flags.endl = true;
        break;
      case 'n':
        flags.number = true;
        break;
      case 's':
        flags.squeeze = true;
        break;
      case 't':
        flags.tab = true;
        flags.vnonPrintable = true;
        break;
      case 'T':
        flags.tab = true;
        break;
      default:
        flags.error = true;
        break;
    }
  }
  return flags;
}

void readFiles(int argc, char* argv[], CatFlags flags, const char* table[256],
               bool* nofiles) {
  for (char **curArg = argv + 1, **end = argv + argc; curArg != end; ++curArg) {
    if (**curArg == '-') continue;

    FILE* file = fopen(*curArg, "rb");
    *nofiles = false;
    if (file == NULL) {
      fprintf(stderr, "%s: ", argv[0]);
      perror(*curArg);
    } else {
      catFile(file, flags, table);
      fclose(file);
    }
  }
}

void catFile(FILE* file, CatFlags flags, const char* table[]) {
  static int c;
  static int linecnt = 0;
  static int last1 = '\n';
  static int last2 = 0;
  while (fread(&c, 1, 1, file) > 0) {
    if (last1 == '\n') {
      if (flags.squeeze && last2 == '\n' && c == '\n') continue;
      if (flags.nonblank) {
        if (c != '\n') printf("%6i\t", ++linecnt);
      } else if (flags.number) {
        printf("%6i\t", ++linecnt);
      }
    }

    printf("%s", table[c]);
    last2 = last1;
    last1 = c;
  }
}
