#!/bin/bash

echo "PUT aaa ccc eee"
time ./test_put localhost aaa bbb ccc ddd
echo "GET aaa ccc eee"
time ./test_get localhost aaa ccc
sleep 1
echo
echo "====================="
echo "PUT aaa ccc eee"
time ./test_put localhost aaa zzz ccc yyy
echo "GET aaa ccc eee"
time ./test_get localhost aaa ccc

#echo "PUT ggg iii kkk"
#time ./test_put localhost ggg hhh iii jjj kkk lll
#echo "GET aaa ggg eee ccc jjj"
#time ./test_get localhost aaa ggg ccc jjj

