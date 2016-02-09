/*
 *  ************************************
 *  * LIL B IS GOD SHARE ALIKE LICENSE *
 *  ************************************
 *
 * Any party is free to use and share this work
 * or any part of it under the condition that:
 *
 * 1. 
 *      This license is included in order to 
 *      attribute the original creator
 * 2. 
 *      This license is included in order to acknowledge 
 *      LIL B the Based God as your lord and savior
 *
 * Creator: Nick Topper
 *
 * I Herby Acknowledge LIL B the Based God as my Lord and Savior
 */

#include <lib.h>  
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include  <sys/wait.h>
#include "svlib.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int MY_SV; //stores the shared_value used by setsv in all tests

int run_test(char *test_name, int (*f)()) {
    /*
        run a test and report sucess or failure
    */

    printf("Running %s\n", test_name);

    int status = f();
    if(status == 1) { //1 is pass
        printf("%s %s %s", ANSI_COLOR_GREEN, "PASSED\n\n", ANSI_COLOR_RESET "\n");
    }
    else if (status == 0) { //0 is fail
        printf("%s %s %s", ANSI_COLOR_RED,     "FAILED\n\n",     ANSI_COLOR_RESET "\n");
    }
    //all other codes are NA

    return status == 1;
}

/*
    this will pass if you implemented your system calls
    the same way I did. Otherwize, modify this to look like 
    your syscall.
*/
int test_set_sv_syscall() {
    /*
        directly calls setsv syscall using global MY_SV
        asserts that the call has an OK status
    */

    MY_SV = rand();
    message m;
    m.m1_i1 = MY_SV;
    
    printf("Setting shared_value to %d\n", MY_SV);
    int status = _syscall(PM_PROC_NR, SETSV, &m);

    printf("Expected Status: 0\n");
    printf("Status: %d\n", status);

    return status == 0;
}


/*
    this will pass if you implemented your system calls
    the same way I did. Otherwize, modify this to look like 
    your syscall.
*/
int test_get_sv_syscall() { 
    /*
        directly calls getsv syscall using it's own PID
        asserts that the shared_value is MY_SVp_sv = get_sv()
    */

    int my_pid = getpid();
    printf("Current PID: %d\n", my_pid);

    message m;
    m.m1_i1 = my_pid;
    int status = _syscall(PM_PROC_NR, GETSV, &m);

    int sval = m.m1_i1;
    printf("Expected Status: 0\n");
    printf("Status: %d\n", status);

    printf("Expected shared_value: %d\n", MY_SV);
    printf("shared_value: %d\n", sval);

    return MY_SV == sval;
}

/*
    this will pass if your setsv library
    function does not fail
*/
int test_svlib_setsv() {
    /*
        same as test_set_sv_syscall
        but uses library function in svlib.h

        assert OK status
    */

    MY_SV = rand();
    int status;

    printf("Setting shared_value to %d\n", MY_SV);
    set_sv(MY_SV, &status);

    printf("Expected Status: 0\n");
    printf("Status: %d\n", status);

    return status == 0;
}

/*
    this will pass if your setsv and getsv 
    are both implemented correctly
*/
int test_svlib_getsv() {
    /*
        same as test_get_sv_syscall
        but uses library function in svlib.h

        assert shared_value is MY_SC and OK status
    */

    int my_pid = getpid();
    int status;

    printf("Current PID: %d\n", my_pid);
    int sval = get_sv(my_pid, &status);

    printf("Expected Status: 0\n");
    printf("Status: %d\n", status);
 
    printf("Expected shared_value: %d\n", MY_SV);
    printf("shared_value: %d\n", sval);

    return MY_SV == sval && status == 0;
}

/*
    this will pass if your using the process table correctly
*/
int test_svlib_forked_processes() {
    /*
        sets the shared_val of this process to 666
        sets the shared_val of a child process
        and calls get_sv from the parent process.

        assert child shared_value = MY_SV, 
        parent shared_value = 666,
        and all calls pertaining to child process have OK status 
    */

    MY_SV = rand();

    int child_status;
    int p_set_status;
    int p_get_status;
    int p_get_child_status;
    int child_sv;
    int p_sv;

    int pid = fork();

    if (pid == 0) { //we are the child

        set_sv(MY_SV, &child_status);
        printf("expected child shared value: %d\n", MY_SV);
        printf("child set_sv status: %d\n", child_status);

        return child_status == 0;
    }

    else { //parent

        sleep(.05); //give child process some time to do its thing

        //set the parent shared_value
        set_sv(666, &p_set_status);

        child_sv = get_sv(pid, &p_get_child_status);
        p_sv = get_sv(getpid(), &p_get_status);

        printf("expected parent shared value: %d\n", 666);
        printf("parent get_sv status: %d\n", p_get_child_status);
        printf("parent shared value: %d\n", p_sv);
        printf("child shared value: %d\n", child_sv);

        return child_sv == MY_SV && p_get_child_status == 0 && p_sv == 666;
    }

    return 0;
}

/*
    this will pass if you handeled your errors
    correctly in getsv
*/
int test_getsv_failure_case() {
    /*
        try to get shared value of invald pid

        assert status of 1 (error)
    */

    int status;
    printf("getting sv from PID = 0\n");
    get_sv(0, &status);

    printf("expected status: 1\n");
    printf("status: %d\n", status);

    return status;
}

int main() {

    srand(time(NULL));
    printf("\n\n UNIT TESTS\n");
    printf(" ---------- \n");
 
    if (run_test("test_set_sv_syscall", test_set_sv_syscall) &&
        run_test("test_get_sv_syscall", test_get_sv_syscall) &&
        run_test("test_svlib_setsv", test_svlib_setsv) &&
        run_test("test_svlib_getsv", test_svlib_getsv) &&
        run_test("test_getsv_failure_case", test_getsv_failure_case) &&
        run_test("test_svlib_forked_processes", test_svlib_forked_processes)) {

            printf("All Tests %s Passed %s \n\n\n",ANSI_COLOR_GREEN, ANSI_COLOR_RESET);
            printf("'WE THE BEST'\n   - DJ Kahlid\n");

    }
    else {

            printf("One or more tests %s Failed %s \n\n\n",ANSI_COLOR_RED, ANSI_COLOR_RESET);
            printf("'NEVER PLAY YOURSELF, THAT'S THE KEY'\n   - DJ Kahlid\n");

    }
    return 0;
}