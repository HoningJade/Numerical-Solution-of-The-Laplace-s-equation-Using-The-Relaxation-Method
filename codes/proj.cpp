#include <dir.h>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

using namespace std;

#define TEST_TIME
// #define GIF

int const M = 50;
int const INVL = 200;

void initGrid(double grid[M][M], int model) {
  fill(grid[0], grid[0] + M * M, -1);
  switch (model) {
    case 1:
      for (int i = 0; i != M; ++i)
        for (int j = 0; j != M; ++j)
          if (i == 0)
            grid[i][j] = 1;
          else if (j == 0 || i == M - 1 || j == M - 1)
            grid[i][j] = 0;
      break;
    case 2:
      for (int i = 0; i != M; ++i)
        for (int j = 0; j != M; ++j)
          if (i == 0)
            grid[i][j] = 0;
          else if (j == 0 || i == M - 1 || j == M - 1)
            grid[i][j] = 1;
      break;
    case 3:
      for (int i = 0; i != M; ++i)
        for (int j = 0; j != M; ++j)
          if (i == 0)
            grid[i][j] = 1;
          else if (j == 0 || i == M - 1 || j == M - 1)
            grid[i][j] = 0;
      for (int i = 0; i != M / 2; ++i) grid[i][M / 2] = 1;
      for (int i = 0; M % 2 && i != M / 2; ++i) grid[i][M / 2 + 1] = 1;
      break;
    default:
      break;
  }
}

void printGrid(double grid[M][M], string name, ostream &os) {
  os << name << " = {";
  for (int i = M - 1; i != -1; i--) {
    os << "{";
    for (int j = 0; j != M - 1; ++j) os << grid[i][j] << ",";
    os << grid[i][M - 1] << "}";
    if (i != 0) os << ",";
  }
  os << "};" << endl;
}

void printCode(double grid[M][M], int count, string name, ostream &os) {
  printGrid(grid, name, os);
  os << "Export[\"" << name << "_density.png\", ListDensityPlot[" << name
     << ", PlotLegends -> Automatic]];" << endl;
  os << "Export[\"" << name << "_contour.png\", ListContourPlot[" << name
     << ", PlotLegends -> Automatic, ContourLabels -> All]];" << endl;
// gif begin
#ifdef GIF
  // os << "Export[\"" << name << "_density.gif\",{";
  // for (int i = 0; i != count - 1; ++i)
  //   os << "ListDensityPlot[" << name + to_string(i)
  //      << ", PlotLegends -> Automatic],";
  // os << "ListDensityPlot[" << name + to_string(count)
  //    << ", PlotLegends -> Automatic]";
  // os << "}]" << endl;
  os << "Export[\"" << name << "_contour.gif\",{";
  for (int i = 0; i != count + 1; ++i)
    os << "ListContourPlot[" << name + to_string(i)
       << ", PlotLegends -> Automatic, ContourLabels -> All],";
  for (int i = 0; i != 10; ++i)
    os << "ListContourPlot[" << name + to_string(count)
       << ", PlotLegends -> Automatic, ContourLabels -> All],";
  os << "ListContourPlot[" << name + to_string(count)
     << ", PlotLegends -> Automatic, ContourLabels -> All]";
  os << "}]" << endl;
#endif
  // vector plot begin
  os << name << "vector = {";
  for (int i = M - 4; i != 1; --i) {
    os << "{";
    for (int j = M - 4; j != 1; --j) {
      os << "{" << 0.5 * (grid[j][i + 1] - grid[j][i - 1]) << ","
         << 0.5 * (grid[j + 1][i] - grid[j - 1][i]) << "}";
      if (j != 2) os << ",";
    }
    os << "}";
    if (i != 2) os << ",";
  }
  os << "};" << endl;
  os << "Export[\"" << name << "_vector.png\", ListVectorPlot[" << name
     << "vector]];" << endl;
}

int iter(double grid[M][M], double const initGrid[M][M], double eps,
         string name, ostream &os) {
  double bufferGrid[M][M] = {0};
  int count = 0;
  for (int i = 0; i != M; ++i)
    for (int j = 0; j != M; ++j)
      grid[i][j] = initGrid[i][j] < 0 ? 0.5 : initGrid[i][j];
  for (int canExit = 0; !canExit; ++count) {
    canExit = 1;
    copy(initGrid[0], initGrid[0] + M * M, bufferGrid[0]);
    for (auto i = 0; i < M; i++) {
      for (auto j = 0; j < M; j++) {
        if (initGrid[i][j] >= 0) continue;
        bufferGrid[i][j] = (grid[i + 1][j] + grid[i - 1][j] + grid[i][j + 1] +
                            grid[i][j - 1]) /
                           4.f;
        if (canExit)
          if (fabs((bufferGrid[i][j] - grid[i][j]) / grid[i][j]) > eps)
            canExit = 0;
      }
    }
    copy(bufferGrid[0], bufferGrid[0] + M * M, grid[0]);
#ifndef TEST_TIME
#ifdef GIF
    if (count % INVL == 0) printGrid(grid, name + to_string(count / INVL), os);
#endif
#endif
  }
  return count;
}

// the real eps will be 1e-[eps]
void generateData(double eps, int model) {
  double grid[M][M] = {0};
  double reGrid[M][M] = {0};
  int iterTime;
  clock_t start, end;
  stringstream name;
  name << M << "00" << eps;
  mkdir((to_string(model)).c_str());
  ofstream fs(to_string(model) + "/" + name.str() + ".wls");
  fs.precision(10);
  fs << fixed;
  initGrid(grid, model);
  start = clock();
  iterTime = iter(reGrid, grid, pow(0.1, eps), "data00" + name.str(), fs);
  end = clock();
  cout << "data00" << name.str() << " eps:" << pow(0.1, eps)
       << " time:" << (double)(end - start) / CLOCKS_PER_SEC
       << "s iter time:" << iterTime << " " << endl;
#ifndef TEST_TIME
  printCode(reGrid, iterTime / INVL - (iterTime % INVL == 0),
            "data00" + name.str(), fs);
  system(
      ("cd " + to_string(model) + " & wolframscript -f " + name.str() + ".wls")
          .c_str());
#endif
}

int main() {
  // vector<thread> myVec;
  // // for (int model = 1; model != 4; ++model)
  // int model = 3;
  // for (int eps = 1; eps != 9; ++++eps)
  //   myVec.push_back(thread(generateData, eps, model));
  // for (auto &thr : myVec) thr.join();
  // int eps = 5, model = 3;
  for (int model = 1; model != 4; ++model)
    for (int eps = 1; eps != 9; ++++eps) generateData(eps, model);
}
