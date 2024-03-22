# stem-leaf-plot-gen
little thing for math class because I do not want to write those numbers by hand (but I am willing to spend hours coding this)<br />
edit: this is now a side project that I work on occasionally (the next time we do statistics in math I need a good stem leaf plot application!!)

INPUT: numbers separated with `:`. By default, it uses the tens place as stem but can be changed with `-f \[number\]` (f for factor).
To make for those goofy two-sided stem leaf plots, you can generate one side then the other by including `-r -n` in the arguments to generate right to left and to hide stem (no stem). Then, join both together in another application.

Accepts the data set as one of the arguments or through stdin (if none in args found). The data set given to the app can be unsorted.

```
examples of stem leaf plot things
1 | 3 = 13
```
# Usage examples
Print help message <br />
`slpg -h`

Print a stem leaf plot with starting stem 1, displayed as integers <br />
`slpg -s 1 13:14:21:50`
```
1 | 3 4
2 | 1
3 |
4 |
5 | 0
```

Print a stem leaf plot with stem as hundreds place, displayed as decimals <br />
`slpg -d -f 100 203:104:116:102`
```
1 | 2.0 4.0 16.0
2 | 3.0
```
