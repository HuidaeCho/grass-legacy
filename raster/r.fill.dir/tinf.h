#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>

/* to add a new multiple-type function first add three prototypes
 * (one for each type).  The functions themselves must be defined
 * elsewhere */

void set_func_pointers(int);

int is_null_c(void *);
int is_null_f(void *);
int is_null_d(void *);

int bpe_c();
int bpe_f();
int bpe_d();

void *get_min_c(void *, void *);
void *get_min_f(void *, void *);
void *get_min_d(void *, void *);

void *get_max_c(void *, void *);
void *get_max_f(void *, void *);
void *get_max_d(void *, void *);

int get_row_c(int, void *, int);
int get_row_f(int, void *, int);
int get_row_d(int, void *, int);

int put_row_c(int, void *);
int put_row_f(int, void *);
int put_row_d(int, void *);

void *get_buf_c();
void *get_buf_f();
void *get_buf_d();

void set_min_c(void *);
void set_min_f(void *);
void set_min_d(void *);

void set_max_c(void *);
void set_max_f(void *);
void set_max_d(void *);

void diff_c(void *, void *);
void diff_f(void *, void *);
void diff_d(void *, void *);

void sum_c(void *, void *);
void sum_f(void *, void *);
void sum_d(void *, void *);

void quot_c(void *, void *);
void quot_f(void *, void *);
void quot_d(void *, void *);

void prod_c(void *, void *);
void prod_f(void *, void *);
void prod_d(void *, void *);


/* to add a new multitype function, add a pointer for the function and
 * its argument list to the list below */

#ifdef TINF_MAIN
int (*is_null) (void *);
int (*bpe) ();
void *(*get_max) (void *, void *);
void *(*get_min) (void *, void *);
int (*get_row) (int, void *, int);
void *(*get_buf) ();
int (*put_row) (int, void *);
double (*slope) (void *, void *, double);
void (*set_min) (void *);
void (*set_max) (void *);
void (*diff) (void *, void *);
void (*sum) (void *, void *);
void (*quot) (void *, void *);
void (*prod) (void *, void *);
#else
extern int (*is_null) (void *);
extern int (*bpe) ();
extern void *(*get_max) (void *, void *);
extern void *(*get_min) (void *, void *);
extern int (*get_row) (int, void *, int);
extern void *(*get_buf) ();
extern int (*put_row) (int, void *);
extern double (*slope) (void *, void *, double);
extern void (*set_min) (void *);
extern void (*set_max) (void *);
extern void (*diff) (void *, void *);
extern void (*sum) (void *, void *);
extern void (*quot) (void *, void *);
extern void (*prod) (void *, void *);
#endif

/* probably not something of general interest */

double slope_c(void *, void *, double);
double slope_f(void *, void *, double);
double slope_d(void *, void *, double);

struct band3
{
    int ns;			/* samples per line */
    int sz;			/* bytes per line */
    char *b[3];			/* pointers to start of each line */
};

int advance_band3(int, struct band3 *);
int retreat_band3(int, struct band3 *);
