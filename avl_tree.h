#ifndef AVL_TREE_SENTRY
#define AVL_TREE_SENTRY
enum side { left, right };

struct node {
   struct node *parent, *left, *right;
   int val, h;
};

void add_node(struct node **tree_root, int value);
void delete_node(struct node **tree_root, struct node *del_node);
struct node * search(struct node *tree, int val);
#endif
