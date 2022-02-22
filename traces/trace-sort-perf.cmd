# Test performance of sort compare to linux queue sort
option fail 0
option malloc 0
new
ih dolphin 1000000
it gerbil 1000000
time sort
reverse
time sort
time sort
reverse
time sort
new
ih RAND 100000
time sort
ih RAND 100000
time sort
option linux_qsort 1
new
ih dolphin 1000000
it gerbil 1000000
time sort
reverse
time sort
time sort
new
ih RAND 100000
time sort
ih RAND 100000
time sort
