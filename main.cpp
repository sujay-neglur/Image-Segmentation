#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int findIntensity(int i, int j) {
    return abs(i + j) / 2;
}

bool checkSimilar(int i, int j) {
//    int greater, smaller;
//    if (i >= j) {
//        greater = i;
//        smaller = j;
//    } else {
//        greater = j;
//        smaller = i;
//    }
//    if (greater * 0.9 < smaller && smaller * 1.1 > greater)
//        return true;
//    else
//        return false;
    return i == j;

}

void colorImage(Mat &image, bool *visited) {
    int i, r, c;
    int row = image.rows;
    int col = image.cols;
    for (i = 0; i < row * col; i++) {
        if (visited[i]) {
            r = i / col;
            c = i % col;
            image.at<Vec3b>(r, c)[0] = 0;
            image.at<Vec3b>(r, c)[1] = 0;
            image.at<Vec3b>(r, c)[2] = 255;
        } else {
            r = i / col;
            c = i % col;
            image.at<Vec3b>(r, c)[0] = 255;
            image.at<Vec3b>(r, c)[1] = 0;
            image.at<Vec3b>(r, c)[2] = 0;
        }
    }
}

bool checkEdgePresent(vector<vector<pair<int, int> > > &adj, int i, int j) {
    bool edgePresent = false;
    vector<pair<int, int> >::iterator it;
    for (it = adj[i].begin(); it != adj[i].end(); it++) {
        if ((*it).first == j && (*it).second != 0)
            edgePresent = true;
    }
    return edgePresent;
}

bool searchPath(vector<vector<pair<int, int> > > &adj, int source, int target, int *parent) {
    if (source == target)
        return true;
    vector<bool> visited(adj.size());
    int i, j;
    for (i = 0; i < visited.size(); i++)
        visited[i] = false;
    visited[source] = true;
    list<int> queue;
    queue.push_back(source);
    vector<pair<int, int> >::iterator it;
    vector<pair<int, int> > temp;
    parent[source] = -1;
    while (!queue.empty()) {
        source = queue.front();
        queue.pop_front();
        for (it = adj[source].begin(); it != adj[source].end(); it++) {
            if (source == target) {
                return true;
            }
            if (!visited[(*it).first] && checkEdgePresent(adj, source, (*it).first)) {
                queue.push_back((*it).first);
                parent[(*it).first] = source;
                visited[(*it).first] = true;
            }

        }
    }
    return false;
}

void checkReachable(vector<vector<pair<int, int> > > &adj, int source, bool *visited) {
    queue<int> q;
    q.push(source);
    visited[source] = true;
    while (!q.empty()) {
        int front = q.front();
        q.pop();
        visited[front] = true;
        vector<pair<int, int> >::iterator it;
        for (it = adj[front].begin(); it != adj[front].end(); it++) {
            if (!visited[(*it).first] && checkEdgePresent(adj, front, (*it).first)) {
                visited[(*it).first] = true;
                q.push((*it).first);
            }
        }
    }
}

bool *graphCut(Mat &in_image, vector<vector<pair<int, int> > > &adj, int source, int target) {
    int i, j;
    vector<vector<pair<int, int> > > residual = adj;
    int *parent = new int[adj.size()];
    while (searchPath(residual, source, target, parent)) {
        int flow = numeric_limits<int>::max();
        for (j = target; j != source; j = parent[j]) {
            int weight = 0;
            i = parent[j];
            vector<pair<int, int> >::iterator it;
            for (it = residual[i].begin(); it != residual[i].end(); it++) {
                if ((*it).first == j)
                    weight = (*it).second;
            }
            flow = min(flow, weight);
        }
        for (j = target; j != source; j = parent[j]) {
            vector<pair<int, int> >::iterator it;
            i = parent[j];
            for (it = residual[i].begin(); it != residual[i].end(); it++) {
                if ((*it).first == j) {
                    (*it).second -= flow;
                }
            }
            for (it = residual[j].begin(); it != residual[j].end(); it++) {
                if ((*it).first == i) {
                    (*it).second += flow;
                }
            }
        }
    }
    vector<pair<int, int> >::iterator it;
    bool *visited = new bool[adj.size()];
    memset(visited, false, adj.size());
    checkReachable(residual, source, visited);
//    for (int i = 0; i < adj.size(); i++) {
//        vector<pair<int, int>>::iterator it;
//        for (it = adj[i].begin(); it != adj[i].end(); it++) {
//            if (visited[i] && !visited[(*it).first] && checkEdgePresent(adj, i, (*it).first)) {
//                cout << i << " - " << (*it).first << endl;
//            }
//        }
//    }
    return visited;

}

void fillAdjacency(Mat image, vector<vector<pair<int, int> > > &adj, int i, int j, int maxIntensity) {
    int N = image.cols;
    pair<int, int> temp;
    int left, right, top, bottom, weight, avgIntensity;
    if (i == 0) {
        if (j == 0) {
            vector<pair<int, int> > vtemp;
            //right
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1))) {
                weight = numeric_limits<int>::max();
            } else
                weight = 50;
            temp = make_pair(N * i + j + 1, weight);
            vtemp.push_back(temp);
            //bottom
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * (i + 1) + j, weight);
            vtemp.push_back(temp);
            adj.push_back(vtemp);
            return;
        } else if (j == image.cols - 1) {
            vector<pair<int, int> > vtemp;
            //left
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j - 1, weight);
            vtemp.push_back(temp);
            //bottom
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * (i + 1) + j, weight);
            vtemp.push_back(temp);
            adj.push_back(vtemp);

        } else {
            vector<pair<int, int> > vtemp;
            //left
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j - 1, weight);
            vtemp.push_back(temp);
            //right
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j + 1, weight);
            vtemp.push_back(temp);
            //bottom
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * (i + 1) + j, weight);
            vtemp.push_back(temp);
            adj.push_back(vtemp);
        }
        return;
    }
    if (i == image.rows - 1) {
        if (j == 0) {
            vector<pair<int, int> > vtemp;
            //right
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j + 1, weight);
            vtemp.push_back(temp);
            //top
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * (i - 1) + j, weight);
            vtemp.push_back(temp);
            adj.push_back(vtemp);
        } else if (j == image.cols - 1) {
            vector<pair<int, int> > vtemp;
            //left
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j - 1, weight);
            vtemp.push_back(temp);
            //top
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * (i - 1) + j, weight);
            vtemp.push_back(temp);
            adj.push_back(vtemp);
        } else {
            vector<pair<int, int> > vtemp;
            //left
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j - 1, weight);
            vtemp.push_back(temp);
            //right
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * i + j + 1, weight);
            vtemp.push_back(temp);
            //top
//            avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j));
//            weight = maxIntensity - avgIntensity;
            if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j)))
                weight = numeric_limits<int>::max();
            else
                weight = 50;
            temp = make_pair(N * (i - 1) + j, weight);
            vtemp.push_back(temp);
            adj.push_back(vtemp);
        }
        return;
    }
    if (j == 0) {
        vector<pair<int, int> > vtemp;
        //right
//        avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1));
//        weight = maxIntensity - avgIntensity;
        if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1)))
            weight = numeric_limits<int>::max();
        else
            weight = 50;
        temp = make_pair(N * i + j + 1, weight);
        vtemp.push_back(temp);
        //top
//        avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j));
//        weight = maxIntensity - avgIntensity;
        if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j)))
            weight = numeric_limits<int>::max();
        else
            weight = 50;
        temp = make_pair(N * (i - 1) + j, weight);
        vtemp.push_back(temp);
        //bottom
//        avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j));
//        weight = maxIntensity - avgIntensity;
        if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j)))
            weight = numeric_limits<int>::max();
        else
            weight = 50;
        temp = make_pair(N * (i + 1) + j, weight);
        vtemp.push_back(temp);
        adj.push_back(vtemp);
        return;
    }
    if (j == image.cols - 1) {
        vector<pair<int, int> > vtemp;
        //left
//        avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1));
//        weight = maxIntensity - avgIntensity;
        if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1)))
            weight = numeric_limits<int>::max();
        else
            weight = 50;
        temp = make_pair(N * i + j - 1, weight);
        vtemp.push_back(temp);
        //top
//        avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j));
//        weight = maxIntensity - avgIntensity;
        if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j)))
            weight = numeric_limits<int>::max();
        else
            weight = 50;
        temp = make_pair(N * (i - 1) + j, weight);
        vtemp.push_back(temp);
        //bottom
//        avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j));
//        weight = maxIntensity - avgIntensity;
        if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j)))
            weight = numeric_limits<int>::max();
        else
            weight = 50;
        temp = make_pair(N * (i + 1) + j, weight);
        vtemp.push_back(temp);
        adj.push_back(vtemp);
        return;
    }
    vector<pair<int, int> > vtemp;
    //left
//    avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1));
//    weight = maxIntensity - avgIntensity;
    if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j - 1)))
        weight = numeric_limits<int>::max();
    else
        weight = 50;
    temp = make_pair(N * i + j - 1, weight);
    vtemp.push_back(temp);
    //right
//    avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1));
//    weight = maxIntensity - avgIntensity;
    if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i, j + 1)))
        weight = numeric_limits<int>::max();
    else
        weight = 50;
    temp = make_pair(N * i + j + 1, weight);
    vtemp.push_back(temp);
    //top
//    avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j));
//    weight = maxIntensity - avgIntensity;
    if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i - 1, j)))
        weight = numeric_limits<int>::max();
    else
        weight = 50;
    temp = make_pair(N * (i - 1) + j, weight);
    vtemp.push_back(temp);
    //bottom
//    avgIntensity = findIntensity((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j));
//    weight = maxIntensity - avgIntensity;
    if (checkSimilar((int) image.at<uchar>(i, j), (int) image.at<uchar>(i + 1, j)))
        weight = numeric_limits<int>::max();
    else
        weight = 50;
    temp = make_pair(N * (i + 1) + j, weight);
    vtemp.push_back(temp);
    adj.push_back(vtemp);
    return;
}

int main(int argc, char *argv[]) {
//    clock_t begin = clock();
    ifstream inFile;
    ofstream outFile;
    int i, j, maxIntensity = 0;
    int row, col, loc;
    Mat image = imread(argv[1]), grayImage;
    GaussianBlur(image, image, Size(3, 3), 0, 0, BORDER_DEFAULT);
    cvtColor(image, grayImage, CV_BGR2GRAY);
//    Mat grdx,grdy;
//    Scharr(grayImage,grdx,CV_16S,1,0);
//    convertScaleAbs(grdx,grdx);
//    Scharr(grayImage,grdy,CV_16S,0,1);
//    convertScaleAbs(grdy,grdy);
//    addWeighted(grdx,0.5,grdy,0.5,0,grayImage);
    for (i = 0; i < image.rows; i++) {
        for (j = 0; j < image.cols; j++) {
            if ((int) image.at<uchar>(i, j) > maxIntensity)
                maxIntensity = (int) image.at<uchar>(i, j);
        }
    }
    vector<vector<pair<int, int> > > adj;
    for (i = 0; i < image.rows; i++) {
        for (j = 0; j < image.cols; j++) {
            fillAdjacency(grayImage, adj, i, j, maxIntensity);
        }
    }

    adj.resize(adj.size() + 2);
    int pixels;
    inFile.open(argv[2]);
    inFile >> pixels;
    i = 0;
    vector<int> fg, bg;
    while (i != pixels) {
        inFile >> col;
        inFile >> row;
        inFile >> loc;
        if (loc == 1)
            fg.push_back(image.cols * row + col);
        else
            bg.push_back(image.cols * row + col);

        i++;
    }
    vector<pair<int, int> > temp1;
    //connect virtual source to fg pixels
    for (i = 0; i < fg.size(); i++) {
        vector<pair<int, int> >::iterator it;
        pair<int, int> temp = make_pair(fg[i], numeric_limits<int>::max());
        temp1.push_back(temp);
    }

    adj[adj.size() - 2] = temp1;
    temp1.clear();
    //connect virtual target to bg pixels
    for (i = 0; i < bg.size(); i++) {
        vector<pair<int, int> >::iterator it;
        pair<int, int> temp = make_pair(bg[i], numeric_limits<int>::max());
        temp1.push_back(temp);
    }

    adj[adj.size() - 1] = temp1;
    temp1.clear();
    //connect fg and bg pixels to virtual source and target
    for (i = 0; i < fg.size(); i++) {
        pair<int, int> temp = make_pair(adj.size() - 2, numeric_limits<int>::max());
        adj[fg[i]].push_back(temp);
        temp = make_pair(adj.size() - 1, numeric_limits<int>::max());
        adj[bg[i]].push_back(temp);
    }

    bool *visited = graphCut(image, adj, image.rows * image.cols, image.rows * image.cols + 1);
    colorImage(image, visited);
    imwrite(argv[3], image);
//     clock_t end = clock();
//    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
//    cout<<"Time "<<time_spent<<endl;
}