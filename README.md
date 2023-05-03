# Threadpool

This project aims to implement a Threadpool that support encoder and decoder functions.


### Getting Started
To run this project, you'll need to have a C compiler installed on your machine.
This program does not support ARM architecture! 

### How to Run? 

In the terminal type `make clean`, and then `make` , to run the encoder function type `./coder * -e < #` 
, to run the encoder function type `./coder * -d < #`  
where * is a Natural number, each number represent different encoder function from the library, and # represent the file you wish to encode/decode.
  If you wish to write the output to a new file, type `./coder * -e < # > #`
  If you wish to see the time different from the user to the threadpool, use `time` function. For example, type `time ./coder 4 -e < test.txt`

### Example
<img width="1191" alt="צילום מסך 2023-05-03 ב-10 52 55" src="https://user-images.githubusercontent.com/92802335/235860109-2b2aae4d-c570-4718-be12-a7e529d5f2a4.png">


