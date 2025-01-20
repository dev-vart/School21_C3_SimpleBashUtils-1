#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int regex_flag;    // Игнорирует различия регистра.
  bool example;      // Шаблон.
  bool v_invert;     // Инвертирует смысл поиска соответствий.
  bool count;        // Выводит только количество совпадающих строк.
  bool l_filenames;  // Выводит только совпадающие файлы.
  bool
      number;  // Предваряет каждую строку вывода номером строки из файла ввода.
  bool h_no_filename;  //	Выводит совпадающие строки, не предваряя их
                       // именами файлов.
  bool supress;        // Подавляет сообщения об ошибках о несуществующих или
                       // нечитаемых файлах.
  bool file;           // Получает регулярные выражения из файла.
  bool only_matching;  // Печатает только совпадающие (непустые) части совпавшей
                       // строки.
  bool error;
} GrepFlags;

GrepFlags grepReadFlags(int argc, char* argv[]);

regex_t* readPatternReal(int argc, char* argv[], char*** argit, int regex_flag,
                         regex_t* preg);

void grepFile(FILE* file, char const* filename, GrepFlags flags, regex_t* preg,
              int fileCount);

void countFiles(int argc, char* argv[], char** argit, int* fileCount);

void readFiles(int argc, char* argv[], char** argit, GrepFlags flags,
               regex_t* preg, int fileCount);

void grepNameFile(FILE* file, char const* filename, GrepFlags flags,
                  regex_t* preg);

void grepCountFile(FILE* file, char const* filename, GrepFlags flags,
                   regex_t* preg, int fileCount);

int main(int argc, char* argv[]) {
  GrepFlags flags = grepReadFlags(argc, argv);
  int fileCount = 0;
  regex_t preg_storage = {0};
  regex_t* preg = &preg_storage;

  char** argit = argv + 1;

  if (flags.error == 0) {
    preg = readPatternReal(argc, argv, &argit, flags.regex_flag, preg);

    if (preg != NULL) {
      countFiles(argc, argv, argit, &fileCount);

      readFiles(argc, argv, argit, flags, preg, fileCount);

      if (fileCount == 0) grepFile(stdin, *argit, flags, preg, fileCount);
    }
  }

  if (preg != NULL) {
    regfree(preg);
  }

  return 0;
}

GrepFlags grepReadFlags(int argc, char* argv[]) {
  GrepFlags flags = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int curFlag = 0;
  while ((curFlag = getopt(argc, argv, "e:ivclnhsfo")) != -1) {
    switch (curFlag) {
      case 'e':
        flags.example = true;
        break;
      case 'i':
        flags.regex_flag |= REG_ICASE;
        break;
      case 'v':
        flags.v_invert = true;
        break;
      case 'c':
        flags.count = true;
        break;
      case 'l':
        flags.l_filenames = true;
        break;
      case 'n':
        flags.number = true;
        break;
      case 'h':
        flags.h_no_filename = true;
        break;
      case 's':
        flags.supress = true;
        break;
      case 'f':
        flags.file = true;
        break;
      case 'o':
        flags.only_matching = true;
        break;
      default:
        flags.error = true;
        break;
    }
  }
  return flags;
}

regex_t* readPatternReal(int argc, char* argv[], char*** argit, int regex_flag,
                         regex_t* preg) {
  char** end = argv + argc;

  for (; *argit != end && ***argit == '-'; ++(*argit)) {
    if (strcmp(**argit, "-e") == 0) {
      ++(*argit);
      break;
    }
  }

  if (*argit == end) {
    fprintf(stderr, "Usage: ./s21_grep [OPTION]... PATTERNS [FILE]...\n");
    preg = NULL;
  } else if (regcomp(preg, **argit, regex_flag)) {
    fprintf(stderr, "Error: Regex compilation failed\n");
    preg = NULL;
  }

  (*argit)++;

  return preg;
}

void countFiles(int argc, char* argv[], char** argit, int* fileCount) {
  char** start = argit;
  char** end = argc + argv;
  for (; argit != end; ++argit) {
    if (**argit == '-') continue;
    (*fileCount)++;
  }
  argit = start;
}

void readFiles(int argc, char* argv[], char** argit, GrepFlags flags,
               regex_t* preg, int fileCount) {
  char** end = argc + argv;

  for (; argit != end; ++argit) {
    if (**argit == '-') continue;

    FILE* file = fopen(*argit, "rb");
    if (file == NULL && flags.supress) {
      ;
    } else if (file == NULL && !flags.supress) {
      fprintf(stderr, "%s: ", argv[0]);
      perror(*argit);
    } else if (flags.l_filenames) {
      grepNameFile(file, *argit, flags, preg);
      fclose(file);
    } else if (flags.count) {
      grepCountFile(file, *argit, flags, preg, fileCount);
      fclose(file);
    } else {
      grepFile(file, *argit, flags, preg, fileCount);
      fclose(file);
    }
  }
}

void grepFile(FILE* file, char const* filename, GrepFlags flags, regex_t* preg,
              int fileCount) {
  char* line = 0;
  int lineCount = 1;
  size_t length = 0;
  regmatch_t match = {0};
  size_t lineLen = 0;

  while (getline(&line, &length, file) > 0) {
    if (regexec(preg, line, 1, &match, 0) != (!flags.v_invert)) {
      if (fileCount > 1 && !flags.h_no_filename &&
          !(flags.only_matching && flags.v_invert))
        printf("%s:", filename);
      if (flags.number && !(flags.only_matching && flags.v_invert))
        printf("%d:", lineCount);

      if (flags.only_matching && !flags.v_invert) {
        printf("%.*s\n", (int)(match.rm_eo - match.rm_so), line + match.rm_so);
        char* remaining = line + match.rm_eo;
        while (regexec(preg, remaining, 1, &match, 0) != (!flags.v_invert)) {
          if (flags.number) printf("%d:", lineCount);
          printf("%.*s\n", (int)(match.rm_eo - match.rm_so),
                 remaining + match.rm_so);
          remaining += match.rm_eo;
        }
      } else if (!flags.only_matching) {
        printf("%s", line);
      }

      lineLen = strlen(line);

      if (lineLen > 0 && line[lineLen - 1] != '\n' && !flags.only_matching)
        printf("\n");
    }
    lineCount++;
  }

  free(line);
}

void grepNameFile(FILE* file, char const* filename, GrepFlags flags,
                  regex_t* preg) {
  char* line = 0;
  size_t length = 0;
  regmatch_t match = {0};
  int count = 0;

  while (getline(&line, &length, file) > 0) {
    if (regexec(preg, line, 1, &match, 0) != (!flags.v_invert)) {
      ++count;
    }
  }

  if (count > 0) printf("%s\n", filename);

  free(line);
}

void grepCountFile(FILE* file, char const* filename, GrepFlags flags,
                   regex_t* preg, int fileCount) {
  char* line = 0;
  size_t length = 0;
  regmatch_t match = {0};
  int count = 0;

  while (getline(&line, &length, file) > 0) {
    if (regexec(preg, line, 1, &match, 0) != (!flags.v_invert)) {
      ++count;
    }
  }

  if (fileCount > 1 && !flags.h_no_filename) printf("%s:", filename);

  printf("%d\n", count);

  free(line);
}
