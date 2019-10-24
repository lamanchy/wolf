//
// Created by lamanchy on 17.7.19.
//
#include <gzip/compress.hpp>
#include <iostream>
#include <fstream>
#include <gzip/decompress.hpp>
#include <base/pipeline.h>

int main(int argc, char **argv) {

  std::string s("ahoj");
  std::string s2("ahoj2");
  std::string c = gzip::compress(s.c_str(), s.size());
  std::string c2 = gzip::compress(s.c_str(), s.size());

  std::ofstream myfile;
  myfile.open("example.txt");
  myfile << c;
  myfile.close();

  std::ifstream file2;
  file2.open("example.txt");
  file2 >> c;
  file2 >> c2;
  file2.close();

  std::cout << gzip::decompress(c.c_str(), c.size()) << std::endl;
  std::cout << gzip::decompress(c2.c_str(), c2.size()) << std::endl;
}