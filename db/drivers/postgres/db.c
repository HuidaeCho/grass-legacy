/*****************************************************************************
*
* MODULE:       PostgreSQL driver forked from DBF driver by Radim Blazek 
*   	    	
* AUTHOR(S):    Alex Shevlakov
*
* PURPOSE:      Simple driver for reading and writing data     
*
* COPYRIGHT:    (C) 2000 by the GRASS Development Team
*
*               This program is free software under the GNU General Public
*   	    	License (>=v2). Read the file COPYING that comes with GRASS
*   	    	for details.
*
*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <gis.h>
#include <dbmi.h>
#include "globals.h"
#include "proto.h"

int db_driver_open_database(handle)
     dbHandle *handle;
{
    char *name, emsg[PG_MSG], buf[500];
    dbConnection connection;

    int i;

    PGresult *res;
    int rec_num;


    db.name[0] = '\0';
    db.tables = NULL;
    db.atables = 0;
    db.ntables = 0;

    db_get_connection(&connection);
    name = db_get_handle_dbname(handle);

    /* if name is empty use connection.databaseName */
    if (strlen(name) == 0) {
	name = connection.databaseName;
    }

    
    /* 'name' may be: 1) database name
    *                 2) connection string in form: host=abc,dbname=db1  */
    if ( strchr(name, '=') == NULL ) { /*db name only */
	sprintf ( buf, "dbname=%s", name );
        strcpy(db.name, buf);
    } else {
        strcpy(db.name, name);
	G_strchg( db.name, ',', ' ');
    }

    G_debug(3, "db_driver_open_database() driver=pg connection='%s'", db.name );

    pg_conn = PQconnectdb( db.name );
    
    if (PQstatus(pg_conn) == CONNECTION_BAD) {
	snprintf(emsg, sizeof(emsg), "Error: connect Postgres: %s\n", PQerrorMessage(pg_conn));
	report_error(emsg);
	PQfinish(pg_conn);
	return DB_FAILED;
    }

    res = PQexec(pg_conn,
		 "select tablename from pg_tables where tablename !~ 'pg_*' order by tablename");

    if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
	snprintf(emsg, sizeof(emsg), "Error: select Postgres: %s\n",
		 PQerrorMessage(pg_conn));
	report_error(emsg);
	PQclear(res);
	PQfinish(pg_conn);
	return DB_FAILED;
    }

    rec_num = PQntuples(res);

    for (i = 0; i < rec_num; i++)
	add_table(PQgetvalue(res, i, 0));

    PQclear(res);

    return DB_OK;
}

int db_driver_close_database()
{
    int i;

    for (i = 0; i < db.ntables; i++) {

	free_table(i);
    }
    free(db.tables);

    PQfinish(pg_conn);

    return DB_OK;
}
