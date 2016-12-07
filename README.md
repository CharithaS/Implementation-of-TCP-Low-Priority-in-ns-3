# Implementation of TCP Low Priority (TCP-LP) in ns-3

## Course Code: CS822

## Assignment: #FP3

### Overview:

TCP-LP is a TCP variant for Low priority data transfer in which the low priority data utilises only the excess bandwidth [1] and has been implemented in ns-2 [2]. This repository contains an implementation of TCP-LP in ns-3 [3].

### TCP-LP Example:

An example program for TCP-LP has been provided in 
```
examples/tcp/tcp-variants-comparison.cc
```
and should be executed as
```
./waf --run "tcp-variants-comparison.cc --transport_prot=TcpLp"
```
### References:

[1] Aleksandar Kuzmanovic, Edward W. Knightly.(2003).TCP-LP: A Distributed Algorithm for Low Priority
Data Transfer.IEEE INFOCOM 2003.

[2] http://www.isi.edu/nsnam/ns/

[3] http://www.nsnam.org/
