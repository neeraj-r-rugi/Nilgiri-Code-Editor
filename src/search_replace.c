#include "search_replace.h"


static void
free_search_run(SearchRun *run)
{
    if (!run) return;
    g_free(run->search_text);
    g_free(run);
}

/* Cancel and detach any running incremental search attached to the widget (search_entry).
 * We immediately clear the pointer stored on the entry so subsequent callers don't act on it.
 */
static void
cancel_search_run(GtkWidget *entry_widget)
{
    gpointer p = g_object_get_data(G_OBJECT(entry_widget), SEARCH_RUN_KEY);
    if (!p) return;
    SearchRun *run = (SearchRun *)p;

    g_object_set_data(G_OBJECT(entry_widget), SEARCH_RUN_KEY, NULL);

    run->cancelled = TRUE;
    if (run->idle_id != 0) {
        gboolean removed = g_source_remove(run->idle_id);
        run->idle_id = 0;
        (void) removed; /* if false, idle had already run — but we still safe-check below */
        free_search_run(run);
    } else {
        /* If idle_id == 0, that means the idle callback is responsible for freeing the run.
           We already set cancelled=TRUE and cleared the entry pointer; idle callback will see
           cancelled and free run. Nothing more to do here. */
    }
}

/* Idle callback: apply up to BATCH_SIZE matches. It owns the run and must free it when done.
 * It clears the entry's stored run pointer before freeing, to avoid races with cancel_search_run().
 */
static gboolean
search_idle_cb(gpointer user_data)
{
    SearchRun *run = (SearchRun *)user_data;
    if (!run) return G_SOURCE_REMOVE;

    /* If cancelled, cleanup and exit */
    if (run->cancelled) {
        if (run->entry)
            g_object_set_data(G_OBJECT(run->entry), SEARCH_RUN_KEY, NULL);

        run->idle_id = 0;
        free_search_run(run);
        return G_SOURCE_REMOVE;
    }

    GtkTextIter match_start, match_end;
    int found_count = 0;
    GtkTextBuffer *textbuf = GTK_TEXT_BUFFER(run->buffer);

    while (found_count < BATCH_SIZE) {
        gboolean ok = gtk_text_iter_forward_search(&run->iter,
                                                   run->search_text,
                                                   GTK_TEXT_SEARCH_CASE_INSENSITIVE,
                                                   &match_start,
                                                   &match_end,
                                                   NULL);
        if (!ok)
            break; 

        gtk_text_buffer_apply_tag(textbuf, run->tag, &match_start, &match_end);
        run->iter = match_end;

        /* Protect against zero-length matches (avoid infinite loop) */
        if (gtk_text_iter_equal(&run->iter, &match_start)) {
            if (!gtk_text_iter_forward_char(&run->iter))
                break;
        }

        found_count++;
    }

    /* Check if more matches exist */
    GtkTextIter tmp_start, tmp_end;
    gboolean more = gtk_text_iter_forward_search(&run->iter,
                                                 run->search_text,
                                                 GTK_TEXT_SEARCH_CASE_INSENSITIVE,
                                                 &tmp_start,
                                                 &tmp_end,
                                                 NULL);

    if (!more || run->cancelled) {
        /* Finished (or canceled). Clear the pointer on the entry BEFORE freeing run
           so cancel_search_run() will not try to operate on a freed run. */
        if (run->entry)
            g_object_set_data(G_OBJECT(run->entry), SEARCH_RUN_KEY, NULL);

        run->idle_id = 0;
        free_search_run(run);
        return G_SOURCE_REMOVE;
    }

    /* Still more — continue calling this idle callback */
    return G_SOURCE_CONTINUE;
}

/* Start a new incremental search run. Attaches the run pointer to search_entry (so it can be canceled later). */
 void start_incremental_search(SearchData *data)
{
    const gchar *raw_search = gtk_entry_get_text(data->search_entry);

    clear_highlights(data->buffer);

    if (!raw_search || raw_search[0] == '\0') {
        return;
    }

    /* Cancel any previous run attached to this entry (this will also free it if it removed the idle) */
    cancel_search_run(GTK_WIDGET(data->search_entry));

    /* Create new run */
    SearchRun *run = g_new0(SearchRun, 1);
    run->buffer = data->buffer;
    run->entry = GTK_ENTRY(data->search_entry);
    run->search_text = g_strdup(raw_search);
    run->tag = ensure_search_tag(GTK_TEXT_BUFFER(data->buffer));
    run->cancelled = FALSE;
    run->idle_id = 0;

    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(run->buffer), &run->iter);

    g_object_set_data(G_OBJECT(data->search_entry), SEARCH_RUN_KEY, run);

    run->idle_id = g_idle_add(search_idle_cb, run);
}






