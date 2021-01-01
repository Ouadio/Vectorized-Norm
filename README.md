## Real Numbers Vector Norm Computation
----
### Goal of the project
The main goal of this POC developed in C language is to demonstrate and benchmark, the power of parallelization methods through **Multithreading** in a **shared memory** system with *POSIX Thread* API, and through **SIMD** *(Single Instruction Multiple Data)* by the mean of vectorization techniques on the register's level *(AVX)*.  

### Main components
The benchmark of different parallelization strategies being the main goal, our approach was basically to implement different versions of the *norm* algorithm and compare their performance and accuracy.  
The 4 versions are : 
+ **Sequential Implementation** [ norm(float *U, int n) ] : Base line implementation for our benchmark having no explicit parallelization inside.
+ **SIMD Implementation** [ vect_norm(float *U, int n) ] : Exploiting Register's Vector level parallelization (SIMD) to process blocks of 8 floats in one clock-cycle. Un-aligned loading is supported to avoid SEG errors when input is not aligned. **Theoretical speedup : 8x | AVX256**.
+ **Multi-threaded Implementation** [ multi_thr_norm(float \*U, int n, int nb_threads, int mode = 0) ] : Exploiting processor's Multi-cores to spawn different threads to run in parallel on physically distinct *(or not, through hyperthreading)* cores. Scheduling is done statically by equally distributing contiguous chunks of input vector on different threads. Since the input length is a multiple of 2^10 at least and the number of cores is 4/8, potential problems of false sharing are unlikely to happen since each chunk from an aligned input has a whole set of cache lines. **Theoretical speedup : 4x - 8x | 4 Cores + Hyperthreading**.
+ **Multi-threaded & Vectorized Implementation**[ multi_thr_norm(float \*U, int n, int nb_threads, int mode = 1) ] : Combination of both Multi-threaded and vectorized versions. This implementation takes advantage of two levels of parallelism : shared memory system parallelisation through multithreading, and SIMD through register vectorized operations. **Theoretical speedup : 32x - 64x | Same config as before**

--------------
Ouadie EL FAROUKI, 2020.