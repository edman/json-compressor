
#include "rapidjson/document.h"
#include "../src/cjson.hpp"
#include "../src/bp_traversal.hpp"
#include "../src/df_traversal.hpp"

using namespace std;
using namespace rapidjson;

/******************************************************************************/
/* File handling and logging */
string filename(int, bool input=false);
long filesize(string);
template <class T> void log_cjson_size(Cjson<T>&, long long, string&);

/******************************************************************************/
/* Json parsing */
Document rapid_from_file(string fn);
Document rapid_from_file(int k=1, bool input=false);

/******************************************************************************/
/* Time measurement and evaluation */
long long tick(string msg="time", bool bt=false);
long long average(long long *a, int n);

/******************************************************************************/
/* Document traversal */
void traversalRapid(Value&);
void doubleTraversalCjson(DfTraversal &tdf, BpTraversal &tbp);

/******************************************************************************/
/* Document traversal (header-only definitions) */
template <class T> static void traverserDfs(T t) {
    if (!t.hasChild()) return;
    vector<T> children = t.getChildren();
    for (auto child : children) {
        traverserDfs(child);
    }
}

template <class T> static void traverserBfs(T t) {
    queue<T> q; q.push(t);
    while (!q.empty()) {
        T cur = q.front(); q.pop();
        if (cur.hasChild()) {
            for (auto child : cur.getChildren())
                q.push(child);
        }
    }
}

template <class T> static void traversalDfs(T& t) {
    // cout << "dfs node " << t << " " << t.getCurrentNode() << " ";
    if (t.goToChild()) {
        traversalDfs(t);
        t.goToParent();
    }
    if (t.goToNextSibling()) traversalDfs(t);
}

