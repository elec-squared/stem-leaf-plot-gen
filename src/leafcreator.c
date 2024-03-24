#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Copyleft NORTRITECH CORPORATION
// TODO: add support for stdin
// if equal returns 0
int cmpfunc(const void * _a, const void * _b) {
   float a = *(float*)_a;
   float b = *(float*)_b;
   float diff = a-b;
   return fabs(diff) < 0.00005 ?
     0 : (int)roundf(diff/fabs(diff)); // pos/pos = 1, pos/neg = -1
}

int cmpfunc_int(const void *a, const void *b) {
  return ( *(int*)a - *(int*)b );
}
	
int print_help() {
  printf("stem-leaf plot generator: 23:25 -> 2 | 3 5 ");
  printf("\nusage: slpg [data] [-nr] [-f factor]");
  printf("\nsplit elements in data with ':'");
  printf("\n -r for stem on RIGHT and leaves on LEFT");
  printf("\n -n to omit/hide stem in output");
  printf("\n -h to show help message");
  printf("\n -d to print leaf values as decimal (e.g. 2 | 1.2)");
  printf("\n -f [num]: change factor of stem (e.g. 10: tens place in stem)");
  printf("\n -s [num]: change starting stem\n");
  return 0;
}

int main(int argc, char *argv[]) {
  // output style variables
  int RIGHT_TO_LEFT = 0; // -r
  int NO_STEM = 0;       // -n
  int PRINT_HELP = 0;    // -h
  int PRINT_DEC = 0;     // -d
  int STEM_NUM_BEGIN = 0;// -s
  // -1: stdin
  // -2: none
  int datastr_index = -2;
  int FACTOR = 10;       // -f
  int ptcount = 0;       // formerly -c
  int i;
  int j;
  
  for (i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      datastr_index = i;
      continue; // i's for loop
    }

    if (strlen(argv[i]) < 2) {
      fprintf(stderr, "Invalid option at index %d\n", i);
    }

    char flags[] = "rnhd";
    int* ftovar[] = {
      &RIGHT_TO_LEFT,
      &NO_STEM,
      &PRINT_HELP,
      &PRINT_DEC,
      NULL
    };

    char optval[] = "fs"; // only options that take INTEGERS
    char opttype[] = "ii"; // i = int, s = str (future?)
    int* ovtovar[] = {
      &FACTOR,
      &STEM_NUM_BEGIN,
      NULL
    };

    char* opt_char = strchr(optval, argv[i][1]);
    if (opt_char != NULL) {
      // parsing option with value
      if (i+1 >= argc
          || argv[i+1][0] == '-') {
        fprintf(stderr, "Value expected for \"%s\", none found\n", argv[i]);
        continue; // i's for loop
      }
      int var_index = opt_char - optval; 
      *(ovtovar[var_index]) = atoi(argv[++i]);
      continue; // done with this arg
    }
    // parsing flag(s) (switch 0 or 1)
    for (j = 1; j < strlen(argv[i]); j++) {
      char* flag_char = strchr(flags, argv[i][j]);
      if (flag_char == NULL) {
        fprintf(stderr, "\"%c\" not a flag, ignoring\n", argv[i][j]);
        continue; // j's for loop
      }
      // printf("flag %c\nindex %ld", argv[i][j], flag_char - flags);
      *(ftovar[flag_char - flags]) = 1;
    }

    if (PRINT_HELP) {
      print_help();
      return 0;
    }
  }

  // if data is not in argument, get from stdin 
  char* stdin_buffer = NULL;
  if (datastr_index == -2) {
    // printf("No data string in arguments found\n");
    datastr_index = -1;
    size_t bsize = 16;
    stdin_buffer = malloc(bsize); 
    i = 0; // position in stdin_buffer
    while (!feof(stdin)) {
      // printf("i: %d\n", i);
      // printf("i+bsize: %d\n", i+bsize);
      char* temp_sb_pt = realloc(stdin_buffer, (size_t) i + bsize);
      if (temp_sb_pt == NULL) {
        fprintf(stderr, "Could not allocate memory to continue reading");
        break;
      }
      stdin_buffer = temp_sb_pt;
      // printf("sizeof stdin_buffer: %d\n", i+bsize);
      //                 * pointer  addition 
      fread(stdin_buffer + i, bsize, 1, stdin);
      i += bsize;
    }
  }

  char* datastr = (datastr_index == -1) ? stdin_buffer : argv[datastr_index];
  // printf("%s\n", datastr);

  if (datastr_index == -2 /* || argc <= 1 */) {
    print_help();
    return 0;
  }

  // find number of colons in string: # + 1 = ptcount
  for (i = 0; i < strlen(datastr); i++) {
    if (datastr[i] == ':') ptcount++;
  }
  ptcount++;

  float* data = (float *) malloc(ptcount * sizeof (float));
  int* datastem = (int* )malloc(ptcount * sizeof (int)); 

  char* token;
  float fart = 0;
  token = strtok(datastr, ":");
  for (i = 0; token != NULL; i++){
    fart = atof(token);
    data[i] = fart;
    datastem[i] = (int)(fart / FACTOR);
    token = strtok(NULL, ":");
  }
  ptcount = i;

  // printf("Sorting... ");
  qsort(data, ptcount, sizeof(float), cmpfunc);
  qsort(datastem, ptcount, sizeof(int), cmpfunc_int); 
  
  // print warning if starting stem cuts off data
  if (data[0] < STEM_NUM_BEGIN * FACTOR) {
    fprintf(stderr,
        "WARNING: smallest data point %.1f is lower than minimum %d\n",
        data[0], STEM_NUM_BEGIN*FACTOR);
  }

  // stem-leaf matrix: [stem][leafindex]
  // e.g. with stem_num_begin as 2
  // 0(2) | 1.0 2.1 3.0
  // 1(3) | 4.0 4.1 5.2
  // stem_num_begin -> maxstem (below applied stem_num_begin offset to start at 0)
  // i iterates 0 -> maxstem - stem_num_begin
  float sl_matrix[datastem[ptcount-1]+1][ptcount];
  for (i = 0; i <= datastem[ptcount-1] - STEM_NUM_BEGIN; i++) {
    for (j=0;j < ptcount;j++){
      sl_matrix[i][j]= -1.0;
    }
  }
  int current_leaf_no = 0;
  for (i = 0; i < ptcount; i++) {
    sl_matrix[datastem[i] - STEM_NUM_BEGIN][current_leaf_no]
      = fmodf(data[i], (float)FACTOR);
    if (i < ptcount-1) {
      if (datastem[i+1]==datastem[i]) current_leaf_no++;
      else current_leaf_no = 0;
    }
  }
  
  // ternary bool ? r-l : l-r
  for (i = 0; i <= datastem[ptcount-1] - STEM_NUM_BEGIN; i++) {
    if (!NO_STEM && !RIGHT_TO_LEFT) printf("%d | ", i + STEM_NUM_BEGIN);
//         (right_to_left ? ptcount-1 : 0)  
    for (j = (ptcount-1)*RIGHT_TO_LEFT; 
      RIGHT_TO_LEFT ? j >= 0 : j < ptcount;
//         (right_to_left ? -1 : 1)   
      j += 1-2*RIGHT_TO_LEFT) {
      if (sl_matrix[i][j] < FACTOR && sl_matrix[i][j] >= 0) {
        // e.g. for factor 100, need 2 digits: 1 | 00 02 04   
        int leaf_digits = (int)log10(FACTOR);
        char leaf_format_str[strlen("%.?f ") + (int)log10(leaf_digits) /*1 -> 1, 10 -> 2*/];
        // %x.yf:
        // x is total digits (all digits left of decimal point, the point itself, all digits right of point)
        // x = leaf_digits + print_dec (if there is a decimal point) + print_dec (if there is decimal)
        // x = print_dec * 2
        sprintf(leaf_format_str, "%%0%d.%df ", leaf_digits + PRINT_DEC*2, PRINT_DEC);
        printf(leaf_format_str, sl_matrix[i][j]);
      }
    }
    if (!NO_STEM && RIGHT_TO_LEFT) printf("| %d", i + STEM_NUM_BEGIN);
    printf("\n");
  }
  
  // printf("NOTE: PLEASE check last row for accuracy-- it may contain extra numbers from garbage memory and I can't be bothered to fix it\n");

  free(data);
  free(datastem);
  free(stdin_buffer);
  return 0;
}
