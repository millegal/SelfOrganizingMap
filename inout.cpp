#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>

#include "som_def.h"

#define SEP ','

using namespace std;

// a = unterer Zielwert, b = oberer Zielwert
double scale_value(double x, double min, double max, double a, double b)
{
  if (max - min == 0)
    return a;
  return x == 0 ? a : a + (b - a) * (x - min) / (max - min);
}

vector<double> scale_values(const vector<double> &in_column_vector)
{
  vector<double> scaled;
  scaled.resize(in_column_vector.size());
  double max = numeric_limits<double>::lowest();
  double min = numeric_limits<double>::max();
  double range;
  for (size_t i = 0; i < in_column_vector.size(); ++i)
  {
    max = max < in_column_vector[i] ? in_column_vector[i] : max;
    min = min > in_column_vector[i] ? in_column_vector[i] : min;
  }
  range = max - min;
  if (range > 0)
    for (size_t i = 0; i < in_column_vector.size(); ++i)
      scaled[i] = scale_value(in_column_vector[i], min, max, 0.01, 0.99);
  else // Konstante
    for (size_t i = 0; i < in_column_vector.size(); ++i)
      scaled[i] = 0.05;
  return scaled;
}

void foo_bar(const char *source)
{
  string line;
  ifstream fs;
  vector<string> lines;

  fs.open(source);
  if (fs.is_open())
    while (getline(fs, line))
      lines.push_back(line);
  fs.close();

  vector<vector<double>> values;
  vector<string> header;
  parse_lines(lines, values, header);
  auto merkmale = get_merkmal(values, header);
  // cout.precision(5);
  // cout.width(8);
  // cout.fill('.');
  cout << setw(10) << left << "Name" << right << setw(10) << "min" << setw(10) << "max" << endl;
  for (size_t i = 0; i < merkmale.size(); ++i)
    cout << setw(10) << left << merkmale[i].name << right << setw(10) << setprecision(5) << fixed << merkmale[i].min << setw(10) << merkmale[i].max << endl;

  vector<double> tmp_col;
  for (size_t i = 0; i < header.size(); ++i)
  {
    tmp_col.clear();
    for (size_t row = 0; row < values.size(); ++row)
    {
      tmp_col.push_back(values[row][i]);
    }
    // Spalte skalieren
    auto column = scale_values(tmp_col);

    // Skalierten Werte zurückschreiben
    for (size_t row = 0; row < values.size(); ++row)
    {
      values[row][i] = column[row];
    }
    // for (size_t n = 0; n < column.size(); ++n)
    //   cout << values[n][i] << endl;
  }
}
vector<Merkmal> get_merkmal(const vector<vector<double>> &in_values, const vector<string> &in_header)
{
  vector<Merkmal> ret_value;
  for (size_t col = 0; col < in_header.size(); ++col)
  {
    Merkmal tmp;
    tmp.min = numeric_limits<double>::max();
    tmp.max = numeric_limits<double>::lowest();
    tmp.name = in_header[col];
    for (size_t row = 0; row < in_values.size(); ++row)
    {
      tmp.min = in_values[row][col] < tmp.min ? in_values[row][col] : tmp.min;
      tmp.max = in_values[row][col] > tmp.max ? in_values[row][col] : tmp.max;
    }
    ret_value.push_back(tmp);
  }
  return ret_value;
}
void parse_lines(const vector<string> &in_lines, vector<vector<double>> &out_values, vector<string> &out_header)
{
  string header_row = in_lines[0];
  string current_row;
  string tmp;
  int idx = 0;
  do
  {
    idx = header_row.find(SEP);
    if (idx < 0)
    {
      out_header.push_back(header_row);
    }
    else
    {
      out_header.push_back(header_row.substr(0, idx));
      header_row = header_row.substr(idx + 1);
    }
  } while (idx >= 0);
  for (size_t i = 0; i < out_header.size(); ++i)
    cout << out_header[i] << endl;

  for (size_t i = 1; i < in_lines.size(); ++i)
  {
    vector<double> row;
    current_row = in_lines[i];
    do
    {
      idx = current_row.find(SEP);
      if (idx < 0)
      {
        row.push_back(stod(current_row));
      }
      else
      {
        row.push_back(stod(current_row.substr(0, idx)));
        current_row = current_row.substr(idx + 1);
      }
    } while (idx >= 0);
    out_values.push_back(row);
  }
}
