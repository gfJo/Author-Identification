# Assignment 7 - Author Identification
This program attempts to identify the most likely authors for an anonymous sample of text given a large database of texts with known authors using hash tables and bloom filters.
## Building
First use “make” to build an executable program, and the Makefile is provided in the directory.
## Running
```
$ ./identify [OPTIONS]
```
Below are several options that could be used.

  -d database     Database of authors and texts [default: lib.db]
 
  -k matches      Set the number of top matches to display [default: 5]
 
  -n noisefile    Set file of words to ignore [default: noise.txt]
 
  -l limit        Set noise word limit [default: 100]
 
  -e              Set distance metric as Euclidean distance [default]
 
  -m              Set distance metric as Manhattan distance
 
  -c              Set distance metric as cosine distance
 
  -v              Enable verbose printing of program run
 
  -h              Display program help and usage
 
Example Input:
```
$ ./identify -d medium.db < texts/anonymous.txt
```
Example Output:
```
Top 5, metric: Euclidean distance, noise limit: 100
1) Anonymous [0.000000000000000]
2) William Shakespeare [0.081973506882896]
3) Wilfred Owen [0.083778622777966]
4) Christopher Marlowe [0.085202107739290]
5) George Bernard Shaw [0.089104442381203]
```
## Cleaning
Use
```
$ make clean
```
which is provided by Makefile.
## Bugs or error
Segment Fault and memory leak exists when running the program. The distances data from the program is different from the data from resources. The program can't run the database.db, which can only use the small.db and medium.db.
 
