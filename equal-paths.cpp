#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool dfs(Node* root, int depth, int &leafDepth) {//dfs variant
    if (!root) return true; 

    if (!root->left && !root->right) {
        if (leafDepth == -1) { 
            leafDepth = depth;
        } else if (leafDepth != depth) { 
            return false;
        }
        return true;
    }

    bool leftValid = dfs(root->left, depth + 1, leafDepth);
    bool rightValid = dfs(root->right, depth + 1, leafDepth);

    return leftValid && rightValid;
}

bool equalPaths(Node* root) {
    int leafDepth = -1; 
    return dfs(root, 0, leafDepth);
}


