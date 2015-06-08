#pragma once

#include "policy.hpp"

namespace bandit{

class UCBMABPolicy : public MABPolicy{
  const uint K;
  const double alpha;
  std::vector<double> Ni;
  std::vector<double> Gi; 
public:
  UCBMABPolicy(uint K, double alpha=2.01): K(K), alpha(alpha) {
    reset();
  }
  void reset(){
    Ni = std::vector<double>(K, 0.0);
    Gi = std::vector<double>(K, 0.0);
  }
  virtual uint selectNextArm(){
    double n = vectorSum(Ni);
    std::vector<double> indices = std::vector<double>(K, 0.0); 
    for(uint k=0;k<K;++k){
      if(Ni[k]==0){
        return k;
      }
      //UCB index
      indices[k] = (Gi[k]/Ni[k]) + sqrt( (alpha * log(n)) / (double)Ni[k] );
    }

    int targetArm = vectorMaxIndex(indices);

    return targetArm;
  }
  virtual void updateState(uint k, double r){
    Ni[k]+=1;
    Gi[k]+=r;
  }
  virtual std::string toString(){
    std::string str="UCB-MABPolicy with alpha=";
    str+=dtos(alpha);
    return str;
  }
  virtual void debug(){
    for(uint k=0;k<K;++k){
      std::cout << "N[" << k << "]=" << Ni[k] <<" G[" << k << "]=" << Gi[k] << " E=" << Gi[k]/Ni[k] << std::endl;
    }
  }
};

} //namespace
