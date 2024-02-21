#include <pthread.h>
#include <time.h>
#include <stdio.h>

#define MAX_SAMPLES 10
#define MAX_MEASUREMENTS 100


struct {
    pthread_mutex_t mutex;
    struct timespec timestamps[MAX_SAMPLES];
    int count;
} shared = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .count = 0
};

void* pwm_listener(void* arg) {
    while(1) {
        
        pthread_mutex_lock(&shared.mutex);

        
        if (shared.count < MAX_SAMPLES) {
            clock_gettime(CLOCK_MONOTONIC, &shared.timestamps[shared.count++]);
        }

      
        pthread_mutex_unlock(&shared.mutex);
    }
}

void* period_calculator(void* arg) {
    for(int m=0; m<MAX_MEASUREMENTS; m++){
        
        pthread_mutex_lock(&shared.mutex);

        
        if (shared.count == MAX_SAMPLES) {
            double total = 0;
            for (int i = 1; i < MAX_SAMPLES; i++) {
                double duration = shared.timestamps[i].tv_sec - shared.timestamps[i - 1].tv_sec;
                total += duration;
            }
            double mean = total / (MAX_SAMPLES - 1);

            
            printf("Mean period: %f seconds\n", mean);
            FILE *fp = fopen("Daniel_Botros_pwmperiods", "a");
            fprintf(fp, "%f\n", mean);
            fclose(fp);

            
            shared.count = 0;
        }

        
        pthread_mutex_unlock(&shared.mutex);
    }
    return NULL;
}

int main() {
    for(int n=0; n<MAX_MEASUREMENTS; n++){
        pthread_t thread1, thread2;

        pthread_create(&thread1, NULL, pwm_listener, NULL);
        pthread_create(&thread2, NULL, period_calculator, NULL);

        pthread_join(thread1, NULL);
        pthread_join(thread2, NULL);
    }
    return 0;
}
