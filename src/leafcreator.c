#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Copyleft NORTRITECH CORPORATION
// TODO: add support for stdin
// if equal returns 0
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}
	
int print_help() {
  printf("stem-leaf plot generator: 23:25 -> 2 | 3 5 ");
  printf("\nusage: slpg [data] [-nr]");
  printf("\ninclude parameter -r for stem on RIGHT and leaves on LEFT");
  printf("\ninclude parameter -n to omit/hide stem in output");
  printf("\ninclude parameter -h to show help message");
//printf("\n**needs** parameter -c [data element count]");
  printf("\nsplit elements in data with ':'\n");
  return 0;
}

int main(int argc, char *argv[]) {
  // output style variables
  int right_to_left = 0;
  int no_stem = 0;
  int stem_num_begin = 0;
  // -1: stdin
  // -2: none
  int datastr_index = -2;
  int ptcount = 0;
  int i;
  int j;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0) {
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
//  if (strcmp(argv[i], "-c") == 0
//    && i+1 < argc) {
//    ptcount = atoi(argv[++i]);
//  }
  }

  // if data is not in argument, get from stdin 
  char* stdin_buffer = NULL;
  if (datastr_index == -2) {
    // printf("No data string in arguments found\n");
    datastr_index = -1;
    stdin_buffer = malloc(30);
    i = 0;
    while (!feof(stdin)) {
      if (++i%30 == 29) {
        char* temp_sb_pt = realloc(stdin_buffer, i+30);
        if (temp_sb_pt == NULL) {
          fprintf(stderr, "Could not allocate memory to continue reading");
          break;
        }
        stdin_buffer = temp_sb_pt;
      }
      fread(stdin_buffer, sizeof(stdin_buffer), 1, stdin);
      // printf("%d", i);
    }
  }

  char* datastr = (datastr_index == -1) ? stdin_buffer : argv[datastr_index];

  if (datastr_index == -2 /* || argc <= 1 */) {
    print_help();
    return 0;
  }

  // find number of colons in string: # + 1 = ptcount
  for (i = 0; i < strlen(datastr); i++) {
    if (datastr[i] == ':') ptcount++;
  }
  ptcount++;

  int* data = (int *) malloc(ptcount * sizeof (int));
  int* datastem = (int *) malloc(ptcount * sizeof (int)); 

  char* token;
  int fart = 0;
  token = strtok(
      datastr,
      ":");
  for (i = 0; token != NULL; i++){
    fart = atoi(token);
    data[i] = fart;
    datastem[i] = fart / 10;
    token = strtok(NULL, ":");
  }
  ptcount = i;

  // printf("Sorting... ");
  qsort(data, ptcount, sizeof(int), cmpfunc);
  qsort(datastem, ptcount, sizeof(int), cmpfunc); 
  /*
  for (i = 0; i < ptcount; i++) {
    printf("%d ", data[i]);
  }
  printf("\n\n");
  */
  
  // stem-leaf matrix: [stem][leafindex]
  // eg:
  // 0 | 1 2 3
  // 1 | 4 4 5
  // i iterates 0 -> maxstem
  int sl_matrix[datastem[ptcount-1]+1][ptcount];
  for (i = 0; i <= datastem[ptcount-1]; i++) {
    for (j=0;j < ptcount;j++){
      sl_matrix[i][j]=-69;
    }
  }
  int current_leaf_no = 0;
  for (i = 0; i < ptcount; i++) {
    sl_matrix[datastem[i]][current_leaf_no] = data[i] - data[i]/10*10;
    if (i < ptcount-1) {
      if (datastem[i+1]==datastem[i]) current_leaf_no++;
      else current_leaf_no = 0;
    }
  }
  
  // ternary bool ? r-l : l-r
  for (i = 0; i <= datastem[ptcount-1]; i++) {
    if (!no_stem && !right_to_left) printf("%d | ", i);
//         (right_to_left ? ptcount-1 : 0)  
    for (j = (ptcount-1)*right_to_left; 
      right_to_left ? j >= 0 : j < ptcount;
//         (right_to_left ? -1 : 1)   
      j += 1-2*right_to_left) {
      if (sl_matrix[i][j] < 10 && sl_matrix[i][j] >= 0)
        printf("%d ", sl_matrix[i][j]);
    }
    if (!no_stem && right_to_left) printf("| %d", i);
    printf("\n");
  }
  
  // printf("NOTE: PLEASE check last row for accuracy-- it may contain extra numbers from garbage memory and I can't be bothered to fix it\n");

  free(data);
  free(datastem);
  free(stdin_buffer);
  return 0;
}
