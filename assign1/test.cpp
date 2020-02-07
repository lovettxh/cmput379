#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <iostream>
#include <stdint.h>
using namespace std;


int main(){
    long long i = 1999999999;
    while(i > 0){
        i--;
    }
    cout<<"done!"<<endl;

    return 0;
}