# dynamic_heap
A custom dynamic heap, for reuse as an imported c library

clone code, update the main function's read test data file path to include your downloaded/execution directory, e.g. line 35 & 36

    // put your full path to test_data.bin
    unsigned char *inmemory  = get_data_for_test("test_data.bin", &file_length);
    
trivial make: gcc -o heap *.c

run executable, then update main to extend test, or to use as library in your project
