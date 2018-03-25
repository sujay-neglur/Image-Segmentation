#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int N, M;

void formAdjacency(int **adjacency, int i, int j, int **classify);

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
    int **classify=new int *[temp_image.rows];
    for(i=0;i<temp_image.rows;i++)
        classify[i]=new int[temp_image.cols];

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
    M=rows;
    N=cols;
    int **adjacency = new int *[rows * cols];
    for (i = 0; i < rows * cols; i++) {
        adjacency[i] = new int[rows * cols];
    }
    for(i=0;i<rows;i++)
    {
        for(j=0;j<cols;j++)
        {
            formAdjacency(adjacency,i,j,classify);
        }
    }
    cout<<adjacency[255999][255998]<<endl;

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
    if(j==0)
    {
        right=classify[i][j+1];
        up=classify[i-1][j];
        bottom=classify[i+1][j];
        current=classify[i][j];
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
    if(j==N-1)
    {
        left=classify[i][j-1];
        up=classify[i-1][j];
        bottom=classify[i+1][j];
        current=classify[i][j];
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
    left=classify[i][j-1];
    up=classify[i-1][j];
    bottom=classify[i+1][j];
    current=classify[i][j];
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