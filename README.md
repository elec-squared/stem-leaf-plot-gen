# stem-leaf-plot-gen
little thing for math class because I do not want to write those numbers by hand (but I am willing to spend hours coding this)

INPUT: integers separated with `:`. It will use the tens place as the stem so process any data beforehand if you want a different legend.
To make for those goofy two-sided stem leaf plots, you can generate one side then the other by including -r -h in the arguments to generate right to left and to hide stem. Then, join both together in another application

Does not follow best practices for command line apps (forcibly interactive input) but I might make it do so if I have time

```
examples of stem leaf plot things
1 | 3 = 13

13, 14, 21, 50:
1 | 3 4
2 | 1
3 |
4 |
5 | 0
```

I might also make it so that it doesn't require you to say the number of data points since I could just look at occurences of colon (separator character) and add 1
