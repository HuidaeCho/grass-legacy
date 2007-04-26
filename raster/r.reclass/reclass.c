#include <stdlib.h>
#include <string.h>
#include <grass/gis.h>
#include <grass/glocale.h>
#include "rule.h"


extern CELL DEFAULT;
extern int default_rule, default_to_itself;
extern char *default_label;


static void compose(struct Reclass *new, const struct Reclass *mid, const struct Reclass *old)
{
    int first;
    CELL i, j, k;
    long num;

    first = 1;

    for (i = old->min; i <= old->max; i++)
    {

	j = old->table[i - old->min];
	if (G_is_c_null_value(&j) || j < mid->min || j > mid->max)
	    continue;

	k = mid->table[j - mid->min];
	if (G_is_c_null_value(&k))
	    continue;

	if (first)
	{
	    new->min = new->max = i;
	    first = 0;
	}
	else
	{
	    if (i < new->min)
		new->min = i;
	    if (i > new->max)
		new->max = i;
	}
    }

    if (first)
	new->min = new->max = 0;

    new->type = RECLASS_TABLE;
    num = new->max - new->min + 1;
    /* make sure don't overflow int */
    if (num != (int) num)
	G_fatal_error (_("Too many categories"));

    new->num = num;
    new->table = (CELL *) G_calloc (new->num, sizeof(CELL));

    for (i = new->min; i <= new->max; i++)
    {
	j = old->table[i - old->min];
	if (G_is_c_null_value(&j) || j < mid->min || j > mid->max)
	{
	    G_set_c_null_value(&new->table[i - new->min], 1);
	    continue;
	}

	k = mid->table[j - mid->min];

	new->table[i - new->min] = k;
    }
}

static void init_reclass(struct Reclass *rec, const RULE *rules)
{
    int first;
    const RULE *r;
    long num;

    first = 1;

    if (default_rule && !G_is_c_null_value(&DEFAULT))
    {
	struct Range range;
	G_read_range(rec->name, rec->mapset, &range);
	G_get_range_min_max(&range, &rec->min, &rec->max);
	if (!G_is_c_null_value(&rec->min) && !G_is_c_null_value(&rec->max))
	    first = 0;
    }

    /* first find the min,max cats */
    for (r = rules; r; r = r->next)
    {
	if (first)
	{
	    rec->min = r->lo;
	    rec->max = r->hi;
	    first = 0;
	}
	else
	{
	    if (r->lo < rec->min)
		rec->min = r->lo;
	    if (r->hi > rec->max)
		rec->max = r->hi;
	}
    }

    /* make sure we have at least one entry */
    if (first)
	rec->min = rec->max = 0;

    /* allocate reclass table */
    rec->type = RECLASS_TABLE;
    num = rec->max - rec->min + 1;

    /* make sure don't overflow int */
    if (num != (int) num)
	G_fatal_error ("Too many categories");

    rec->num = num;
    rec->table = G_calloc(rec->num, sizeof(CELL));
}

static void init_table(struct Reclass *rec, int *is_default)
{
    int i;

    for (i = 0; i < rec->num; i++)
    {
	if (default_rule)
	{
	    if(default_to_itself)
		rec->table[i] = i + rec->min;
	    else
		rec->table[i] = DEFAULT;
	    is_default[i] = 1;
	}
	else
	{
	    G_set_c_null_value(&rec->table[i], 1);
	    is_default[i] = 0;
	}
    }
}

static void fill_table(struct Reclass *rec, int *is_default, const RULE *rules)
{
    const RULE *r;
    int i;

    for (r = rules; r; r = r->next)
	for (i = r->lo; i <= r->hi; i++)
	{
	    rec->table[i - rec->min] = r->new;
	    if (r->new >= rec->min && r->new <= rec->max)
		is_default[r->new - rec->min] = 0;
	}
}

static void set_cats(struct Categories *cats, /* const */ int *is_default, /* const */ struct Reclass *rec)
{
    struct Categories old_cats;
    int cats_read = 0;

    if (default_rule && default_to_itself)
	cats_read = (G_read_cats(rec->name, rec->mapset, &old_cats) >= 0);

    if (cats_read)
    {
	int i;

	for (i = 0; i < rec->num; i++)
	    if (is_default[i])
	    {
		int x = i + rec->min;
		char *label = G_get_cat(x, &old_cats);
		G_set_cat(x, label, cats);
	    }
    }
    else if (default_rule)
        G_set_cat(DEFAULT, default_label, cats);
}

static int _reclass(/* const */ RULE *rules, struct Categories *cats, struct Reclass *new)
{
    int *is_default;

    init_reclass(new, rules);
    is_default = G_calloc (new->num, sizeof(int));
    init_table(new, is_default);
    fill_table(new, is_default, rules);
    set_cats(cats, is_default, new);

    return 0;
}

static int re_reclass(/* const */ RULE *rules, struct Categories *cats,
	/* const */ struct Reclass *old, struct Reclass *new,
	char *input_name, char *input_mapset)
{
    struct Reclass mid;

    mid.name = G_store(input_name);
    mid.mapset = G_store(input_mapset);

    _reclass(rules, cats, &mid);

    compose(new, &mid, old);

    return 0;
}

int reclass (char *old_name, char *old_mapset,
    char *new_name, RULE *rules, struct Categories *cats, char *title)
{
    struct Reclass old, new;
    struct History hist;
    int is_reclass;
    FILE *fd;
    char buf[256];

    is_reclass = G_get_reclass (old_name, old_mapset, &old);
    if (is_reclass < 0)
	G_fatal_error (_("%s in %s - can't read header file"), old_name, old_mapset);

    if (is_reclass)
    {
	new.name = G_store(old.name);
	new.mapset = G_store(old.mapset);
	re_reclass (rules, cats, &old, &new, old_name, old_mapset);
    }
    else
    {
	new.name = G_store(old.name);
	new.mapset = G_store(old.mapset);
	_reclass (rules, cats, &new);
    }

    if (G_put_reclass (new_name, &new) < 0)
	G_fatal_error (_("%s - unable to create reclass file"), new_name);

    if (title == NULL)
	sprintf (title = buf, "Reclass of %s in %s", new.name, new.mapset);

    if ((fd = G_fopen_new ("cell", new_name)) == NULL)
	G_fatal_error (_("%s - unable to create raster map"), new_name);

    fprintf (fd, "Don't remove me\n");
    fclose (fd);

    G_set_cats_title (title, cats);
    if (G_write_cats (new_name, cats) == -1)
	G_fatal_error (_("%s - unable to create category file"), new_name);

    G_free_cats (cats);

    G_short_history (new_name, "reclass", &hist);
    strcpy (hist.datsrc_1, "Reclassified map based on:");
    sprintf (hist.datsrc_2, "  Map [%s] in mapset [%s]",
	new.name, new.mapset);
    G_write_history (new_name, &hist);

    new_range (new_name, &new);

    return 0;
}
