#include "codec.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <threadpool.h>

// TODO: pthread create - when doing this, dont forget to initialize the struct args first, so there will be no junk values