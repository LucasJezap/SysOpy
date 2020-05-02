#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <math.h>

/**
 * Global variables
 */
int number_of_threads;
pthread_t *threads;
char *work_type;
char *input_file;
char *output_file;

int n=0,m=1;
int **image;

int number_of_pixels = 256;
int *pixels;

/**
 * Ancilliary functions helping the programme
 */

void print_time(int thread, int microseconds, int isMain);
int pthread_index(pthread_t self);
void* execute_thread_sign();
void* execute_thread_block();
void* execute_thread_interleaved();
int prepareImage();
void save_results_to_file();
void print_histogram();

/**
 * Program counting pixels using many threads 
 */
int main(int argc, char*argv[]) {
    /* parsing arguments */
    if (argc != 5) {
        printf("Wrong number of arguments: %d\n",argc);
        return -1;
    }

    if ((number_of_threads = atoi(argv[1])) == 0) {
        printf("Wrong number of threads argument: %s\n",argv[1]);
        return -1;
    }

    threads = (pthread_t *)calloc(number_of_threads,sizeof(pthread_t));

    if (strcmp(argv[2],"sign") == 0)
        work_type = argv[2];
    else if (strcmp(argv[2],"block") == 0)
        work_type = argv[2];
    else if (strcmp(argv[2],"interleaved") == 0)
        work_type = argv[2];
    else {
        printf("Wrong type argument: %s\n",argv[2]);
        return -1;
    }

    input_file = argv[3];
    output_file = argv[4];
    /* -------------------*/

    /* preparing image */
    if (prepareImage() == -1) {
        printf("Wrong image provided!\n");
        return -1;
    }
    /* -------------------*/

    /* setting time counter and creating threads */
    struct timeval st, et;
    gettimeofday(&st,NULL);

    pixels = (int *)calloc(number_of_pixels,sizeof(int));

    for (int i=0; i<number_of_pixels; i++)
        pixels[i] = 0;

    for (int i=0; i<number_of_threads; i++) {
        if (strcmp(work_type,"sign") == 0)
            pthread_create(&threads[i],NULL,execute_thread_sign,NULL);
        else if (strcmp(work_type,"block") == 0)
            pthread_create(&threads[i],NULL,execute_thread_block,NULL);
        else
            pthread_create(&threads[i],NULL,execute_thread_interleaved,NULL);
    }
    /* -------------------*/

    /* waiting for threads to end and getting results back */
    for (int i=0; i<number_of_threads; i++) {
        int returnvalue;
        pthread_join(threads[i],(void**)&returnvalue);
        print_time(i,returnvalue,0);
    }
    /* -------------------*/

    /* saving results to file and printing program time duration */
    save_results_to_file();    

    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec-st.tv_sec) * 1e6) + (et.tv_usec-st.tv_usec);
    print_time(-1,elapsed,1);

    /* -------------------*/

    /* optionally printing histogram */
    print_histogram();
    /* -------------------*/

    free(image);
    free(threads);
    free(pixels);

    return 0;
}

/**
 * Function converting microseconds to seconds,milliseconds and microseconds and printing the results
 */
void print_time(int thread, int microseconds, int isMain) {
    int seconds = microseconds / 1e6;
    microseconds -= seconds * 1e6;
    int miliseconds = microseconds / 1e3;
    microseconds -= miliseconds * 1e3;
    if (!isMain)
        printf("Thread %d operating time: %d seconds %d milliseconds %d microseconds\n",thread,seconds,miliseconds,microseconds);
    else
        printf("\n\nThe main thread operating time: %d seconds %d milliseconds %d microseconds\n",seconds,miliseconds,microseconds);
}

/**
 * Function returning an index of the thread in threads array
 */
int pthread_index(pthread_t self) {
    for (int i=0; i<number_of_threads; i++) {
        if (pthread_equal(self,threads[i]))
            return i;
    }
    return -1;
} 

/**
 * Function on which thread operates if work mode is SIGN
 */
void* execute_thread_sign() {
    int thread_index = pthread_index(pthread_self());

    int left = thread_index * (number_of_pixels / number_of_threads + 1);
    int right = left + number_of_pixels / number_of_threads;

    struct timeval st, et;
    gettimeofday(&st,NULL);

    for (int i=0; i<n; i++) {
        for (int j=0; j<m; j++) {
            if (image[i][j] >= left && image[i][j] <= right) {
                pixels[image[i][j]]++;
            }
        }
    }
    gettimeofday(&et,NULL);

    int elapsed = ((et.tv_sec-st.tv_sec) * 1e6) + (et.tv_usec-st.tv_usec);
    pthread_exit((void *) (intptr_t) elapsed);
}

/**
 * Function on which thread operates if work mode is BLOCK
 */
void* execute_thread_block() {
    int thread_index = pthread_index(pthread_self());

    int left = thread_index * (int) ceil(m/number_of_threads);
    int right = (thread_index + 1) * (int) ceil(m/number_of_threads) - 1;

    struct timeval st, et;
    gettimeofday(&st,NULL);

    for (int i=0; i<n; i++) {
        for (int j=left; j<=right; j++) {
            pixels[image[i][j]]++;
        }
    }
    gettimeofday(&et,NULL);

    int elapsed = ((et.tv_sec-st.tv_sec) * 1e6) + (et.tv_usec-st.tv_usec);
    pthread_exit((void *) (intptr_t) elapsed);
}

/**
 * Function on which thread operates if work mode is INTERLEAVED
 */
void* execute_thread_interleaved() {
    int thread_index = pthread_index(pthread_self());

    int start = thread_index;

    struct timeval st, et;
    gettimeofday(&st,NULL);

    while (start < m) {
        for (int i=0; i<n; i++) {
            pixels[image[i][start]]++;
        }
        start += number_of_threads;
    }
    gettimeofday(&et,NULL);

    int elapsed = ((et.tv_sec-st.tv_sec) * 1e6) + (et.tv_usec-st.tv_usec);
    pthread_exit((void *) (intptr_t) elapsed);
}

/**
 * Function parsing image from PGM to 2d array
 */
int prepareImage() {
    FILE *im;

    if ((im = fopen(input_file,"r")) == NULL) {
        printf("Wrong input file provided: %s\n",input_file);
    }

    char chunk[2000];

    for (int i=0; i<2; i++) {
        if (fgets(chunk,sizeof(chunk),im) == NULL) {
            printf("Error while reading line of file\n");
            return -1;
        }
    }

    char *buf;
    buf = strtok(chunk," ");

    if ((m = atoi(buf)) < 0) {
        printf("Wrong width size!\n");
        return -1;
    }

    buf = strtok(NULL," ");

    if ((n = atoi(buf)) < 0) {
        printf("Wrong height size!\n");
        return -1;
    }

    if (fgets(chunk,sizeof(chunk),im) == NULL) {
        printf("Error while reading line of file\n");
        return -1;
    }

    if (atoi(chunk) > 255) {
        printf("The maximum value is higher than 255\n");
        return -1;
    }

    image = (int **)calloc(n,sizeof(int*));

    for (int i=0; i<n; i++)
        image[i] = (int *)calloc(m,sizeof(int));

    int i = 0;
    while (fgets(chunk,sizeof(chunk),im) != NULL) {
        int j=0;
        char *token;
        token = strtok(chunk," ");

        while (token != NULL) {
            image[i][j++] = atoi(token);
            token = strtok(NULL," ");
        }

        if (j != m) {
            printf("The image is not %dx%d square matrix! %d %d\n",n,m,i,j);
            return -1;
        }
        i++;
    }

    fclose(im);
    return 0;
}

/**
 * Function writing the results to the given output file
 */
void save_results_to_file() {
    FILE *result = fopen(output_file,"w");
    fwrite("P2\n",3,1,result);
    char line[15];
    sprintf(line,"%d %d\n",2,number_of_pixels);
    fwrite(line,strlen(line),1,result);

    int max = 0;
    for (int i=0; i<number_of_pixels; i++)
        if(pixels[i] > max)
            max = pixels[i];
    
    sprintf(line,"%d\n",max);
    fwrite(line,strlen(line),1,result);

    for(int i=0; i<number_of_pixels; i++) {
        sprintf(line,"%d %d\n",i,pixels[i]);
        fwrite(line,strlen(line),1,result);
    }

    fclose(result);
}

/**
 * Function printing the histogram of the results
 */
void print_histogram() {
    char decision;
    printf("Do you want to see the histogram of results? [y/n]\n");
    decision = getc(stdin);
    if (decision == 'n')
        return;

    int max = 0;
    for (int i=0; i<number_of_pixels; i++)
        if(pixels[i] > max)
            max = pixels[i];
    
    double ratio = 1.0 * max / 100.0;

    for (int i=0; i<number_of_pixels; i++) {
        int len = pixels[i] / ratio;
        printf("%10s"," ");
        for (int j=0; j<len; j++) printf("-");
        printf("\nPixel %3d |",i);
        for (int j=0; j<len-2; j++) printf(" ");
        printf("| %d\n",pixels[i]);
        printf("%10s"," ");
        for (int j=0; j<len; j++) printf("-");
        printf("\n");
    }
}