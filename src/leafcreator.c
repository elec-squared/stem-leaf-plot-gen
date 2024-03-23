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
  int right_to_left = 0;
  int no_stem = 0;
  int print_dec = 0;
  int stem_num_begin = 0;
  // -1: stdin
  // -2: none
  int datastr_index = -2;
  int factor = 10;
  int ptcount = 0;
  int i;
  int j;
  
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0
        || strcmp(argv[i], "--help") == 0) {
      print_help();
      return 0;
    } 
    if (argv[i][0] != '-') {
      // if data is argument
      datastr_index = i;
    }
    if (strcmp(argv[i], "-r") == 0) {
      right_to_left = 1;
    }
    if (strcmp(argv[i], "-n") == 0) {
      no_stem = 1;
    }
    if (strcmp(argv[i], "-d") == 0) {
      print_dec = 1;
    }
    if (strcmp(argv[i], "-f") == 0 
      && i+1 < argc) {
      factor = atoi(argv[++i]);
    }
    if (strcmp(argv[i], "-s") == 0 
      && i+1 < argc) {
      stem_num_begin = atoi(argv[++i]);
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
      } else {
        stdin_buffer = temp_sb_pt;
      }
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
    datastem[i] = (int)(fart / factor);
    token = strtok(NULL, ":");
  }
  ptcount = i;

  // printf("Sorting... ");
  qsort(data, ptcount, sizeof(float), cmpfunc);
  qsort(datastem, ptcount, sizeof(int), cmpfunc_int); 
  
  // print warning if starting stem cuts off data
  if (data[0] < stem_num_begin * factor) {
    fprintf(stderr,
        "WARNING: smallest data point %.1f is lower than minimum %d\n",
        data[0], stem_num_begin*factor);
  }

  // stem-leaf matrix: [stem][leafindex]
  // e.g. with stem_num_begin as 2
  // 0(2) | 1.0 2.1 3.0
  // 1(3) | 4.0 4.1 5.2
  // stem_num_begin -> maxstem (below applied stem_num_begin offset to start at 0)
  // i iterates 0 -> maxstem - stem_num_begin
  float sl_matrix[datastem[ptcount-1]+1][ptcount];
  for (i = 0; i <= datastem[ptcount-1] - stem_num_begin; i++) {
    for (j=0;j < ptcount;j++){
      sl_matrix[i][j]= -1.0;
    }
  }
  int current_leaf_no = 0;
  for (i = 0; i < ptcount; i++) {
    sl_matrix[datastem[i] - stem_num_begin][current_leaf_no]
      = fmodf(data[i], (float)factor);
    if (i < ptcount-1) {
      if (datastem[i+1]==datastem[i]) current_leaf_no++;
      else current_leaf_no = 0;
    }
  }
  
  // ternary bool ? r-l : l-r
  for (i = 0; i <= datastem[ptcount-1] - stem_num_begin; i++) {
    if (!no_stem && !right_to_left) printf("%d | ", i + stem_num_begin);
//         (right_to_left ? ptcount-1 : 0)  
    for (j = (ptcount-1)*right_to_left; 
      right_to_left ? j >= 0 : j < ptcount;
//         (right_to_left ? -1 : 1)   
      j += 1-2*right_to_left) {
      if (sl_matrix[i][j] < factor && sl_matrix[i][j] >= 0) {
        // e.g. for factor 100, need 2 digits: 1 | 00 02 04   
        int leaf_digits = (int)log10(factor);
        char leaf_format_str[strlen("%.?f ") + (int)log10(leaf_digits) /*1 -> 1, 10 -> 2*/];
        // %x.yf:
        // x is total digits (all digits left of decimal point, the point itself, all digits right of point)
        // x = leaf_digits + print_dec (if there is a decimal point) + print_dec (if there is decimal)
        // x = print_dec * 2
        sprintf(leaf_format_str, "%%0%d.%df ", leaf_digits + print_dec*2, print_dec);
        printf(leaf_format_str, sl_matrix[i][j]);
      }
    }
    if (!no_stem && right_to_left) printf("| %d", i + stem_num_begin);
    printf("\n");
  }
  
  // printf("NOTE: PLEASE check last row for accuracy-- it may contain extra numbers from garbage memory and I can't be bothered to fix it\n");

  free(data);
  free(datastem);
  free(stdin_buffer);
  return 0;
}
