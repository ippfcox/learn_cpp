// 插入新节点时，总是红色的
//   如果它的父节点是黑色，那便结束
//   如果它的父节点也是红色，那么它的祖父节点一定是黑色
//     如果它的叔父节点也是红色，那么修改父节点、叔父节点为黑色，修改祖父节点为红色，并以祖父节点继续进行回溯
//     如果它的叔父节点是黑色。。。这里就整不太明白了

#define RED 0
#define BLACK 1

struct rbtree_node
{
    int key;
    void *value;

    struct rbtree_node *left;
    struct rbtree_node *right;
    struct rbtree_node *parent;

    unsigned char color;
};

struct rbtree
{
    struct rbtree_node *root;
    struct rbtree_node *nil; // 空节点，所有指向空节点的都指向这个
};

//        x->parent                                x->parent
//            |             left rotate                |
//            x             ----------->               y
//           / \                                      / \
//    x->left   y           <-----------             x   y->right
//             / \          right rotate            / \
//      y->left  y->right                    x->left   y->left
void rbtree_left_rotate(struct rbtree *T, struct rbtree_node *x)
{
    struct rbtree_node *y = x->right;

    x->right = y->left;
    if (y->left != T->nil)
        y->left->parent = x;

    y->parent = x->parent;
    if (x->parent == T->nil)
        T->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

void rbtree_right_rotate(struct rbtree *T, struct rbtree_node *y)
{
    struct rbtree_node *x = y->left;

    y->left = x->right;
    if (x->right != T->nil)
        x->right->parent = y;

    x->parent = y->parent;
    if (y->parent == T->nil)
        T->root = x;
    else if (y->parent->right == y)
        y->parent->right = x;
    else
        y->parent->left = x;

    x->right = y;
    y->parent = x;
}

void rbtree_insert_fixup(struct rbtree *T, struct rbtree_node *z)
{
    // z->color == RED
    // only if z->parent->color == RED, disobey rule 4
    //     now: z: RED, z->parent: RED, z->parent-parent: BLACK
    while (z->parent->color == RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            struct rbtree_node *y = z->parent->parent->right; // uncle of z
            // if z's uncle is RED, satisfy rule 5, so just change parent/uncle/parent's parent color
            if (y->color == RED)
            {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;

                z = z->parent->parent;
            }
            else // cannot satisfy rule 5, so we need ratate
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    rbtree_left_rotate(T, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rbtree_right_rotate(T, z->parent->parent);
            }
        }
        else
        {
        }
    }
}

void rbtree_insert(struct rbtree *T, struct rbtree_node *z)
{
    struct rbtree_node *y = T->nil;
    struct rbtree_node *x = T->root;

    // find position
    while (x != T->nil)
    {
        y = x;
        if (z->key < x->key)
            x = x->left;
        else if (z->key > x->key)
            x = x->right;
        else // exist
            return;
    }

    if (y == T->nil)
    {
        T->root = z;
    }
    else
    {
        if (y->key > z->key)
            y->left = z;
        else
            y->right = z;
    }

    z->parent = y;
    z->left = T->nil;
    z->right = T->nil;
    z->color = RED; // new node MUST be RED
}