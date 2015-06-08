#pragma once

#include "policy.hpp"

namespace bandit{

//Savage algorithm
class SavagePolicy : public Policy{
  const uint K; //N=K^2 in condorcet (or other dueling) SAVAGE
  const uint T;
  const double delta;
  std::vector<std::vector<uint> > resultMatrix;
  std::set<uint> W; //active bandit arms
public:
  SavagePolicy(uint K, uint T, double delta): K(K), T(T), delta(delta) { //delta = 1/T at $5
    resultMatrix = std::vector<std::vector<uint> >(K, std::vector<uint>(K, 0));
    W.clear();
    for(uint i=0;i<K;++i){
      W.insert(i);
    }
  }
  double getConfidenceBound(uint sampleNum){
    return sqrt(log(2*K*T/delta)/(2.0*sampleNum));
  }
  std::pair<uint, uint> minimumTrialPair(){
    if(W.size() <= 1){
      std::cout << "Error: winner already defined" << std::endl;
    }
    std::pair<uint, uint> p;
    uint mv = UINT_MAX;
    //minimum
    for(std::set<uint>::iterator it = W.begin(); it != W.end(); it++){
      const uint i = *it;
      for(std::set<uint>::iterator it2 = W.begin(); it2 != W.end(); it2++){
        const uint j = *it2;
        uint n = resultMatrix[i][j] + resultMatrix[j][i];
        if(n<mv){
          mv = n;
          p = std::make_pair(i,j);
        }
      }
    }
    if(mv == UINT_MAX){
      std::cout << "Error: minimum sample not found in Savage::minimumTrialPair" << std::endl;
      exit(0);
    }
    return p;
  }
  virtual std::pair<uint,uint> selectNextPair(uint /*t*/){
    if(W.size()==0){
      std::cout << "Error: no arm left" << std::endl;
      exit(0);
    }else if(W.size()==1){ //winner determined
      uint i = *W.begin();
      return std::make_pair(i, i);
    }else{ //main routine
      return minimumTrialPair();
    }
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint /*t*/){
    uint i = armPair.first;
    uint j = armPair.second;
    if(result == 1){
      resultMatrix[i][j]+=1; //i beat j
    }else if(result == 0){
      resultMatrix[j][i]+=1; //j beat i
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }
    for(std::set<uint>::iterator it = W.begin(); it != W.end();){
      const uint i = *it;
      it++;
      for(uint j=0;j<K;++j){
        uint n = resultMatrix[i][j] + resultMatrix[j][i];
        if(n==0){ continue; }
        double p = resultMatrix[i][j] / (double)(n);
        double c = getConfidenceBound(n);
        if(p+c< 0.5){ //i is not as good as j
          W.erase(i);
          break;
        }
      }
    }
  }
  virtual std::string toString(){
    std::string str="Savage";
    str+=" with delta ="+dtos(delta);
    return str;
  }
};

} //namespace
