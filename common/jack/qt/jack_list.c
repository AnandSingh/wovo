/* -------------------------------------------- Header File Inclusion */
#include <time.h>       /* for time() for random seed */
#include <stdlib.h>
#include "osal_list.h"
#include "osal_memory.h"


#ifndef SIMCLIST_MAX_SPARE_ELEMS
#define SIMCLIST_MAX_SPARE_ELEMS        5
#endif

//#define srandom srand
//#define random rand

/* ----------------------------------------- External Global Variables */


/* ----------------------------------------- Exported Global Variables */

/* ----------------------------------------- Static Global Variables */


/* ----------------------------------------- Functions */
/* list initialization */
int ScmListInit(SCM_LIST_T * l)
{
    if (l == NULL) return -1;

    //random((unsigned long)time(NULL));
	//rand((unsigned long)time(NULL));

    l->numels = 0;

    /* head/tail sentinels and mid pointer */
    l->head_sentinel = (struct SCM_LIST_ENTRY_T_TAG *)Evmalloc(sizeof(struct SCM_LIST_ENTRY_T_TAG));
    l->tail_sentinel = (struct SCM_LIST_ENTRY_T_TAG *)Evmalloc(sizeof(struct SCM_LIST_ENTRY_T_TAG));
    l->head_sentinel->next = l->tail_sentinel;
    l->tail_sentinel->prev = l->head_sentinel;
    l->head_sentinel->prev = l->tail_sentinel->next = l->mid = NULL;
    l->head_sentinel->data = l->tail_sentinel->data = NULL;

    /* iteration attributes */
    l->iter_active = 0;
    l->iter_pos = 0;
    l->iter_curentry = NULL;

    /* free-list attributes */
    l->spareels = (SCM_LIST_ENTRY_T **)Evmalloc(SIMCLIST_MAX_SPARE_ELEMS * sizeof(SCM_LIST_ENTRY_T *));
    l->spareelsnum = 0;

#ifdef SIMCLIST_WITH_THREADS
    l->threadcount = 0;
#endif

	 l->attrs.comparator = NULL;
    /* also free() element data when removing and element from the list */
    l->attrs.meter = NULL;
    l->attrs.copy_data = 0;

  //assert(ScmListAttrOk(l));


    return 0;
}

int ScmListAppend(SCM_LIST_T * l, const void *data)
{
    return ScmListInsertAt(l, data, l->numels);
}

int ScmListInsertAt(SCM_LIST_T * l, const void *data, unsigned int pos)
{
    SCM_LIST_ENTRY_T *lent, *succ, *prec;
    
    if (l->iter_active || pos > l->numels) return -1;
    
    /* this code optimizes malloc() with a free-list */
    if (l->spareelsnum > 0) {
        lent = l->spareels[l->spareelsnum-1];
        l->spareelsnum--;
    } else {
        lent = (SCM_LIST_ENTRY_T *)Evmalloc(sizeof(SCM_LIST_ENTRY_T));
        if (lent == NULL)
            return -1;
    }

    if (l->attrs.copy_data) {
        /* make room for user' data (has to be copied) */
        size_t datalen = l->attrs.meter(data);
        lent->data = (SCM_LIST_ENTRY_T *)Evmalloc(datalen);
        memcpy(lent->data, data, datalen);
    } else {
        lent->data = (void*)data;
    }

    /* actually append element */
    prec = ScmListFindPos(l, pos-1);
    succ = prec->next;
    
    prec->next = lent;
    lent->prev = prec;
    lent->next = succ;
    succ->prev = lent;

    l->numels++;

    /* fix mid pointer */
    if (l->numels == 1) { /* first element, set pointer */
        l->mid = lent;
    } else if (l->numels % 2) {    /* now odd */
        if (pos >= (l->numels-1)/2) l->mid = l->mid->next;
    } else {                /* now even */
        if (pos <= (l->numels-1)/2) l->mid = l->mid->prev;
    }

//    assert(list_repOk(l));

    return 1;
}

/* set tmp to point to element at index posstart in l */
static SCM_LIST_ENTRY_T *ScmListFindPos(const SCM_LIST_T * l, int posstart) 
{
    SCM_LIST_ENTRY_T *ptr;
    float x;
    int i;

    /* accept 1 slot overflow for fetching head and tail sentinels */
    if (posstart < -1 || posstart > (int)l->numels) return NULL;

    x = (float)(posstart+1) / l->numels;
    if (x <= 0.25) {
        /* first quarter: get to posstart from head */
        for (i = -1, ptr = l->head_sentinel; i < posstart; ptr = ptr->next, i++);
    } else if (x < 0.5) {
        /* second quarter: get to posstart from mid */
        for (i = (l->numels-1)/2, ptr = l->mid; i > posstart; ptr = ptr->prev, i--);
    } else if (x <= 0.75) {
        /* third quarter: get to posstart from mid */
        for (i = (l->numels-1)/2, ptr = l->mid; i < posstart; ptr = ptr->next, i++);
    } else {
        /* fourth quarter: get to posstart from tail */
        for (i = l->numels, ptr = l->tail_sentinel; i > posstart; ptr = ptr->prev, i--);
    }
    return ptr;
}


int ScmListStart(SCM_LIST_T * l)
{
    if (l->iter_active) return 0;
    l->iter_pos = 0;
    l->iter_active = 1;
    l->iter_curentry = l->head_sentinel->next;
    return 1;
}


int ScmListCheckNext(const SCM_LIST_T * l)
{
    if (! l->iter_active) return 0;
    return (l->iter_pos < l->numels);
}

void *ScmListNext(SCM_LIST_T * l) 
{
    void *toret;

    if (! l->iter_active) return NULL;

    toret = l->iter_curentry->data;
    l->iter_curentry = l->iter_curentry->next;
    l->iter_pos++;

    return toret;
}

int ScmListEnd(SCM_LIST_T * l) {
    if (! l->iter_active) return 0;
    l->iter_pos = 0;
    l->iter_active = 0;
    return 1;
}


int ScmListAttrCopy(SCM_LIST_T *l, SCM_LIST_ELEMENT_SIZE_T element_size_fun, int copy_data)
{
    if (l == NULL || (element_size_fun == NULL && copy_data != 0)) return -1;

    l->attrs.meter = element_size_fun;
    l->attrs.copy_data = copy_data;

	//assert(ScmListAttrOk(l));

    return 0;
}


/* setting list properties */
int ScmListAttrCompare(SCM_LIST_T *l, SCM_LIST_ELEMENT_COMPARE_T comparator_fun)
{
    if (l == NULL) return -1;

    l->attrs.comparator = comparator_fun;

//    assert(ScmListAttrOk(l));
    
    return 0;
}



int ScmListSort(SCM_LIST_T *l, int versus) 
{
    if (l->iter_active || l->attrs.comparator == NULL) /* cannot modify list in the middle of an iteration */
        return -1;

    if (l->numels <= 1)
        return 0;
    ScmListQuickSort(l, versus, 0, l->head_sentinel->next, l->numels-1, l->tail_sentinel->prev);
//    assert(ScmListAttrOk(l));
    return 0;
}

#ifdef SIMCLIST_WITH_THREADS
struct list_sort_wrappedparams {
    SCM_LIST_T *restrict l;
    int versus;
    unsigned int first, last;
    struct SCM_LIST_ENTRY_T *fel, *lel;
};

static void *list_sort_quicksort_threadwrapper(void *wrapped_params) {
    struct list_sort_wrappedparams *wp = (struct list_sort_wrappedparams *)wrapped_params;
    list_sort_quicksort(wp->l, wp->versus, wp->first, wp->fel, wp->last, wp->lel);
    free(wp);
    pthread_exit(NULL);
    return NULL;
}
#endif


static void ScmListSelectionSort(SCM_LIST_T * l, int versus, 
        unsigned int first, SCM_LIST_ENTRY_T *fel,
        unsigned int last, SCM_LIST_ENTRY_T *lel) 
{
    SCM_LIST_ENTRY_T *cursor, *toswap, *firstunsorted;
    void *tmpdata;

    if (last <= first) /* <= 1-element lists are always sorted */
        return;
    
    for (firstunsorted = fel; firstunsorted != lel; firstunsorted = firstunsorted->next) {
        /* find min or max in the remainder of the list */
        for (toswap = firstunsorted, cursor = firstunsorted->next; cursor != lel->next; cursor = cursor->next)
            if (l->attrs.comparator(toswap->data, cursor->data) * -versus > 0) toswap = cursor;
        if (toswap != firstunsorted) { /* swap firstunsorted with toswap */
            tmpdata = firstunsorted->data;
            firstunsorted->data = toswap->data;
            toswap->data = tmpdata;
        }
    }
}




static void ScmListQuickSort(SCM_LIST_T * l, int versus, 
        unsigned int first,SCM_LIST_ENTRY_T *fel,
        unsigned int last,SCM_LIST_ENTRY_T *lel)
{
    unsigned int pivotid;
    unsigned int i;
    register SCM_LIST_ENTRY_T *pivot;
    SCM_LIST_ENTRY_T *left, *right;
    void *tmpdata;
#ifdef SIMCLIST_WITH_THREADS
    pthread_t tid;
    int traised;
#endif


    if (last <= first)      /* <= 1-element lists are always sorted */
        return;
    
    if (last - first+1 <= SIMCLIST_MINQUICKSORTELS) {
        ScmListSelectionSort(l, versus, first, fel, last, lel);
        return;
    }
    
    /* base of iteration: one element list */
    if (! (last > first)) return;

    //pivotid = (rand() % (last - first + 1));
     pivotid = (last - first + 1) / 2; 

    /* find pivot */
    if (pivotid < (last - first + 1)/2) {
        for (i = 0, pivot = fel; i < pivotid; pivot = pivot->next, i++);
    } else {
        for (i = last - first, pivot = lel; i > pivotid; pivot = pivot->prev, i--);
    }

    /* smaller PIVOT bigger */
    left = fel;
    right = lel;
    /* iterate     --- left ---> PIV <--- right --- */
    while (left != pivot && right != pivot) {
        for (; left != pivot && (l->attrs.comparator(left->data, pivot->data) * -versus <= 0); left = left->next);
        /* left points to a smaller element, or to pivot */
        for (; right != pivot && (l->attrs.comparator(right->data, pivot->data) * -versus >= 0); right = right->prev);
        /* right points to a bigger element, or to pivot */
        if (left != pivot && right != pivot) {
            /* swap, then move iterators */
            tmpdata = left->data;
            left->data = right->data;
            right->data = tmpdata;

            left = left->next;
            right = right->prev;
        }
    }

    /* now either left points to pivot (end run), or right */
    if (right == pivot) {    /* left part longer */
        while (left != pivot) {
            if (l->attrs.comparator(left->data, pivot->data) * -versus > 0) {
                tmpdata = left->data;
                left->data = pivot->prev->data;
                pivot->prev->data = pivot->data;
                pivot->data = tmpdata;
                pivot = pivot->prev;
                pivotid--;
                if (pivot == left) break;
            } else {
                left = left->next;
            }
        }
    } else {                /* right part longer */
        while (right != pivot) {
            if (l->attrs.comparator(right->data, pivot->data) * -versus < 0) {
                /* move current right before pivot */
                tmpdata = right->data;
                right->data = pivot->next->data;
                pivot->next->data = pivot->data;
                pivot->data = tmpdata;
                pivot = pivot->next;
                pivotid++;
                if (pivot == right) break;
            } else {
                right = right->prev;
            }
        }
    }

    /* sort sublists A and B :       |---A---| pivot |---B---| */

#ifdef SIMCLIST_WITH_THREADS
    traised = 0;
    if (pivotid > 0) {
        /* prepare wrapped args, then start thread */
        if (l->threadcount < SIMCLIST_MAXTHREADS-1) {
            struct list_sort_wrappedparams *wp = (struct list_sort_wrappedparams *)malloc(sizeof(struct list_sort_wrappedparams));
            l->threadcount++;
            traised = 1;
            wp->l = l;
            wp->versus = versus;
            wp->first = first;
            wp->fel = fel;
            wp->last = first+pivotid-1;
            wp->lel = pivot->prev;
            if (pthread_create(&tid, NULL, list_sort_quicksort_threadwrapper, wp) != 0) {
                free(wp);
                traised = 0;
                list_sort_quicksort(l, versus, first, fel, first+pivotid-1, pivot->prev);
            }
        } else {
            list_sort_quicksort(l, versus, first, fel, first+pivotid-1, pivot->prev);
        }
    }
    if (first + pivotid < last) list_sort_quicksort(l, versus, first+pivotid+1, pivot->next, last, lel);
    if (traised) {
        pthread_join(tid, (void **)NULL);
        l->threadcount--;
    }
#else
    if (pivotid > 0) ScmListQuickSort(l, versus, first, fel, first+pivotid-1, pivot->prev);
    if (first + pivotid < last)ScmListQuickSort(l, versus, first+pivotid+1, pivot->next, last, lel);
#endif
}

int ScmListDeleteAt(SCM_LIST_T * l, unsigned int pos)
{
    SCM_LIST_ENTRY_T *delendo;


    if (/*l->iter_active ||*/ pos >= l->numels)
	{
		return -1;
	}

    delendo = ScmListFindPos(l, pos);

    ScmListDropElement(l, delendo, pos);

    l->numels--;
    //assert(ScmListAttrOk(l));

    return  0;
}


static int ScmListDropElement(SCM_LIST_T * l,SCM_LIST_ENTRY_T *tmp, unsigned int pos)
{
    if (tmp == NULL) return -1;

    /* fix mid pointer */
    if (l->numels % 2) {    /* now odd */
        if (pos >= l->numels/2) l->mid = l->mid->prev;
    } else {                /* now even */
        if (pos < l->numels/2) l->mid = l->mid->next;
    }
    
    tmp->prev->next = tmp->next;
    tmp->next->prev = tmp->prev;

    /* free what's to be freed */
    if (l->attrs.copy_data && tmp->data != NULL)
        Evfree(tmp->data);

    if (l->spareelsnum < SIMCLIST_MAX_SPARE_ELEMS) {
        l->spareels[l->spareelsnum++] = tmp;
    } else {
        Evfree(tmp);
    }

    return 0;
}

void ScmListDestroy(SCM_LIST_T * l)
{
    unsigned int i;

    ScmListClear(l);
    for (i = 0; i < l->spareelsnum; i++) {
        Evfree(l->spareels[i]);
    }
    Evfree(l->spareels);
    Evfree(l->head_sentinel);
    Evfree(l->tail_sentinel);
}




int ScmListClear(SCM_LIST_T * l)
{
    //struct list_entry_s *s;
	SCM_LIST_ENTRY_T *s;

    if (l->iter_active) return -1;
    
    if (l->attrs.copy_data) {        /* also free user data */
        /* spare a loop conditional with two loops: spareing elems and freeing elems */
        for (s = l->head_sentinel->next; l->spareelsnum < SIMCLIST_MAX_SPARE_ELEMS && s != l->tail_sentinel; s = s->next) {
            /* move elements as spares as long as there is room */
            if (s->data != NULL) Evfree(s->data);
            l->spareels[l->spareelsnum++] = s;
        }
        while (s != l->tail_sentinel) {
            /* free the remaining elems */
            if (s->data != NULL) Evfree(s->data);
            s = s->next;
            Evfree(s->prev);
        }
        l->head_sentinel->next = l->tail_sentinel;
        l->tail_sentinel->prev = l->head_sentinel;
    } else { /* only free element containers */
        /* spare a loop conditional with two loops: spareing elems and freeing elems */
        for (s = l->head_sentinel->next; l->spareelsnum < SIMCLIST_MAX_SPARE_ELEMS && s != l->tail_sentinel; s = s->next) {
            /* move elements as spares as long as there is room */
            l->spareels[l->spareelsnum++] = s;
        }
        while (s != l->tail_sentinel) {
            /* free the remaining elems */
            s = s->next;
            Evfree(s->prev);
        }
        l->head_sentinel->next = l->tail_sentinel;
        l->tail_sentinel->prev = l->head_sentinel;
    }
    l->numels = 0;
    l->mid = NULL;

//    assert(list_repOk(l));

    return 0;
}


#if 0
static int ScmListOk(const list_t * restrict l)(const list_t *restrict l) {
    int ok, i;
    struct list_entry_s *s;

    ok = (l != NULL) && (
            /* head/tail checks */
            (l->head_sentinel != NULL && l->tail_sentinel != NULL) &&
                (l->head_sentinel != l->tail_sentinel) && (l->head_sentinel->prev == NULL && l->tail_sentinel->next == NULL) &&
            /* empty list */
            (l->numels > 0 || (l->mid == NULL && l->head_sentinel->next == l->tail_sentinel && l->tail_sentinel->prev == l->head_sentinel)) &&
            /* spare elements checks */
            l->spareelsnum <= SIMCLIST_MAX_SPARE_ELEMS
         );
    
    if (!ok) return 0;

    if (l->numels >= 1) {
        /* correct referencing */
        for (i = -1, s = l->head_sentinel; i < (int)(l->numels-1)/2 && s->next != NULL; i++, s = s->next) {
            if (s->next->prev != s) break;
        }
        ok = (i == (int)(l->numels-1)/2 && l->mid == s);
        if (!ok) return 0;
        for (; s->next != NULL; i++, s = s->next) {
            if (s->next->prev != s) break;
        }
        ok = (i == (int)l->numels && s == l->tail_sentinel);
    }

    return ok;
}
#endif
