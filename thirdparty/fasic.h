/*

This Header Only Library is a collection of libraries I find myself using often.

 */


#ifndef FASIC_H_
#define FASIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>

#define FASICSTATIC static __attribute__((unused))
#define ARRAY_LEN(array) (sizeof(array)/ sizeof(array[0]))

typedef enum {
    INFO,
    ERROR,
    DEBUG,
    WARN,
    COUNT_LEVELS,
} Log_Levels;

FASICSTATIC void Logging(FILE *stream, Log_Levels level, const char *fmt, ...);

#define Log_Out(level, fmt, ...) Logging(stdout, level, fmt, ##__VA_ARGS__)
#define Log_File(stream, level, fmt, ...) Logging(stream, level, fmt, ##__VA_ARGS__)

// Matrix Structure and Information
typedef struct _fasic_matrix {
    float *A;     // Pointer to the array of matrix elements
    size_t nrows; // Number of rows in the matrix
    size_t ncols; // Number of columns in the matrix
} Fasic_Matrix;

typedef struct _fasic_shape {
    size_t rows;
    size_t cols;
} Fasic_Shape;

/*
************** MATRIX OPERATIONS FUNCTIONS DECLARATIONS *******************
*/

// Core Functions
FASICSTATIC float get_element(Fasic_Matrix C, size_t row, size_t col);                      // get specific element from the Fasic_Matrix
FASICSTATIC void set_element(Fasic_Matrix C, size_t row, size_t col, float new_value);      // set a specific element from the Fasic_Matrix to a new value
FASICSTATIC Fasic_Matrix create_matrix(size_t rows, size_t cols);                           // creates an empty Fasic_Matrix with specified parameters
FASICSTATIC void unload(Fasic_Matrix *b);                                                   // Function to free Allocated Memory

// Utility Functions
FASICSTATIC Fasic_Matrix random_matrix(size_t nrows, size_t ncols);                         // generates random Fasic_Matrix(nrows x ncols)
FASICSTATIC void print_matrix(const Fasic_Matrix b, const char *name);                      // prints a Fasic_Matrix
FASICSTATIC Fasic_Shape matrix_shape(Fasic_Matrix *A);                                             // returns shape of the Fasic_Matrix
FASICSTATIC Fasic_Matrix fill(size_t nrows, size_t ncols, float fill_value);                // creates a Fasic_Matrix filled with a specific value

// Operation Functions
FASICSTATIC Fasic_Matrix matrix_add(Fasic_Matrix *a, Fasic_Matrix *b);                                  // adds two matrices
FASICSTATIC Fasic_Matrix matrix_subtract(Fasic_Matrix *a, Fasic_Matrix *b);                             // subtracts two matrices
FASICSTATIC Fasic_Matrix hadamard_product(Fasic_Matrix *a, Fasic_Matrix *b);                            // computes element-wise product of two matrices
FASICSTATIC Fasic_Matrix dot_product(Fasic_Matrix *a, Fasic_Matrix *b);                                 // computes the dot-product of two matrices
FASICSTATIC Fasic_Matrix transpose(Fasic_Matrix *a);                                              // return transpose of Fasic_Matrix a
FASICSTATIC bool test_matrix_equal(Fasic_Matrix a, Fasic_Matrix b);                               // test matrices for equality


// Special Macro Functions
#define PRINT(B) print_matrix(B, #B)                                /* Macro definition of a special print function */
#define ALLOCATION_FAILED "Memory Allocation Failed.\n"             /* Return Error String */

/* Macro function For Printing the shape of a Fasic_Matrix */
#define PRINT_SHAPE(A)\
    do {\
        Fasic_Shape shape = matrix_shape(&A);\
        printf("%s Shape: [%zu, %zu]\n",#A, shape.rows, shape.cols);\
    } while (0)

#define BUCKETS_SIZE 100000

typedef struct Dictionary {
    char *key;
    unsigned int value;
} Dictionary;

typedef struct HashMap {
	Dictionary *buckets;
	unsigned int count;
} HashMap;

FASICSTATIC unsigned int hash_function(char *key);
FASICSTATIC bool hashmap_init(HashMap *hashmap);
FASICSTATIC bool hashmap_insert(HashMap *hashmap, char *key);
FASICSTATIC bool hashmap_delete(HashMap *hashmap, char *key);
FASICSTATIC unsigned int hashmap_lookup(HashMap *hashmap, char *key);
FASICSTATIC void hashmap_print(HashMap *hashmap);
FASICSTATIC void hashmap_destroy(HashMap *hashmap);

/*
    CUSTOM 'C' BUILD SYSTEM
*/

FASICSTATIC bool file_exists(const char *file_path);
FASICSTATIC bool remove_file(char *args[]);
FASICSTATIC bool remove_directory(const char *dir_path);
FASICSTATIC bool make_new_directory(const char *dir_path);

typedef struct {
    const char **items;
    unsigned int count;
    unsigned int capacity;
} Fasic_Cmd;

#define INITIAL_CAPACITY_ARRAY 5

#define append_to_array(array, item) \
    do {\
        if ((array)->count >= (array)->capacity) {\
            (array)->capacity = (array)->capacity == 0 ? INITIAL_CAPACITY_ARRAY : (array)->capacity*2;\
            (array)->items = (typeof(*(array)->items)*)realloc((array)->items, sizeof(*(array)->items)*(array)->capacity);\
        }\
        (array)->items[(array)->count++] = item;\
    } while(0)

#define append_to_array_many(array, items) \
    do {\
        unsigned int n = 0;\
        while(n < ARRAY_LEN(items)) {\
            append_to_array((array), items[n]);\
            n++;\
        }\
    } while (0)

FASICSTATIC bool build_c_file(const Fasic_Cmd *cmd_line);


// Custom string duplicate implementation
static inline char *strdup_custom(const char *source)
{
    size_t len = strlen(source);
    char *temp = (char *)malloc(len + 1);
    assert(temp != NULL && "source duplication Failed.");
    memcpy(temp, source,len + 1);
    return temp;
}


#ifdef __cplusplus
}
#endif

#ifdef FASIC_IMPLEMENTATION

/* CUSTOM LOGGING FUNCTIONS IMPLEMENTATION BEGIN */

FASICSTATIC const char * Levels[COUNT_LEVELS] = {
    [INFO]  = "INFO",
    [ERROR] = "ERROR",
    [DEBUG] = "DEBUG",
    [WARN]  = "WARN",
};

FASICSTATIC void Logging(FILE *stream, Log_Levels level, const char *fmt, ...)
{
    fprintf(stream, "%s: " , Levels[level]);

    va_list args;
    va_start(args, fmt);
    vfprintf(stream, fmt, args);
    va_end(args);
}

/* CUSTOM LOGGING FUNCTIONS IMPLEMENTATION ENDS */

/*
************** MATRIX OPERATIONS FUNCTIONS IMPLEMENTATIONS *******************
*/

FASICSTATIC float get_element(Fasic_Matrix C, size_t row, size_t col) {
    // check for valid indices
    if (row >= C.nrows || col >= C.ncols) {
        Log_File(stderr, ERROR, "index out of bounds: [%zu, %zu] for Fasic_Matrix size [%zu, %zu]\n",
                row, col, C.nrows, C.ncols);
        exit(EXIT_FAILURE); // handle the error, exit or return an error value
    }

    return C.A[row * C.ncols + col];
}

FASICSTATIC void set_element(Fasic_Matrix C, size_t row, size_t col, float new_value) {
    // set the element at the specified row and column to a new value
    C.A[row * C.ncols + col] = new_value;
}

FASICSTATIC Fasic_Matrix create_matrix(size_t rows, size_t cols) {
    Fasic_Matrix create;
    create.nrows = rows;
    create.ncols = cols;
    create.A = (float *)calloc(sizeof(float), (rows * cols));
    if (create.A == NULL) {
        Log_File(stderr, ERROR, ALLOCATION_FAILED);
        exit(EXIT_FAILURE);
    }

    return create;
}

FASICSTATIC Fasic_Matrix random_matrix(size_t nrows, size_t ncols) {
    // initialize the Fasic_Matrix
    Fasic_Matrix random = create_matrix(nrows, ncols);

    // fill the Fasic_Matrix with random values
    for (size_t i = 0; i < random.nrows; ++i) {
        for (size_t j = 0; j < random.ncols; ++j) {
            float v = ((float)rand() /(float) RAND_MAX) * 1000 - 500;
            set_element(random, i , j , v);
        }
    }

    return random; // return the generated Fasic_Matrix
}

FASICSTATIC void print_matrix(const Fasic_Matrix b, const char *name) {
    // print the Fasic_Matrix with its name
    printf("%s = [\n", name);

    for (size_t i = 0; i < b.nrows; ++i) {
        for (size_t j = 0; j < b.ncols; ++j) {
            float value = get_element(b, i , j);
            printf(" %6.2lf  ", value);
        }
        printf("\n");
    }
    printf("]"); // end of Fasic_Matrix
    printf("\n\n");
}

FASICSTATIC Fasic_Shape matrix_shape(Fasic_Matrix *A) {
    // shape of the Fasic_Matrix (rows, columns)
    return (Fasic_Shape) {A->nrows, A->ncols};
}

FASICSTATIC Fasic_Matrix matrix_add(Fasic_Matrix *A, Fasic_Matrix *B) {
    // check if the dimensions of the matrices are the same
    assert((A->ncols == B->ncols && B->nrows == A->nrows) && "addition failed, matrices shape mismatch");

    // initialize a new Fasic_Matrix for the result
    Fasic_Matrix C = create_matrix(A->nrows, A->ncols);

    for (size_t i = 0; i < A->nrows; ++i) {
        for (size_t j = 0; j < A->ncols; ++j) {
            float a = get_element(*A, i , j);
            float b = get_element(*B, i , j);
            float c = a + b;
            set_element(C, i , j , c);
        }
    }

    return C; // return the result Fasic_Matrix
}

FASICSTATIC Fasic_Matrix matrix_subtract(Fasic_Matrix *A, Fasic_Matrix *B) {
    // check if the dimensions of the matrices are the same
    assert((A->ncols == B->ncols && B->nrows == A->nrows) && "subtraction failed, matrices shape mismatch");

    // initialize a new Fasic_Matrix for the result
    Fasic_Matrix C = create_matrix(A->nrows, A->ncols);

    for (size_t i = 0; i < A->nrows; ++i) {
        for (size_t j = 0; j < A->ncols; ++j) {
            float a = get_element(*A, i , j);
            float b = get_element(*B, i , j);
            float c = a - b;
            set_element(C, i , j , c);
        }
    }

    return C; // return the result
}

FASICSTATIC Fasic_Matrix hadamard_product(Fasic_Matrix *A, Fasic_Matrix *B) {
    // ensure matrices are of the same size and type
    assert(A->ncols == B->ncols && "matrices dimensions mismatch");
    assert(A->nrows == B->nrows && "matrices dimensions mismatch");

    // initialize a new Fasic_Matrix for the result
    Fasic_Matrix C = create_matrix(A->nrows, A->ncols);

    // calculate element-wise product
    for (size_t i = 0; i < A->nrows; ++i) {
        for (size_t j = 0; j < A->ncols; ++j) {
            float a , b , c;
            a = get_element(*A, i , j);
            b = get_element(*B, i , j);
            c = a * b;
            set_element(C, i , j , c);
        }
    }

    return C; // return the result Fasic_Matrix
}

FASICSTATIC Fasic_Matrix dot_product(Fasic_Matrix *A, Fasic_Matrix *B) {
    // check if the matrices can be multiplied
    assert((A->ncols == B->nrows) && "error multiplying: dimensions mismatch");

    // initialize a new Fasic_Matrix for the result
    Fasic_Matrix C = create_matrix(A->nrows, B->ncols);

    // calculate the dot product
    for (size_t i = 0; i < C.nrows; ++i) {
        for (size_t j = 0; j < C.ncols; ++j) {
            float c = 0;
            for (size_t k = 0; k < A->ncols; ++k) {
                float a , b;
                a = get_element(*A, i , k );
                b = get_element(*B, k , j );
                c += a * b;
            }
            set_element(C , i , j , c);
        }
    }

    return C; // return the result Fasic_Matrix
}

FASICSTATIC Fasic_Matrix transpose(Fasic_Matrix *A) {
    // initialize a new Fasic_Matrix for the result
    Fasic_Matrix C = create_matrix(A->ncols, A->nrows);

    for (size_t i = 0; i < A->nrows; ++i) {
        for (size_t j = 0; j < A->ncols; ++j) {
            float a;
            a = get_element(*A, i , j);
            set_element(C, j , i , a);
        }
    }

    return C; // return the transposed Fasic_Matrix
}

FASICSTATIC bool test_matrix_equal(Fasic_Matrix A, Fasic_Matrix B) {
    // check if the dimensions match
    assert(((A.ncols == B.ncols) || (A.nrows == B.nrows))&& "not equal: different dimensions.");

    // check if all elements are equal
    for (size_t i = 0; i < A.nrows; ++i) {
        for (size_t j = 0; j < A.ncols; ++j) {
            float a , b;
            a = get_element(A, i, j);
            b = get_element(B, i, j);
            if (a != b) {
                return false; // not equal
            }
        }
    }

    return true; // equal
}

FASICSTATIC Fasic_Matrix fill(size_t nrows, size_t ncols, float fill_value) {
    // initialize Fasic_Matrix
    Fasic_Matrix a = create_matrix(nrows, ncols);

    // fill the Fasic_Matrix with the specified value
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t j = 0; j < ncols; ++j) {
            set_element(a, i , j , fill_value); // set all elements
        }
    }

    return a; // return the filled Fasic_Matrix
}

FASICSTATIC void unload(Fasic_Matrix *B) {
    // free allocated memory for the Fasic_Matrix
    if (B->A != NULL)
    {
        free(B->A); // free the array of elements
        B->A = NULL; // avoid dangling pointer
    }

    B->nrows = 0; // reset rows
    B->ncols = 0; // reset columns
}

/* MATRIX FUNCTION IMPLEMENTATIONS ENDS */


/* HASHMAP FUNCTION IMPLEMENTATION BEGINS */
FASICSTATIC unsigned int hash_function(char *key)
{
    unsigned int hash = 0;
    unsigned int slen = strlen(key);

    for (unsigned int i = 0; i < slen && key[i] != '\0'; ++i) {
        hash = 31 * hash + key[i];
    }

    return hash % BUCKETS_SIZE;
}

FASICSTATIC bool hashmap_init(HashMap *hashmap)
{
    hashmap->buckets = (Dictionary *)calloc(BUCKETS_SIZE , sizeof(Dictionary));
    if (!hashmap->buckets) return false;
    hashmap->count = 0;
    return true;
}

FASICSTATIC bool hashmap_insert(HashMap *hashmap, char *key)
{
    unsigned int hash = hash_function(key);
    Dictionary *bucket = &hashmap->buckets[hash];

    if (bucket->key == NULL) {
        unsigned int slen = strlen(key);
        bucket->key = (char *)malloc(sizeof(char)* (slen + 1));
        if (bucket->key == NULL) {
            free(bucket->key);
            return false;
        }
        strcpy(bucket->key, key);
        bucket->value = 1;
        hashmap->count++;
        return true;
    } else if (strcmp(bucket->key, key) == 0) {
        bucket->value +=1;
        return true;
    }

    for (unsigned int i = 1; i < BUCKETS_SIZE; ++i) {
        unsigned int idx = (hash + i) % BUCKETS_SIZE;
        bucket = &hashmap->buckets[idx];
        if (bucket->key == NULL) {
            unsigned int slen = strlen(key);
            bucket->key = (char *)malloc(sizeof(char)* (slen + 1));
            if (bucket->key == NULL) {
                free(bucket->key);
                return false;
            }
            strcpy(bucket->key, key);
            bucket->value = 1;
            hashmap->count++;
            return true;
        } else if (strcmp(bucket->key, key) == 0) {
            bucket->value +=1;
            return true;
        }
    }
    return false;
}

FASICSTATIC bool hashmap_delete(HashMap *hashmap, char *key)
{
    unsigned int hash = hash_function(key);
    Dictionary *temp = &hashmap->buckets[hash];
    if (temp == NULL) {
        return false;
    }

    free(hashmap->buckets[hash].key);
    hashmap->buckets[hash].key = NULL;
    hashmap->buckets[hash].value = 0;
    hashmap->count--;

    return true;
}

FASICSTATIC unsigned int hashmap_lookup(HashMap *hashmap, char *key)
{
    if (!hashmap) exit(EXIT_FAILURE);

    unsigned int hash = hash_function(key);
    for (unsigned int i = 0; i < BUCKETS_SIZE; ++i) {
        unsigned int index = (hash + i) % BUCKETS_SIZE;
        Dictionary *bucket = &hashmap->buckets[index];

        if (bucket->key == NULL) {
            return 0;
        }

        else if (strcmp(key, bucket->key) == 0) {
            return bucket->value;
        }
    }
    return 0;
}

FASICSTATIC void hashmap_print(HashMap *hashmap)
{
    for (unsigned int i = 0; i < BUCKETS_SIZE; ++i) {
        Dictionary *temp = &hashmap->buckets[i];
        if (temp->key != NULL) {
            unsigned int hash = hash_function(temp->key);
            Log_Out(INFO, "Index: %5d, K: %10s, V: %5d\n",hash, temp->key, temp->value);
        }
    }
}

FASICSTATIC void hashmap_destroy(HashMap *hashmap)
{
    for (unsigned int i = 0; i < BUCKETS_SIZE; ++i) {
        free(hashmap->buckets[i].key);
    }
    free(hashmap->buckets);
    hashmap->count = 0;
}

/* HASHMAP FUNCTION IMPLEMENTATION ENDS */

FASICSTATIC bool file_exists(const char *file_path)
{
    if (access(file_path, F_OK) != 0) {
        Log_Out(INFO, "File `%s` does not exist.\n", file_path);
        return false;
    }
    return true;
}

FASICSTATIC bool remove_file(char *args[])
{
    pid_t pid = fork();
    if (pid == -1) {
        Log_File(stderr, ERROR, "Fork Failed: %s.\n", strerror(errno));
        return false;
    }

    if (pid == 0) {
        if (file_exists(args[1])){
            Log_Out(INFO, "Removing `%s` file...\n", args[1]);
            execvp(args[0], args);
            Log_File(stderr, ERROR, "Removing File `%s` Failed: %s.\n",args[1], strerror(errno));
            _exit(EXIT_FAILURE);
        }
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

FASICSTATIC bool remove_directory(const char *dir_path)
{
    struct stat sb;

    if (stat(dir_path, &sb) != 0) {
        if (errno == ENOENT) {
            // Directory Does not exist
            return true;
        }
        Log_Out(ERROR, "Checking Directory `%s`: %s.\n", dir_path, strerror(errno));
        return false;
    }

    if (!S_ISDIR(sb.st_mode)) {
        Log_Out(WARN, "`%s` is Not a Directory.\n", dir_path);
        return false;
    }

    DIR *fdir = opendir(dir_path);
    if (!fdir) {
        Log_Out(ERROR, "Cannot Open Directory `%s`: %s.\n", dir_path, strerror(errno));
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(fdir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
        strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            // if there sub-directories, recursively delete
            if (!remove_directory(path)) {
                closedir(fdir);
                return false;
            }
        } else {
            if (unlink(path) != 0) {
                Log_Out(ERROR, "Failed to Delete `%s` path: %s\n", path, strerror(errno));
                closedir(fdir);
                return false;
            }
        }
    }

    closedir(fdir);

    if (rmdir(dir_path) == -1) {
        Log_Out(INFO, "Final Directory `%s` Removal Failed.\n", dir_path);
        return false;
    }

    Log_Out(INFO, "Successfully Removed `%s` Directory.\n", dir_path);
    return true;
}

FASICSTATIC bool make_new_directory(const char *dir_path)
{
    struct stat sb;

    if (stat(dir_path, &sb) == 0) {
        if (S_ISDIR(sb.st_mode)) {
            Log_Out(INFO, "Directory \'%s\' already Exists.\n", dir_path);
            return true;
        }
        Log_Out(WARN, "`%s` Exists But is Not a directory.\n", dir_path);
        return false;
    }

    if (mkdir(dir_path, 0775) == -1) {
        Log_Out(INFO, "Failed to Create `%s` directory.\n", dir_path);
        return false;
    }

    Log_Out(INFO, "Created New Directory `%s`.\n", dir_path);
    return true;
}

FASICSTATIC bool build_c_file(const Fasic_Cmd *cmd_line)
{
    pid_t pid = fork();
    if (pid == -1) {
        Log_File(stderr, ERROR, "Fork Failed: %s.\n", strerror(errno));
        return false;
    }

    if (pid == 0) {
        Log_Out(INFO, "");
        for(unsigned int i = 0; i < cmd_line->count && cmd_line->items[i] != NULL; ++i) {
            printf("%s ", cmd_line->items[i]);
        }
        printf("\n");
        execvp(cmd_line->items[0], (char * const *)cmd_line->items);
        Log_File(stderr, ERROR, "Build Failed: %s.\n", strerror(errno));
        _exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

#endif // FASIC_IMPLEMENTATION

#ifndef UPPERCASE_GUARD
#define UPPERCASE_GUARD
#ifdef UPPERCASE_
#define UNLOAD unload
#define CREATE_MATRIX create_matrix
#define TRANSPOSE transpose
#define MATRIX_ADD matrix_add
#define MATRIX_SUBTRACT matrix_subtract
#define DOT_PRODUCT dot_product
#define HADAMARD_PRODUCT hadamard_product
#define GET_ELEMENT get_element
#define SET_ELEMENT set_element
#define TEST_MATRIX test_matrix_equal
#endif // UPPERCASE_
#endif // UPPERCASE_GUARD

#endif // FASIC_H_

// TODO: Add prefixes to functions
// TODO: Modify the hashtable to take in Anytype
// TODO: Prefix the Hashmap functions and the logging Functions
