#ifndef SEARCH_REPLACE_H
#define SEARCH_REPLACE_H
#include "defines.h"
#define BATCH_SIZE 200
#define SEARCH_RUN_KEY "nilgiri-search-run"

static void free_search_run(SearchRun * );
static void cancel_search_run(GtkWidget *);
static gboolean search_idle_cb(gpointer user_data);
void start_incremental_search(SearchData *);
#endif