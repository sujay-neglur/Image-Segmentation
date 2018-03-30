#include <opencv2/opencv.hpp>
#include <iostream>
#include <limits>
#include <algorithm>
#include <string>
#include <queue>

using namespace cv;
using namespace std;

int N, M;


void formAdjacency(int **adjacency, int i, int j, int **classify);

int bfs(int **adjacency, int s, int t, int parent[],int rows,int cols)
{
    bool visited[rows];
    memset(visited, 0, sizeof(visited));
    int count=0;
    // Create a queue, enqueue source vertex and mark source vertex
    // as visited
    queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    // Standard BFS Loop
    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        for (int v=0; v<rows; v++)
        {
            if (visited[v]==false && adjacency[u][v] > 0)
            {
                cout<<count++<<endl;
                q.push(v);
                parent[v] = u;
                visited[v] = 1;
            }
        }
    }
    cout<<"queue empty"<<endl;
    // If we reached sink in BFS starting from source, then return
    // true, else false
    return (visited[t] == 1);
}

// A DFS based function to find all reachable vertices from s.  The function
// marks visited[i] as true if i is reachable from s.  The initial values in
// visited[] must be false. We can also use BFS to find reachable vertices
void dfs(int **adjacency, int s, bool visited[],int rows,int cols)
{
    visited[s] = true;
    for (int i = 0; i < rows; i++)
        if (adjacency[s][i] && !visited[i])
            dfs(adjacency, i, visited,rows,cols);
}

// Prints the minimum s-t cut
void minCut(int **adjacency, int s, int t,int rows,int cols)
{
    int u, v;

    // Create a residual graph and fill the residual graph with
    // given capacities in the original graph as residual capacities
    // in residual graph

    int parent[rows];  // This array is filled by BFS and to store path

    // Augment the flow while tere is path from source to sink
    cout<<bfs(adjacency, s, t, parent,rows,cols)<<endl;
    while (bfs(adjacency, s, t, parent,rows,cols))
    {
        cout<<"in while"<<endl;
        // Find minimum residual capacity of the edhes along the
        // path filled by BFS. Or we can say find the maximum flow
        // through the path found.
        int path_flow = INT_MAX;
        for (v=t; v!=s; v=parent[v])
        {
            u = parent[v];
            path_flow = min(path_flow, adjacency[u][v]);
        }

        // update residual capacities of the edges and reverse edges
        // along the path
        for (v=t; v != s; v=parent[v])
        {
            u = parent[v];
            adjacency[u][v] -= path_flow;
            adjacency[v][u] += path_flow;
        }
    }

    // Flow is maximum now, find vertices reachable from s
    bool visited[rows];
    memset(visited, false, sizeof(visited));
    dfs(adjacency, s, visited,rows,cols);

    // Print all edges that are from a reachable vertex to
    // non-reachable vertex in the original graph
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < rows; j++)
            if (visited[i] && !visited[j] && adjacency[i][j])
                cout << i << " - " << j << endl;

    return;
}

int main() {
    Mat in_image, ii_image, temp_image;
    in_image = imread("simple.png");
    ii_image = in_image.clone();
    cvtColor(ii_image, temp_image, CV_BGR2GRAY);
    vector<int> fgPixels = {230, 180, 380, 180, 230, 280, 380, 280};
    vector<int> bgPixels = {200, 150, 410, 170, 180, 300, 410, 290};
    int fgAvg = 0, bgAvg = 0, sum = 0;
    int i, j;
    for (i = 0; i < fgPixels.size(); i += 2) {
        sum += (int) temp_image.at<uchar>(i, i + 1);
    }
    fgAvg = sum / 4;
    sum = 0;
    for (j = 0; j < bgPixels.size(); j += 2) {
        sum += (int) temp_image.at<uchar>(j, j + 1);
    }
    bgAvg = sum / 4;
    int **classify = new int *[temp_image.rows];
    for (i = 0; i < temp_image.rows; i++)
        classify[i] = new int[temp_image.cols];

    int fgdif, bgdif;
    for (i = 0; i < temp_image.rows; i++) {
        for (j = 0; j < temp_image.cols; j++) {
            fgdif = abs((int) temp_image.at<uchar>(i, j) - fgAvg);
            bgdif = abs((int) temp_image.at<uchar>(i, j) - bgAvg);
            if (fgdif < bgdif) {
                classify[i][j] = 1;
            } else {
                classify[i][j] = 0;
            }
        }
    }
    int rows = temp_image.rows;
    int cols = temp_image.cols;
    M = rows;
    N = cols;
    int **adjacency = new int *[rows * cols];
    for (i = 0; i < rows * cols; i++) {
        adjacency[i] = new int[rows * cols];
    }
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            formAdjacency(adjacency, i, j, classify);
        }
    }
    minCut(adjacency,N*230+180,N*200+150,rows*cols,rows*cols);
    cout << adjacency[255999][255998] << endl;

    return 0;
}

void formAdjacency(int **adjacency, int i, int j, int **classify) {
    int left, right, bottom, up, current;
    if (i == j) {
        adjacency[i][j] = 0;
        return;
    }
    if (i == 0) {
        if (j == N - 1) {
            j = N - 1;
            left = classify[i][j - 1];
            bottom = classify[i + 1][j];
            current = classify[i][j];
            if (current == left) {
                adjacency[N * i + j][N * i + j - 1] = 1000;
                adjacency[N * i + j - 1][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * i + j - 1] = 50;
                adjacency[N * i + j - 1][N * i + j] = 50;
            }
            if (current == bottom) {
                adjacency[N * i + j][N * (i + 1) + j] = 1000;
                adjacency[N * (i + 1) + j][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * (i + 1) + j] = 50;
                adjacency[N * (i + 1) + j][N * i + j] = 50;
            }

        } else {

            left = classify[i][j - 1];
            right = classify[i][j + 1];
            current = classify[i][j];
            bottom = classify[i + 1][j];
            if (left == current) {
                adjacency[N * i + j][N * i + j - 1] = 1000;
                adjacency[N * i + j - 1][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * i + j - 1] = 50;
                adjacency[N * i + j - 1][N * i + j] = 50;
            }
            if (right == current) {
                adjacency[N * i + j][N * i + j + 1] = 1000;
                adjacency[N * i + j + 1][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * i + j + 1] = 50;
                adjacency[N * i + j + 1][N * i + j] = 50;
            }
            if (current == bottom) {
                adjacency[N * i + j][N * (i + 1) + j] = 1000;
                adjacency[N * (i + 1) + j][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * (i + 1) + j] = 50;
                adjacency[N * (i + 1) + j][N * i + j] = 50;
            }

        }
        return;
    }

    if (i == M - 1) {
        if (j == 0) {
            right = classify[i][j + 1];
            up = classify[i - 1][j];
            current = classify[i][j];
            if (right == current) {
                adjacency[N * i + j][N * i + j + 1] = 1000;
                adjacency[N * i + j + 1][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * i + j + 1] = 50;
                adjacency[N * i + j + 1][N * i + j] = 50;
            }
            if (up == current) {
                adjacency[N * i + j][N * (i - 1) + j] = 1000;
                adjacency[N * (i - 1) + j][N * i + j] = 1000;
            } else {
                adjacency[N * i + j][N * (i - 1) + j] = 50;
                adjacency[N * (i - 1) + j][N * i + j] = 50;
            }
        } else {
            if (j == N - 1) {
                left = classify[i][j - 1];
                up = classify[i - 1][j];
                current = classify[i][j];
                if (left == current) {
                    adjacency[N * i + j][N * i + j - 1] = 1000;
                    adjacency[N * i + j - 1][N * i + j] = 1000;
                } else {
                    adjacency[N * i + j][N * i + j - 1] = 50;
                    adjacency[N * i + j - 1][N * i + j] = 50;
                }
                if (current == up) {
                    adjacency[N * i + j][N * (i - 1) + j] = 1000;
                    adjacency[N * (i - 1) + j][N * i + j] = 1000;
                } else {
                    adjacency[N * i + j][N * (i - 1) + j] = 50;
                    adjacency[N * (i - 1) + j][N * i + j] = 50;
                }
            } else {
                left = classify[i][j - 1];
                right = classify[i][j + 1];
                current = classify[i][j];
                up = classify[i - 1][j];
                if (left == current) {
                    adjacency[N * i + j][N * i + j - 1] = 1000;
                    adjacency[N * i + j - 1][N * i + j] = 1000;
                } else {
                    adjacency[N * i + j][N * i + j - 1] = 50;
                    adjacency[N * i + j - 1][N * i + j] = 50;
                }
                if (right == current) {
                    adjacency[N * i + j][N * i + j + 1] = 1000;
                    adjacency[N * i + j + 1][N * i + j] = 1000;
                } else {
                    adjacency[N * i + j][N * i + j + 1] = 50;
                    adjacency[N * i + j + 1][N * i + j] = 50;
                }
                if (current == up) {
                    adjacency[N * i + j][N * (i - 1) + j] = 1000;
                    adjacency[N * (i - 1) + j][N * i + j] = 1000;
                } else {
                    adjacency[N * i + j][N * (i - 1) + j] = 50;
                    adjacency[N * (i - 1) + j][N * i + j] = 50;
                }
            }
        }
        return;
    }
    if (j == 0) {
        right = classify[i][j + 1];
        up = classify[i - 1][j];
        bottom = classify[i + 1][j];
        current = classify[i][j];
        if (right == current) {
            adjacency[N * i + j][N * i + j + 1] = 1000;
            adjacency[N * i + j + 1][N * i + j] = 1000;
        } else {
            adjacency[N * i + j][N * i + j + 1] = 50;
            adjacency[N * i + j + 1][N * i + j] = 50;
        }
        if (current == up) {
            adjacency[N * i + j][N * (i - 1) + j] = 1000;
            adjacency[N * (i - 1) + j][N * i + j] = 1000;
        } else {
            adjacency[N * i + j][N * (i - 1) + j] = 50;
            adjacency[N * (i - 1) + j][N * i + j] = 50;
        }
        if (current == bottom) {
            adjacency[N * i + j][N * (i + 1) + j] = 1000;
            adjacency[N * (i + 1) + j][N * i + j] = 1000;
        } else {
            adjacency[N * i + j][N * (i + 1) + j] = 50;
            adjacency[N * (i + 1) + j][N * i + j] = 50;
        }
        return;
    }
    if (j == N - 1) {
        left = classify[i][j - 1];
        up = classify[i - 1][j];
        bottom = classify[i + 1][j];
        current = classify[i][j];
        if (current == up) {
            adjacency[N * i + j][N * (i - 1) + j] = 1000;
            adjacency[N * (i - 1) + j][N * i + j] = 1000;
        } else {
            adjacency[N * i + j][N * (i - 1) + j] = 50;
            adjacency[N * (i - 1) + j][N * i + j] = 50;
        }
        if (current == bottom) {
            adjacency[N * i + j][N * (i + 1) + j] = 1000;
            adjacency[N * (i + 1) + j][N * i + j] = 1000;
        } else {
            adjacency[N * i + j][N * (i + 1) + j] = 50;
            adjacency[N * (i + 1) + j][N * i + j] = 50;
        }
        if (left == current) {
            adjacency[N * i + j][N * i + j - 1] = 1000;
            adjacency[N * i + j - 1][N * i + j] = 1000;
        } else {
            adjacency[N * i + j][N * i + j - 1] = 50;
            adjacency[N * i + j - 1][N * i + j] = 50;
        }
        return;
    }
    left = classify[i][j - 1];
    up = classify[i - 1][j];
    bottom = classify[i + 1][j];
    current = classify[i][j];
    right = classify[i][j + 1];
    if (current == up) {
        adjacency[N * i + j][N * (i - 1) + j] = 1000;
        adjacency[N * (i - 1) + j][N * i + j] = 1000;
    } else {
        adjacency[N * i + j][N * (i - 1) + j] = 50;
        adjacency[N * (i - 1) + j][N * i + j] = 50;
    }
    if (current == bottom) {
        adjacency[N * i + j][N * (i + 1) + j] = 1000;
        adjacency[N * (i + 1) + j][N * i + j] = 1000;
    } else {
        adjacency[N * i + j][N * (i + 1) + j] = 50;
        adjacency[N * (i + 1) + j][N * i + j] = 50;
    }
    if (left == current) {
        adjacency[N * i + j][N * i + j - 1] = 1000;
        adjacency[N * i + j - 1][N * i + j] = 1000;
    } else {
        adjacency[N * i + j][N * i + j - 1] = 50;
        adjacency[N * i + j - 1][N * i + j] = 50;
    }
    if (right == current) {
        adjacency[N * i + j][N * i + j + 1] = 1000;
        adjacency[N * i + j + 1][N * i + j] = 1000;
    } else {
        adjacency[N * i + j][N * i + j + 1] = 50;
        adjacency[N * i + j + 1][N * i + j] = 50;
    }
    return;
}