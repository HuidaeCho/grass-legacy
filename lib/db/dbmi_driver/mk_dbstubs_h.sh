#!/bin/sh
# generates dbstubs.h

tmp=/tmp/mk_dbstubs_h.tmp.$$
cat <<'EOT'> dbstubs.h
#ifndef DBSTUBS_H
#define	DBSTUBS_H

#ifdef	DB_DRIVER_C
EOT

grep -h '^\( *int *\)\?db__driver' ../stubs/*.c | sed -e 's/^\( *int *\)\?/extern int /' -e 's/ *(.*).*$/();/' > $tmp
cat $tmp >> dbstubs.h
sed 's/^extern int *db__\([a-zA-Z_]\+\).*$/int (*db_\1)() = db__\1;/' $tmp >> dbstubs.h

cat <<'EOT'>> dbstubs.h
#else
EOT
sed 's/^extern int *db__\([a-zA-Z_]\+\).*$/extern int (*db_\1)();/' $tmp >> dbstubs.h
cat <<'EOT'>> dbstubs.h
#endif

#endif
EOT

rm $tmp
