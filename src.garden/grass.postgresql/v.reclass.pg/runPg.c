#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <libpq-fe.h>
#include "gis.h"
#include "glocale.h"
#include "dbvect.h"

int runPg(SQL_stmt, input, output, vtype, disolve)
     char *SQL_stmt, *input, *output, *vtype;
     int disolve;
{
    FILE *fpout;
    int i, TMP;
    char *tmpfile_rules;

    char sysbuf[QRY_LENGTH];
    char buf1[QRY_LENGTH];
    char buf2[QRY_LENGTH];

    PGconn *pg_conn;
    PGresult *res;
    char *pghost;


    TMP = 0;

    memset(sysbuf, '\0', sizeof(sysbuf));
    memset(buf1, '\0', sizeof(buf1));
    memset(buf2, '\0', sizeof(buf1));


#ifdef VERBOSE
    printf("\n\nExecuting\n%s;\n\n", SQL_stmt);
#endif

    pghost = G__getenv("PG_HOST");

    pg_conn = PQsetdb(pghost, NULL, NULL, NULL, G_getenv("PG_DBASE"));
    if (PQstatus(pg_conn) == CONNECTION_BAD) {
	fprintf(stderr, _("Error: connect Postgres:%s\n"),
		PQerrorMessage(pg_conn));
	PQfinish(pg_conn);
	exit(-1);
    }

    res = PQexec(pg_conn, SQL_stmt);
    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
	fprintf(stderr, _("Error: select Postgres:%s\n"),
		PQerrorMessage(pg_conn));
	PQclear(res);
	PQfinish(pg_conn);
	exit(-1);
    }

    tmpfile_rules = G_tempfile();

    if ((fpout = fopen(tmpfile_rules, "w")) == NULL) {
	fprintf(stderr, _("File write error on temporary file (rules)\n"));
	exit(-1);
    }

    for (i = 0; i < PQntuples(res); i++) {
	strncpy(buf1, PQgetvalue(res, i, 0), 1024);
	strncpy(buf2, PQgetvalue(res, i, 1), 1024);

	fprintf(fpout, "%s=%s\n", buf1, buf2);


    }
    fprintf(fpout, "end");
    fclose(fpout);
    if (!output) {
	output = "tmp.recl";
	TMP = TRUE;
    }

    if (disolve) {
	snprintf(sysbuf, 1024,
		 "v.reclass input=%s output=%s type=%s -d < %s\n", input,
		 output, vtype, tmpfile_rules);
    }
    else {
	snprintf(sysbuf, 1024, "v.reclass input=%s output=%s type=%s < %s\n",
		 input, output, vtype, tmpfile_rules);
    }
    system(sysbuf);

    if (TMP == TRUE) {
	snprintf(sysbuf, 1024, "d.vect %s\n", output);
	system(sysbuf);
    }

    unlink(tmpfile_rules);

    PQclear(res);
    /* explicitly close select result to avoid memory leaks  */

    PQfinish(pg_conn);
    /* close connection to database */



    if (TMP == TRUE) {
	G_remove("dig", "tmp.recl");
	G_remove("dig_cats", "tmp.recl");
	G_remove("dig_misc", "tmp.recl");
	G_remove("hist", "tmp.recl");
	G_remove("dig_plus", "tmp.recl");
	G_remove("dig_att", "tmp.recl");
    }
    return (0);
}
