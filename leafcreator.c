#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Copyleft NORTRITECH CORPORATION

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(int argc, char *argv[]) {
  // output style variables
  int right_to_left = 0;
  int no_stem = 0;

  for (int i =0; i < argc; i++) {
    if (strcmp(argv[i], "-r") == 0) {
      right_to_left = 1;
    }
    if (strcmp(argv[i], "-h") == 0) {
      no_stem = 1;
    }
  }
  
  printf("stem-leaf plot creator: 23:25 -> 2 | 3 5 ");
  printf("\ninclude parameter -r for stem on RIGHT and leaves on LEFT");
  printf("\ninclude parameter -h to omit/hide stem in output");
  // TODO: add R to L mode

  // collect data will be unsorted.
  int ptcount = 0;
  printf("\ndata point quantity: ");
  scanf("%d", &ptcount);
  
  int* data = (int *) malloc(ptcount * sizeof (int));
  int* datastem = (int *) malloc(ptcount * sizeof (int));  
  char datastring[ptcount*4];
 
  printf("Insert data to parse into stem (tens-ones)- split with `:'\n>");
  scanf("%s", datastring);
  
  char* token;
  int fart = 0;
  int i;
  int j;
  token = strtok(datastring, ":");
  for (i = 0; i < ptcount; i++) {
    fart = atoi(token);
    data[i] = fart;
    datastem[i] = fart / 10;
    token = strtok(NULL, ":");
  }

  printf("Sorting... ");
  qsort(data, ptcount, sizeof(int), cmpfunc);
  qsort(datastem, ptcount, sizeof(int), cmpfunc);
  
  // TODO: organize data into necessary tens stems and print
  // do not forget R to L mode
  // test after sort
  for (i = 0; i < ptcount; i++) {
    printf("%d ", data[i]);
  }
  printf("\n\n");
  
  // stem-leaf matrix: [stem][leafindex]
  // eg:
  // 0 | 1 2 3
  // 1 | 4 4 5
  // intialize with max stem
  int sl_matrix[datastem[ptcount-1]][ptcount];
  for (i = 0; i < datastem[ptcount-1]; i++) {
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
  
  // L to R

  for (i = 0; i <= datastem[ptcount-1]; i++) {
    if (!no_stem && !right_to_left) printf("%d | ", i);
    for (j = (right_to_left ? ptcount-1 : 0); 
      (right_to_left ? j >= 0 : j < ptcount);
      j += (right_to_left ? -1 : 1)) {
      if (sl_matrix[i][j] < 10 && sl_matrix[i][j] >= 0)
        printf("%d ", sl_matrix[i][j]);
    }
    if (!no_stem && right_to_left) printf("| %d", i);
    printf("\n");
  }
  
  printf("NOTE: PLEASE check last row for accuracy-- it may contain extra numbers from garbage memory and I can't be bothered to fix it\n");

  free(data);
  free(datastem);
  return 0;
}
