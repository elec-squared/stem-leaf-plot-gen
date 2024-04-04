// SLPG (stem leaf plot generator)
// Copyright (C) 2024 David Luz
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

const char VERSION[] = "v1.7.0";

// output style variables
int RIGHT_TO_LEFT  = 0; // -r
int NO_STEM        = 0; // -n
int PRINT_HELP     = 0; // -h
int PRINT_VER      = 0; // -v
int PRINT_DEC      = 0; // -d
// -1: don't print key
int KEY_NUM       = -1; // -k
int STEM_NUM_BEGIN = 0; // -s
// -1: none in args (get from stdin)
int datastr_index = -1;
int di_is_filepath = 0;
int FACTOR        = 10; // -f
int ptcount        = 0; // formerly -c

char flags[] = "rnhvd";
// [flag #] [char #]
char flags_full[6][24] = {
  "--right-to-left",
  "--no-stem",
  "--help",
  "--version",
  "--decimal",
  "",
};
int *ftovar[] = {
  &RIGHT_TO_LEFT,
  &NO_STEM,
  &PRINT_HELP,
  &PRINT_VER,
  &PRINT_DEC,
  NULL
};

char optval[] = "fsk"; // only options that take INTEGERS
char opttype[] = "iii"; // i = int, s = str (future?)
char optval_full[4][24] = {
  "--factor",
  "--starting-stem",
  "--key",
  "",
};
int *ovtovar[] = {
  &FACTOR,
  &STEM_NUM_BEGIN,
  &KEY_NUM,
  NULL
};


// if equal returns 0
int cmpfunc(const void * _a, const void * _b) {
   float a = *(float*)_a;
   float b = *(float*)_b;
   float diff = a-b;
   return fabs(diff) < 0.00005
     ? 0
     : (int)roundf(diff/fabs(diff)); // pos/pos = 1, pos/neg = -1
}

int cmpfunc_int(const void *a, const void *b) {
  return ( *(int*)a - *(int*)b );
}

int print_help() {
  printf("stem-leaf plot generator: 23:25 -> 2 | 3 5 ");
  printf("\nusage: slpg [data] options...");
  printf("\n       slpg [file_path] options...");
  printf("\nsplit elements in data with ':'");
  printf("\naccepts data from stdin");
  printf("\n-r / --right-to-left        stem on RIGHT and leaves on LEFT");
  printf("\n-n / --no-stem              omit/hide stem in output");
  printf("\n-h / --help                 show help message");
  printf("\n-v / --version              print version information");
  printf("\n-d / --decimal              format leaf values as decimal (e.g. 2 | 1.2)");
  printf("\n-k / --key [sample num]     print a key at the end (e.g. Key: 1 | 2 = 12)");
  printf("\n-f / --factor [num]         change factor of stem (e.g. 10: tens place in stem)");
  printf("\n-s / --starting-stem [num]  change starting stem\n");
  return 0;
}

int print_ver() {
  printf("Stem-leaf plot generator (slpg) %s", VERSION);
  printf("\n\nUses the GNU GPL-3.0 license");
  printf("\nThis program comes with no warranty (see LICENSE for details)\n");
  return 0;
}

int filepath_valid(char *fpath) {
  if (access(fpath, F_OK) == -1) {
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  int i, j;

  for (i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      datastr_index = i;
      continue; // i's for loop
    }

    if (strlen(argv[i]) < 2) {
      fprintf(stderr, "Invalid option at index %d\n", i);
      continue; // i's for loop
    }

    // full arg names
    char *opt_char = strchr(optval, argv[i][1]);
    if (argv[i][1] == '-') { 
      // options with values
      for (j = 0; j < sizeof(optval_full) / 24; j++) {
        if (strcmp(optval_full[j], argv[i]) == 0) {
          *(ovtovar[j]) = atoi(argv[++i]);
          break; // out of j
        }
      }
      // flags
      for (j = 0; j < sizeof(flags_full) / 24; j++) {
        if (strcmp(flags_full[j], argv[i]) == 0) {
          *(ftovar[j]) = 1; 
          break; // out of j
        }
      }
      continue; // done with this arg; i's for loop
    } else if (opt_char != NULL) { // option with one char
      if (i+1 >= argc
          || argv[i+1][0] == '-') {
        fprintf(stderr, "Value expected for \"%s\", none found\n", argv[i]);
        continue; // i's for loop
      }
      int var_index = opt_char - optval;
      *(ovtovar[var_index]) = atoi(argv[++i]);

      continue; // done with this arg; i's for loop
    }

    // parsing flag(s) with one char (switch 0 or 1)
    for (j = 1; j < strlen(argv[i]); j++) {
      char *flag_char = strchr(flags, argv[i][j]);
      if (flag_char == NULL) {
        fprintf(stderr, "\"%c\" not a flag, ignoring\n", argv[i][j]);
        continue; // j's for loop
      }
      // printf("flag %c\nindex %ld", argv[i][j], flag_char - flags);
      *(ftovar[flag_char - flags]) = 1;
    }
  }
  di_is_filepath = filepath_valid(argv[datastr_index]);

  if (PRINT_HELP) {
    print_help();
  }
  if (PRINT_VER) {
    print_ver();
  }
  if (PRINT_HELP || PRINT_VER) {
    return 0;
  }

  // if data is not in argument, get from stdin
  // OR if data is filepath, get from file
  char *filein_buffer = NULL;
  if (datastr_index == -1 || di_is_filepath) {
    size_t bsize = 16;
    filein_buffer = malloc(bsize);
    i = 0; // position in stdin_buffer
    FILE *input = stdin;
    if (di_is_filepath) {
      input = fopen(argv[datastr_index], "r");
    }

    while (!feof(input)) {
      // printf("i: %d\n", i);
      // printf("i+bsize: %d\n", i+bsize);
      char *temp_sb_pt = realloc(filein_buffer, (size_t) i + bsize);
      if (temp_sb_pt == NULL) {
        fprintf(stderr, "Could not allocate memory to continue reading");
        break;
      }
      filein_buffer = temp_sb_pt;
      // printf("sizeof stdin_buffer: %d\n", i+bsize);
      //                 * pointer  addition
      fread(filein_buffer + i, bsize, 1, input);
      i += bsize;
    }
    if (di_is_filepath) fclose(input);
  } 

  char *datastr = (datastr_index == -1 || di_is_filepath)
    ? filein_buffer : argv[datastr_index];
  // printf("%d %s\n", di_is_filepath, datastr);

  // find number of colons in string: # + 1 = ptcount
  for (i = 0; i < strlen(datastr); i++) {
    if (datastr[i] == ':') ptcount++;
  }
  ptcount++;

  float *data = (float*)malloc(ptcount * sizeof (float));
  int *datastem = (int*)malloc(ptcount * sizeof (int));

  char *token;
  float fart = 0;
  token = strtok(datastr, ":");
  for (i = 0; token != NULL; i++){
    fart = atof(token);
    data[i] = fart;
    datastem[i] = (int)(fart / FACTOR);
    token = strtok(NULL, ":");
  }
  ptcount = i;

  qsort(data, ptcount, sizeof(float), cmpfunc);
  qsort(datastem, ptcount, sizeof(int), cmpfunc_int);

  int leaf_max = 1;
  int current_leaf_no = 0;
  // find max amount of leaves any given stem in data set may have
  // do this post sorting so stem always increases
  for (i = 1; i < ptcount; i++) {
    if (datastem[i-1] == datastem[i]) {
      current_leaf_no++;
      if (leaf_max < current_leaf_no + 1)
        leaf_max = current_leaf_no + 1;
    } else
      current_leaf_no = 0;
  }

  // print warning if starting stem cuts off data
  if (data[0] < STEM_NUM_BEGIN * FACTOR) {
    fprintf(stderr,
        "WARNING: smallest data point %.1f is lower than minimum %d\n",
        data[0], STEM_NUM_BEGIN*FACTOR);
  }

  // generate stem-leaf matrix: [stem][leafindex]
  // e.g. with stem_num_begin as 2
  // 0(2) | 1.0 2.1 3.0
  // 1(3) | 4.0 4.1 5.2
  // stem_num_begin -> maxstem
  // i iterates 0 -> maxstem - stem_num_begin
  // stored continuously:
  // 1.0 2.1 3.0 - - - 4.0 4.1 5.2 - - -
  if (FACTOR < 10) { // cannot continue as log10(FACTOR) would be less than 1
                     // see code further ahead
    fprintf(stderr, "ERROR: Factor is less than 10, aborting\n");
    return -1;
  }
  int stem_max = datastem[ptcount-1] - STEM_NUM_BEGIN + 1;
  // int leaf_max = ptcount + 1;
  float *sl_matrix = (float*)malloc(stem_max
      * leaf_max * sizeof(float));
  for (i = 0; i < stem_max; i++) {
    for (j = 0; j < leaf_max; j++){
      sl_matrix[i * leaf_max + j] = -1.0;
    }
  }
  // generate leaves
  current_leaf_no = 0;
  for (i = 0; i < ptcount; i++) {
    sl_matrix[(datastem[i] - STEM_NUM_BEGIN)*leaf_max + current_leaf_no]
      = fmodf(data[i], (float)FACTOR); // data[i] % FACTOR
    if (i < ptcount-1) {
      if (datastem[i+1]==datastem[i]) current_leaf_no++;
      else current_leaf_no = 0;
    }
  }
  free(data);
  free(datastem);

  // e.g. for factor 100, need 2 digits: 1 | 00 02 04
  // %x.yf:
  // x is total digits (all digits left of decimal point, the point itself, all digits right of point)
  // x = leaf_digits + print_dec (if there is a decimal point) + print_dec (if there is decimal)
  // x = print_dec * 2
  int leaf_digits = (int)log10(FACTOR);
  char leaf_format_str[strlen("%.?f ")
    + (int)log10(leaf_digits) /*1 -> 1, 10 -> 2*/];

  // ternary bool ? r-l : l-r
  for (i = 0; i < stem_max; i++) {
    if (!NO_STEM && !RIGHT_TO_LEFT) printf("%d | ", i + STEM_NUM_BEGIN);
    for (j = RIGHT_TO_LEFT ? leaf_max - 1 : 0;
      RIGHT_TO_LEFT ? j >= 0 : j < leaf_max;
      j += RIGHT_TO_LEFT ? -1 : 1) {
      if (sl_matrix[i * leaf_max + j] < FACTOR
          && sl_matrix[i * leaf_max + j] >= 0) {
        sprintf(leaf_format_str, "%%0%d.%df ", leaf_digits + PRINT_DEC*2, PRINT_DEC);
        printf(leaf_format_str, sl_matrix[i * leaf_max + j]);
      }
    }
    // "| %d" NOT " | %d" because the leaves printed will have a trailing space
    if (!NO_STEM && RIGHT_TO_LEFT) printf("| %d", i + STEM_NUM_BEGIN);
    printf("\n");
  }

  // print key/legend
  if (KEY_NUM >= 0) {
    printf("Key: ");
    if (!RIGHT_TO_LEFT) printf("%d | ", (int)(KEY_NUM/FACTOR));
    printf(leaf_format_str, fmodf(KEY_NUM, (float)FACTOR));
    if (RIGHT_TO_LEFT) printf("| %d ", (int)(KEY_NUM/FACTOR));
    printf("= %d\n", KEY_NUM);
  }

  free(sl_matrix);
  free(filein_buffer);
  return 0;
}
