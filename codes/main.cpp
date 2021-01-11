#include <cmath>
#include <iostream>

using namespace std;

double a[3][1010][1010];
int M = 200;
double eps;

int main() {
  freopen("out.txt", "w", stdout);
  // for (double eeps = 3; eeps <= 11; eeps += 0.1) {
  for(int M = 50; M != 201; ++M){
    eps = pow(10, -5);
    int now = 0, pre = 1;
    for (int i = 1; i <= M; i++)
      for (int j = 1; j <= M; j++)
        if (i == 1)
          a[now][i][j] = 1;
        else if (j == 1 || j == M || i == M)
          a[now][i][j] = 0;
        else
          a[now][i][j] = 0.5;
    int I = 0, flag = 0;
    for (; !flag;) {
      flag = 1, now ^= 1, pre ^= 1;
      for (int i = 1; i <= M; i++)
        for (int j = 1; j <= M; j++)
          if (i == 1)
            a[now][i][j] = a[pre][i][j];
          else if (j == 1 || j == M || i == M)
            a[now][i][j] = a[pre][i][j];
          else
            a[now][i][j] = a[pre][i - 1][j] + a[pre][i + 1][j] +
                           a[pre][i][j - 1] + a[pre][i][j + 1],
            a[now][i][j] /= 4;
      for (int i = 1; i <= M; i++)
        for (int j = 1; j <= M; j++)
          if (fabs(a[now][i][j] - a[pre][i][j]) / a[now][i][j] > eps) {
            flag = 0;
            break;
          }
      I++;
    }
    printf("%d %d\n", M, I);
  }
  return 0;
}
