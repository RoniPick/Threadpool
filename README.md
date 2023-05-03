# Threadpool

This project aims to implement a Threadpool that support encoder and decoder functions.   
The program reads data from stdin, encodes or decodes the data using a specified key, and writes the result to stdout.

### Implementation
The program creates a thread pool with a number of threads equal to the number of cores on the computer. Each thread in the pool waits for a task to be submitted to the queue. A task consists of a buffer of data to be encrypted/decrypted and metadata including the task index, flag, key, and buffer size.

The main thread reads the input file in chunks and creates a task for each chunk, which is then submitted to the thread pool queue. Once all tasks have been submitted, the main thread signals the threads to start processing tasks and waits for all threads to finish. The output data is then written to the output file.



### Getting Started
To run this project, you'll need to have a C compiler installed on your machine.
This program does not support ARM architecture! 

### Usage
The program takes 2 arguments:

- key: an integer representing the number of positions to shift each character in the file.
- flag: a string indicating whether to encrypt or decrypt the file. Use "-e" to encrypt and "-d" to decrypt.

### How to Run? 

In the terminal type `make clean`, and then `make` , to run the encoder function type `./coder * -e < #` 
, to run the encoder function type `./coder * -d < #`  
where * represents the key mention above, and # represent the file you wish to encode/decode.
  If you wish to write the output to a new file, type `./coder * -e < # > #`
  If you wish to see the time different from the user to the threadpool, use `time` function.   
  For example type:  
  ```
  time ./coder 6 -e < test.txt
  ```

### Output Example
<img width="1191" alt="צילום מסך 2023-05-03 ב-10 52 55" src="https://user-images.githubusercontent.com/92802335/235860109-2b2aae4d-c570-4718-be12-a7e529d5f2a4.png">


