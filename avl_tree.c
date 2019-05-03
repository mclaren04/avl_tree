#include <stdio.h>
#include <stdlib.h>
#include "avl_tree.h"

static struct node ** find_pivot(struct node **tree_root, struct node *cur)
{
  if (cur->parent)
      if (cur->parent->left == cur)
         return &(cur->parent->left);
      else
         return &(cur->parent->right);
   else
      return tree_root;
}

static struct node * right_rotate(struct node **tree_root, struct node *cur)
{
   int balance_factor = cur->left->h;
   struct node **pivot = find_pivot(tree_root, cur);
   *pivot = cur->left;
   (*pivot)->parent = cur->parent;
   cur->parent = *pivot;
   cur->left = (*pivot)->right;
   if ((*pivot)->right)
      (*pivot)->right->parent = cur;
   (*pivot)->right = cur;
   if (balance_factor == 0) {
      cur->h = 1;
      (*pivot)->h = -1;
   } else {
      cur->h = 0;
      (*pivot)->h = 0;
   }
   return *pivot;
}

static struct node *
right_rotate_double(struct node **tree_root, struct node *cur)
{
   int balance_factor = cur->left->right->h;
   struct node **pivot = find_pivot(tree_root, cur);
   struct node *m = cur->left->right->left;
   struct node *n = cur->left->right->right;
   *pivot = cur->left->right;
   (*pivot)->parent = cur->parent;
   cur->left->parent = cur->parent = *pivot;
   if (m)
      m->parent = cur->left;
   if (n)
      n->parent = cur;
   (*pivot)->left = cur->left;
   (*pivot)->right = cur;
   (*pivot)->left->right = m;
   (*pivot)->right->left = n;
   (*pivot)->h = 0;
   if (balance_factor == 1) {
      (*pivot)->left->h = 0;
      (*pivot)->right->h = -1;
   } else if (balance_factor == 0) {
      (*pivot)->left->h = 0;
      (*pivot)->right->h = 0;
   } else {
      (*pivot)->left->h = 1;
      (*pivot)->right->h = 0;
   }
   return *pivot;
}

static struct node * left_rotate(struct node **tree_root, struct node *cur)
{
   int balance_factor = cur->right->h;
   struct node **pivot = find_pivot(tree_root, cur);
   *pivot = cur->right;
   (*pivot)->parent = cur->parent;
   cur->parent = *pivot;
   if (cur->right->left)
      cur->right->left->parent = cur;
   cur->right = (*pivot)->left;
   (*pivot)->left = cur;
   if (balance_factor == 0) {
      cur->h = -1;
      (*pivot)->h = 1;
   } else {
      cur->h = 0;
      (*pivot)->h = 0;
   }
   return *pivot;
}

static struct node *
left_rotate_double(struct node **tree_root, struct node *cur)
{
   int balance_factor = cur->right->left->h;
   struct node **pivot = find_pivot(tree_root, cur);
   struct node *m = cur->right->left->left;
   struct node *n = cur->right->left->right;
   *pivot = cur->right->left;
   (*pivot)->parent = cur->parent;
   cur->right->parent = cur->parent = *pivot;
   if (m)
      m->parent = cur;
   if (n)
      n->parent = cur->right;
   (*pivot)->left = cur;
   (*pivot)->right = cur->right;
   (*pivot)->left->right = m;
   (*pivot)->right->left = n;
   (*pivot)->h = 0;
   if (balance_factor == 1) {
      (*pivot)->left->h = 0;
      (*pivot)->right->h = -1;
   } else if (balance_factor == 0) {
      (*pivot)->left->h = 0;
      (*pivot)->right->h = 0;
   } else {
      (*pivot)->left->h = 1;
      (*pivot)->right->h = 0;
   }
   return *pivot;
}

static void
balance_add(struct node **tree_root, struct node *cur, struct node *from)
{
   if (cur) {
      cur->h += (cur->left == from) ? 1 : -1;
      if (cur->h == 2) {
         if (cur->left->h >= 0)
            cur = right_rotate(tree_root, cur);
         else
            cur = right_rotate_double(tree_root, cur);
      } else if (cur->h == -2) {
         if (cur->right->h <= 0)
            cur = left_rotate(tree_root, cur);
         else
            cur = left_rotate_double(tree_root, cur);
      }
      if (cur->h) {
         balance_add(tree_root, cur->parent, cur);
      }
   }
}

void add_node(struct node **tree_root, int value)
{
   struct node **cur = tree_root;
   struct node *parent = NULL;
   while (*cur) {
      parent = *cur;
      if ((*cur)->val < value)
         cur = &((*cur)->right);
      else
         cur = &((*cur)->left);
   }
   *cur = malloc(sizeof(struct node));
   (*cur)->val = value;
   (*cur)->left = (*cur)->right = NULL;
   (*cur)->parent = parent;
   (*cur)->h = 0;
   balance_add(tree_root, parent, *cur);
}

static void
balance_del(struct node **tree_root, struct node *cur, enum side sd)
{
   cur->h += sd == left ? -1 : 1;
   if (cur->h == 2) {
      if (cur->left->h >= 0)
         cur = right_rotate(tree_root, cur);
      else
         cur = right_rotate_double(tree_root, cur);
   } else if (cur->h == -2) {
      if (cur->right->h <= 0)
         cur = left_rotate(tree_root, cur);
      else
         cur = left_rotate_double(tree_root, cur);
   }
   if (cur->h == 0 && cur->parent) {
      enum side which_child = left;
      if (cur->parent->right == cur)
         which_child = right;
      balance_del(tree_root, cur->parent, which_child);
   }
}

static void proccess_del_not_list(struct node **root, struct node *closest)
{
   struct node *child_d = NULL;
   struct node **parents_ptr = NULL;
   struct node *parent = closest->parent;
   enum side from_del;
   child_d = closest->left;
   if (closest->right)
      child_d = closest->right;
   if (parent->left == closest) {
      parents_ptr = &(parent->left);
      from_del = left;
   } else {
      parents_ptr = &(parent->right);
      from_del = right;
   }
   if (child_d) {
      child_d->parent = parent;
      *parents_ptr = child_d;
   } else {
      *parents_ptr = NULL;
   }
   free(closest);
   balance_del(root, parent, from_del);
}

static struct node * closest_right(struct node *pivot)
{
   while (pivot->left)
      pivot = pivot->left;
   return pivot;
}

static struct node * closest_left(struct node *pivot)
{
   while (pivot->right)
      pivot = pivot->right;
   return pivot;
}

static void delete_leaf(struct node **root, struct node *del_node)
{
   if (del_node == *root) {
      *root = NULL;
      free(del_node);
   } else {
      enum side which_child;
      struct node *parent = del_node->parent;
      if (parent->left == del_node) {
         parent->left = NULL;
         which_child = left;
      } else {
         parent->right = NULL;
         which_child = right;
      }
      free(del_node);
      balance_del(root, parent, which_child);
   }
}

void delete_node(struct node **tree_root, struct node *del_node)
{
   if (!del_node->left && !del_node->right) {
      delete_leaf(tree_root, del_node);
   } else {
      struct node *closest = NULL, *aux = NULL;
      switch (del_node->h) {
         case -1:
            closest = closest_right(del_node->right);
            break;
         case 1:
            closest = closest_left(del_node->left);
            break;
         case 0:
            aux = closest_left(del_node->left);
            closest = closest_right(del_node->right);
            if (del_node->val - aux->val < closest->val -del_node->val) 
               closest = aux;
      }
      del_node->val = closest->val;
      proccess_del_not_list(tree_root, closest);
   }
}

struct node * search(struct node *tree, int val)
{
   if (tree->val == val || !tree)
      return tree;
   else
      if (val < tree->val)
         return search(tree->left, val);
      else
         return search(tree->right, val);
}

void print_tree(struct node *root)
{
   if (root) {
      print_tree(root->left);
      printf("%d ", root->val);
      print_tree(root->right);
   }
}
