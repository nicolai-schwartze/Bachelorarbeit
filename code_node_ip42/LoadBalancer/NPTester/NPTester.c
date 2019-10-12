

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>



int main (void) {
    
    struct timeval tv1, tv2;
    double time_elapsed = 0.0;
    
    char task1[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/databaseAccess";
    char task2[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/websiteCall";
    char task3[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/imageProcessing";
    char task4[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/averageCalc";
    char task5[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/sendEmail";
    char task6[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/userInput";
    char task7[] = "/var/lib/cloud9/c_cpp_projects/LoadBalancer/loadBalancingTask/blinky";
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task1);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("databaseAccess average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task2);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("websiteCall average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task3);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("imageProcessing average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task4);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("averageCalc average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task5);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("sendEmail average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task6);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("userInput average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    for(int i = 0; i < 10; i++) {
        gettimeofday(&tv1, NULL);
        system(task7);
        gettimeofday(&tv2, NULL);
        time_elapsed = time_elapsed + ((double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec));
    }
    time_elapsed = time_elapsed / 10;
    printf("blinky average time = %lf \n", time_elapsed);
    time_elapsed = 0.0;
    
    return 0;
}