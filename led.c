#include <stdio.h>
#include <unistd.h>


int main(){
printf("\n LED Flash Start \n");
FILE *LEDHandle = NULL;
const char *LEDBrightness="/sys/class/leds/beaglebone:green:usr0/brightness";


for(int i=0; i<10; i++){
    if((LEDHandle = fopen(LEDBrightness, "r+")) != NULL){
        fwrite("1", sizeof(char), 1, LEDHandle);
        fclose(LEDHandle);
    }
    usleep(1000000);
    
    
    if((LEDHandle = fopen(LEDBrightness, "r+")) != NULL){
        fwrite("0", sizeof(char), 1, LEDHandle);
        fclose(LEDHandle);
    }
    usleep(1000000);
}
printf("\n LED Flash End \n");
}
