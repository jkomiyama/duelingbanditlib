#pragma once

#include <vector>
#include <string>
#include <set>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <climits> //INT_MAX

namespace bandit{

typedef unsigned int uint;

const double pi = 3.14159265;
const double e  = 2.718281828;

//RNG engine
std::mt19937 randomEngine(std::time(0));

template<class T>
int vectorMaxIndex(const std::vector<T> &elems){
  int m=0;
  T mv=elems[0];
  for(unsigned int i=0;i<elems.size();++i){
    if(elems[i]>mv){
      mv=elems[i];
      m=i;
    }
  }
  return m;
}

template<class T>
T vectorMax(std::vector<T> &elems){
  T mv=elems[0];
  for(unsigned int i=0;i<elems.size();++i){
    if(elems[i]>mv){
      mv=elems[i];
    }
  }
  return mv;
}

template<class T>
std::pair<uint,T> mapMinIndexAndVal(std::map<uint, T> &elems){
  uint m = elems.begin()->first;
  T mv = elems.begin()->second;
  for(typename std::map<uint, T>::iterator it=elems.begin();it!=elems.end();++it){
    uint i = it->first;
    T v = it->second;
    if(v < mv){
      mv = v;
      m = i;
    }
  }
  return std::make_pair(m, mv);
}

template<class T>
std::pair<uint,T> mapMaxIndexAndVal(std::map<uint, T> &elems){
  uint m = elems.begin()->first;
  T mv = elems.begin()->second;
  for(typename std::map<uint, T>::iterator it=elems.begin();it!=elems.end();++it){
    uint i = it->first;
    T v = it->second;
    if(v > mv){
      mv = v;
      m = i;
    }
  }
  return std::make_pair(m, mv);
}

template<class T>
T vectorSum(const std::vector<T> &elems){
  T s=0;
  for(uint i=0;i<elems.size();++i){
    s+=elems[i];
  }
  return s;
}

//int -> string
std::string itos(int number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}

//double -> string
std::string dtos(double number)
{
  std::stringstream ss;
  ss << number;
  return ss.str();
}

template<class T>
T randomElementFromSet(const std::set<T> &s){
  uint l = s.size();
  uint i = std::uniform_int_distribution<int>(0, l-1)(randomEngine); 
  typename std::set<T>::const_iterator it(s.begin());
  std::advance(it, i); 
  return *it;
}

//split by char
std::vector<std::string> split(const std::string &str, char delim){
  std::vector<std::string> res;
  int current = 0, found;
  while((found = str.find_first_of(delim, current)) != (int)(std::string::npos) ){
    res.push_back(std::string(str, current, found - current));
    current = found + 1;
  }
  res.push_back(std::string(str, current, str.size() - current));
  return res;
}

//split by string
std::vector<std::string> split2(const std::string &str, const std::string &delim){
  std::vector<std::string> res;
  int current = 0, found, delimlen = (int)delim.size();
  while((found = str.find(delim, current)) != (int)(std::string::npos)){
    res.push_back(std::string(str, current, found - current));
    current = found + delimlen;
  }
    res.push_back(std::string(str, current, str.size() - current));
    return res;
}

std::vector<std::string> readlines(const std::string filename, bool skipBlankLine = true){
  std::ifstream ifs(filename);
  if(!ifs.is_open()){
    std::cerr << "ERROR: File " << filename << " not found." << std::endl;
    exit(0);
  }
  std::vector<std::string> lines;
  std::string str; 
  while (std::getline(ifs, str))
  {
    if(skipBlankLine && (str.size()==0) ) continue;
    lines.push_back(str);
  } 
  ifs.close();
  return lines;
}

std::string setToString(std::set<uint> s){
  std::string astr = "";
  for(std::set<uint>::iterator it = s.begin(); it != s.end();it++){
    if(astr.length()==0){
      astr += itos(*it);
    }else{
      astr += std::string(" ") + itos(*it);
    }
  }
  return astr;
}

std::string vectorToString(std::vector<uint> v){
  std::string astr = "";
  for(std::vector<uint>::iterator it = v.begin(); it != v.end();it++){
    if(astr.length()==0){
      astr += itos(*it);
    }else{
      astr += std::string(" ") + itos(*it);
    }
  }
  return astr;
}

double kl(double p, double q){ //calculate kl-divergence. p \in [0,1] and q \in (0,1)
  //const double v =  p * log(p/q) + (1-p)*log((1-p)/(1-q));
  double v = 0.0;
  if(q>=1.0 || q<= 0.0){
    std::cout << "Error: q must be in (0,1)" << std::endl;
    exit(0);
  }
  if(p>0){ v += p * log(p/q); }
  if(p<1){ v += (1-p)*log((1-p)/(1-q)); }
  return v;
}

} //namespace
