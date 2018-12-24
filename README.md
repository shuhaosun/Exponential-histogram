# Exponential-histogram

The implementation of Exponential-histogram, a model that maintain aggregates and statistics  of streaming data over sliding windows. 

According to the paper : MAINTAINING STREAM STATISTICS OVER SLIDING WINDOWS（ACM–SIAM 2002）



The file DNS.txt is the real DNS Request and Response at our campus. The number of packets of the dataset is 1M and the number of distinct flows is 622968.



## How to run

Suppose you've already cloned the repository and start from the `Exponential-histogram` directory.

You just need:

```
$ make 
$ ./main
```

