// 根节点分叉
// 节点满了之后，先分裂再添加

// 每次添加，是添加到叶子节点，叶子节点满了的话，分裂该节点，中间节点归于父节点，父节点满了的话，分裂父节点增加层高

// 删除时需要支持删除状态
//   1. 合并然后删除
//   2. 借位

// 根节点分裂：1->3

#include <stdlib.h>

#define SUB_M 3 // M/2

struct btree_node
{
    int keys[2 * SUB_M - 1];
    struct btree_node *chilrens[2 * SUB_M];

    int num;
    int leaf;
};

struct btree
{
    struct btree_node *root;
};

struct btree_node *btree_create_node(int leaf)
{
    struct btree_node *node = calloc(1, sizeof(struct btree_node));
    if (node == NULL)
        return NULL;
    node->leaf = leaf;
    // 这里也可以是动态分配
    // node->keys = calloc(2 * SUB_M - 1, sizeof(int));
    // node->chilrens = calloc(2 * SUB_M, sizeof(struct btree_node *));
    return node;
}

void btree_destroy_node(struct btree_node *node)
{
    if (node)
        free(node);
}

// 分裂时，中间节点归并到父节点
// 前一半占用之前的节点
// 后一半占用新创建的节点
void btree_split_child(struct btree *T, struct btree_node *x, int idx)
{
    struct btree_node *y = x->chilrens[idx];           // 原有的节点
    struct btree_node *z = btree_create_node(y->leaf); // 分裂时增加的节点

    // 从y中拷贝后面几个key到新的z中
    z->num = SUB_M - 1;
    for (int i = 0; i < SUB_M - 1; ++i)
    {
        z->keys[i] = y->keys[SUB_M + i];
    }

    // 如果不是叶子节点，那就还需要把子节点也拷贝过去（直接都拷贝也可以，反正都是NULL）
    if (y->leaf == 0)
    {
        for (int i = 0; i < SUB_M - 1; ++i)
        {
            z->chilrens[i] = y->chilrens[SUB_M + i];
        }
    }

    y->num = SUB_M;

    // 将idx后面的都往后移，给y腾地方
    for (int i = x->num; i >= idx + 1; --i)
    {
        x->chilrens[i + 1] = x->chilrens[i];
    }
    x->chilrens[idx] = y; // 需要确认

    for (int i = x->num - 1; i >= idx; --i)
    {
        x->keys[i + 1] = x->keys[i];
    }
    x->keys[idx] = y->keys[SUB_M]; // 需要确认

    ++x->num;
}

// 分裂根节点时，首先创建一个新节点指向原来的根节点，然后以新节点作为x执行分裂
void btree_insert(struct btree *T, int key)
{
    struct btree_node *r = T->root;

    if (r->num == 2 * SUB_M - 1)
    {
        struct btree_node *node = btree_create_node(0);
        T->root = node;
        node->chilrens[0] = r;
        btree_split_child(T, node, 0);
    }
}

void btree_merge(struct btree *T, struct btree_node *x, int idx)
{
    struct btree_node *left = x->chilrens[idx];      // 目标节点左子树
    struct btree_node *right = x->chilrens[idx + 1]; // 目标节点右子树

    // 考虑到向左子树合并时不需要移位
    left->keys[left->num] = x->keys[idx]; // 向左子树添加合并中间的节点
    // 向左子树添加右子树的几个key
    for (int i = 0; i < right->num; ++i)
    {
        left->keys[SUB_M + i] = right->keys[i];
    }
    // 向左子树添加右子树的子节点
    if (right->leaf == 0) // 需要确认
    {
        for (int i = 0; i < SUB_M; ++i)
        {
            left->chilrens[SUB_M + i] = right->chilrens[i];
        }
    }
    left->num += SUB_M;
    btree_destroy_node(right);

    for (int i = idx + 1; i < x->num; ++i)
    {
        x->keys[i - 1] = x->keys[i];
        x->chilrens[i] = x->chilrens[i + 1];
    }
}