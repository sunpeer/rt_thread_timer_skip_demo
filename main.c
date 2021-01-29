#include "stdio.h"
#include "malloc.h"

#define RT_TIMER_SKIP_LIST_LEVEL 10
#define RT_TIMER_SKIP_LIST_MASK 3

#define TIMER_NUM 48


//定义一个rt_list_node结构体
struct rt_list_node
{
    struct rt_list_node *next;                          /**< point to next node. */
    struct rt_list_node *prev;                          /**< point to prev node. */
    struct timer_t *timer;
};
typedef struct rt_list_node rt_list_t;                  /**< Type for lists. */

struct timer_t
{   
    rt_list_t timer_list_node[RT_TIMER_SKIP_LIST_LEVEL];
    unsigned int value;
};


void rt_list_init(rt_list_t *l)
{
    l->next = l->prev = l;
    l->timer=NULL;
}

void rt_list_insert_after(rt_list_t *l, rt_list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

//用来存储list_node的数组
rt_list_t timer_list[RT_TIMER_SKIP_LIST_LEVEL];

void timer_list_init(void)
{
    short i;
    for(i=0;i<RT_TIMER_SKIP_LIST_LEVEL;i++)
    {
        rt_list_init(&timer_list[i]);
    }
}

void add_timer(struct timer_t *timer)
{
    static unsigned int timer_n=0;
    unsigned int tst_nr;
    int row_lvl;
    rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];
    row_head[0]  = &timer_list[0];
    for (row_lvl = 0; row_lvl < RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        for (; row_head[row_lvl] != timer_list[row_lvl].prev;
             row_head[row_lvl]  = row_head[row_lvl]->next)
        {
            struct timer_t *t;
            rt_list_t *p = row_head[row_lvl]->next;

            /* fix up the entry pointer */
            t = p->timer;

            /* If we have two timers that timeout at the same time, it's
             * preferred that the timer inserted early get called early.
             * So insert the new timer to the end the the some-timeout timer
             * list.
             */
            if ((t->value - timer->value) == 0)
            {
                continue;
            }
            else if (t->value>timer->value)
            {
                break;
            }
        }
        if (row_lvl != RT_TIMER_SKIP_LIST_LEVEL - 1)
            row_head[row_lvl + 1] = row_head[row_lvl] + 1;
    }

    timer_n++;
    tst_nr=timer_n;
    rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL-1],&(timer->timer_list_node[RT_TIMER_SKIP_LIST_LEVEL-1]));
    for (row_lvl = 2; row_lvl <= RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        if (!(tst_nr & RT_TIMER_SKIP_LIST_MASK))
            rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL - row_lvl],
                                 &(timer->timer_list_node[RT_TIMER_SKIP_LIST_LEVEL - row_lvl]));
        else
            break;
        /* Shift over the bits we have tested. Works well with 1 bit and 2
         * bits. */
        tst_nr >>= (RT_TIMER_SKIP_LIST_MASK + 1) >> 1;
    }
}

struct timer_t* create_timer(unsigned int value)
{
    //申请内存
    struct timer_t* p=malloc(sizeof(struct timer_t));
    //初始化
    p->value=value;
    short i;
    for(i=0;i<RT_TIMER_SKIP_LIST_LEVEL;i++)
    {
        rt_list_init(&p->timer_list_node[i]);
        rt_list_t *node_point=&p->timer_list_node[i];
        node_point->timer=p;
    }
    return p;
}

void printf_timer_list(void)
{
    int i;
    rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];

    //打印timer_list数组的情况
    for (i = 0; i < RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        row_head[i]=timer_list[i].next;
        for(;row_head[i]!=&timer_list[i];row_head[i]=row_head[i]->next)
        {
            printf("%d ",row_head[i]->timer->value);
        }
        printf("\n");
    }
}

int main(void)
{
    /* test memory address length */
    // printf("sizeof((void*)p)=%d",sizeof((void*)0));
    
    timer_list_init();
    
    /* test part 1 */
    /* add timer in incresing order */
    struct timer_t* timer_array_8[TIMER_NUM];
    int i;
    for(i=0;i<TIMER_NUM;i++)
    {
        timer_array_8[i] = create_timer(i * 8);
        add_timer(timer_array_8[i]);
    }

    /* test part 2 */
    /* add some random value timer */
    struct timer_t* timer_array_7[TIMER_NUM];
    for(i=0;i<TIMER_NUM;i++)
    {
        timer_array_7[i]=create_timer(i*7);
        add_timer(timer_array_7[i]);
    }

    printf_timer_list();
}