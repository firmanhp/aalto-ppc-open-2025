#include <math.h>
#include <span>
#include <vector>

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *data, float *result) {
  // precompute mean
  std::vector<double> norm(nx * ny, 0.0f);
  for (int y = 0; y < ny; ++y) {
    double avg = 0.0f;
    for (int x = 0; x < nx; ++x)
      avg += (double)data[x + y * nx] / nx;
    for (int x = 0; x < nx; ++x)
      norm[x + y * nx] = (double)data[x + y * nx] - avg;
  }

  std::vector<double> stddev(ny, 0.0f);
  for (int y = 0; y < ny; ++y) {
    for (int x = 0; x < nx; ++x) {
      stddev[y] += norm[x + y * nx] * norm[x + y * nx];
    }
    stddev[y] = std::sqrt(stddev[y]);
  }

  // sum((x-x_)(y-y_)) / (stddev(x).stddev(y))
  auto pearson = [nx, &norm, &stddev](int i, int j) {
    double ret = 0.0f;
    const double sigma = stddev[i] * stddev[j];
    for (int x = 0; x < nx; ++x) {
      ret += norm[x + i * nx] * norm[x + j * nx] / sigma;
    }
    return ret;
  };

  for (int i = 0; i < ny; ++i) {
    for (int j = 0; j <= i; ++j) {
      result[i + j * ny] = pearson(i, j);
    }
  }
}
