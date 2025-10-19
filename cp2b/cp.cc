#include <math.h>
#include <numeric>
#include <span>
#include <vector>

double mean_row(std::span<const float> row) {
  const int nx = row.size();
  double acc[] = {0, 0, 0, 0};
  int x = 0;
  for (; x + 7 < nx; x += 8) {
    acc[0] += static_cast<double>(row[x]) / nx;
    acc[1] += static_cast<double>(row[x + 1]) / nx;
    acc[2] += static_cast<double>(row[x + 2]) / nx;
    acc[3] += static_cast<double>(row[x + 3]) / nx;
    acc[0] += static_cast<double>(row[x + 4]) / nx;
    acc[1] += static_cast<double>(row[x + 5]) / nx;
    acc[2] += static_cast<double>(row[x + 6]) / nx;
    acc[3] += static_cast<double>(row[x + 7]) / nx;
  }
  for (; x < nx; ++x) {
    acc[0] += static_cast<double>(row[x]) / nx;
  }

  return std::accumulate(std::begin(acc), std::end(acc), 0.0);
}

double norm_stddev_row(std::span<const double> row) {
  const int nx = row.size();
  double acc[] = {0, 0, 0, 0};
  int x = 0;
  for (; x + 7 < nx; x += 8) {
    acc[0] += (row[x] * row[x]);
    acc[1] += (row[x + 1] * row[x + 1]);
    acc[2] += (row[x + 2] * row[x + 2]);
    acc[3] += (row[x + 3] * row[x + 3]);
    acc[0] += (row[x + 4] * row[x + 4]);
    acc[1] += (row[x + 5] * row[x + 5]);
    acc[2] += (row[x + 6] * row[x + 6]);
    acc[3] += (row[x + 7] * row[x + 7]);
  }
  for (; x<nx;++x) {
    acc[0] += (row[x] * row[x]);
  }
  return std::sqrt(std::accumulate(std::begin(acc), std::end(acc), 0.0));
}

/*
This is the function you need to implement. Quick reference:
- input rows: 0 <= y < ny
- input columns: 0 <= x < nx
- element at row y and column x is stored in data[x + y*nx]
- correlation between rows i and row j has to be stored in result[i + j*ny]
- only parts with 0 <= j <= i < ny need to be filled
*/
void correlate(int ny, int nx, const float *__restrict__ data,
               float *__restrict__ result) {
  // norm[y] = {x - x_} / stddev
  std::vector<double> norm(ny * nx, 0.0f);

  #pragma omp parallel for
  for (int y = 0; y < ny; ++y) {
    auto data_row = std::span(data+y*nx, nx);
    auto norm_row = std::span(norm.data()+y*nx, nx);

    const double mean = mean_row(data_row);
    for (int x = 0; x < nx; ++x)
      norm_row[x] = (double)data_row[x] - mean;
    const double stddev = norm_stddev_row(norm_row);
    for (int x = 0; x < nx; ++x) {
      norm_row[x] /= stddev;
    }
  }

  // sum((x-x_)(y-y_)) / (stddev(x).stddev(y))
  // NORM*NORM_T
  auto pearson = [nx, &norm](int i, int j) {
    auto row_i = std::span(norm.data() + i * nx, nx);
    auto row_j = std::span(norm.data() + j * nx, nx);

    // independent accumulators;
    double acc[] = {0, 0, 0, 0};
    int x = 0;
    for (; x + 7 < nx; x += 8) {
      acc[0] += (row_i[x]) * (row_j[x]);
      acc[1] += (row_i[x + 1]) * (row_j[x + 1]);
      acc[2] += (row_i[x + 2]) * (row_j[x + 2]);
      acc[3] += (row_i[x + 3]) * (row_j[x + 3]);
      acc[0] += (row_i[x + 4]) * (row_j[x + 4]);
      acc[1] += (row_i[x + 5]) * (row_j[x + 5]);
      acc[2] += (row_i[x + 6]) * (row_j[x + 6]);
      acc[3] += (row_i[x + 7]) * (row_j[x + 7]);
    }
    for (; x < nx; ++x)
      acc[0] += (row_i[x]) * (row_j[x]);

    return std::accumulate(std::begin(acc), std::end(acc), (double)0.0f);
  };

  #pragma omp parallel for
  for (int i = 0; i < ny; ++i) {
    for (int j = 0; j <= i; ++j) {
      result[i + j * ny] = pearson(i, j);
    }
  }
}
