#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int rows1, cols1, rows2, cols2;
int **matrixA;
int **matrixB;
int **resultMatrix;

// Structure to pass arguments to the thread function
typedef struct {
    int row;
    int column;
} ThreadArgs;


void read_matrices_from_file(char *);
void free_matrices();
void *compute_one_element(void *);
void *compute_row(void *);
void initialize_result_matrix();
void procedure_1(double*);
void procedure_2(double*);
void print_result_matrix();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    

    double element_time_elapsed, row_time_elapsed;

    read_matrices_from_file(argv[1]);
    if(cols1 != rows2){
        printf("Matrix Multiplication is not possible !!\n");
        return 1;
    }
    initialize_result_matrix();
  

    // First Variation - Element Threads
    procedure_1(&element_time_elapsed);
    printf("By element\n");
    print_result_matrix();
    printf("Time: %f\n", element_time_elapsed);
    initialize_result_matrix();

    // Second Variation - Row Threads
    procedure_2(&row_time_elapsed);
    printf("By row\n");
    print_result_matrix();
    printf("Time: %f\n", row_time_elapsed);
    
    // Comparing the time elapsed
    if(row_time_elapsed > element_time_elapsed){
        printf("First Variation is faster\n");
    }
    else{
        printf("Second Variation is faster\n");
    }
    // Function to free the memory used by the matrices
    free_matrices();
    pthread_exit(NULL);
    return 0;
}
// Function to read the input matrices from a file
void read_matrices_from_file(char *filename) {
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) {
        printf("Error opening input file.\n");
        return;
    }

    

    // Read the number of rows and columns of the first matrix
    fscanf(input_file, "%d %d", &rows1, &cols1);

    // Allocate memory for the first matrix
    matrixA = (int **)malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++) {
        matrixA[i] = (int *)malloc(cols1 * sizeof(int));
    }

    // Read the entries of the first matrix
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols1; j++) {
            fscanf(input_file, "%d", &matrixA[i][j]);
        }
    }

    // Read the number of rows and columns of the second matrix
    fscanf(input_file, "%d %d", &rows2, &cols2);

    // Allocate memory for the second matrix
    matrixB = (int **)malloc(rows2 * sizeof(int *));
    for (int i = 0; i < rows2; i++) {
        matrixB[i] = (int *)malloc(cols2 * sizeof(int));
    }

    // Read the entries of the second matrix
    for (int i = 0; i < rows2; i++) {
        for (int j = 0; j < cols2; j++) {
            fscanf(input_file, "%d", &matrixB[i][j]);
        }
    }

    fclose(input_file);

    resultMatrix = (int **)malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++) {
        resultMatrix[i] = (int *)malloc(cols2 * sizeof(int));
    }
    
}
void free_matrices() {
    // Free matrixA
    for (int i = 0; i < rows1; i++) {
        free(matrixA[i]);
    }
    free(matrixA);

    // Free matrixB
    for (int i = 0; i < rows2; i++) {
        free(matrixB[i]);
    }
    free(matrixB);

    // Free resultMatrix
    for (int i = 0; i < rows1; i++) {
        free(resultMatrix[i]);
    }
    free(resultMatrix);

}
// Function to compute each element of the output matrix in a thread
void *compute_one_element(void *args) {
    ThreadArgs *arguments = (ThreadArgs *)args;
    int row = arguments->row;
    int column = arguments->column;
    for (int k = 0; k < cols1; k++) {
        resultMatrix[row][column] += matrixA[row][k] * matrixB[k][column];
    }
}
// Function to compute each row of the output matrix in a thread
void *compute_row(void *args) {
    int row = (int)args; 
    for (int j = 0 ; j < cols2;j++){
        for (int k = 0 ; k< cols1; k++){
            resultMatrix[row][j] += matrixA[row][k] * matrixB[k][j];
        }
    }
}



void initialize_result_matrix(){
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++){
            resultMatrix[i][j] = 0;
        }
    }
}
void procedure_1(double*time_elapsed){

    struct timeval start_time, end_time;
    int totalThreads = rows1 * cols2;

    pthread_t elementThreads[totalThreads];
    ThreadArgs elementArgs[totalThreads];

    gettimeofday(&start_time, NULL);
    for (int i = 0; i < totalThreads; i++) {
        elementArgs[i].row = i / cols2;
        elementArgs[i].column = i % cols2;
        pthread_create(&elementThreads[i], NULL, compute_one_element, &elementArgs[i]);
        pthread_join(elementThreads[i], NULL);
    }
    
    gettimeofday(&end_time, NULL);

    *time_elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;


}
void procedure_2(double*time_elapsed){
    struct timeval start_time, end_time;
    int totalThreads = rows1;

    pthread_t rowThreads[totalThreads];

    gettimeofday(&start_time, NULL);
    for(int i =0 ; i< totalThreads; i++){
        pthread_create(&rowThreads[i], NULL, compute_row, (void *)i);
        pthread_join(rowThreads[i], NULL);
    }
    gettimeofday(&end_time, NULL);
    
    *time_elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

}
void print_result_matrix(){
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++){
            printf("%d ", resultMatrix[i][j]);
        }
        printf("\n");
    }
}
