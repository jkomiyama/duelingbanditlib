#pragma once

#include "policy.hpp"

namespace bandit{

//Beat the Mean Bandit (BTM)
class BTMPolicy : public Policy{
  const uint K;
  const uint T;
  const double gamma;
  std::vector<std::vector<uint> > winMatrix;
  std::vector<std::vector<uint> > loseMatrix;
  std::set<uint> W; //active bandit arms
  std::vector<uint> n_b;
  std::vector<uint> w_b; 
public:
  BTMPolicy(uint K, uint T, double gamma=1.2): K(K), T(T), gamma(gamma) { 
    winMatrix = std::vector<std::vector<uint> >(K, std::vector<uint>(K, 0));
    loseMatrix = std::vector<std::vector<uint> >(K, std::vector<uint>(K, 0));
    W.clear();
    for(uint i=0;i<K;++i){
      W.insert(i);
    }
    for(uint i=0;i<K;++i){
      n_b.push_back(0);
      w_b.push_back(0);
    }
  }
  double getConfidenceBound(uint k){
    //BTM^T (online) confidence bound
    if(k==0){ return 1.0; }
    double delta = 1.0/(2.0*T*K);
    //return 3.0 * gamma * gamma * sqrt(log(1.0/delta)/k);
    return gamma * gamma * sqrt(log(1.0/delta)/k); //"tight" conf interval
  }
  std::pair<uint,uint> minimumTrialArmAndNum(){ // argmin n_b, min n_b
    uint b = K; //initial value void
    uint n_b_min =  UINT_MAX;
    for(std::set<uint>::iterator it = W.begin(); it != W.end(); it++){
      uint i = *it;
      if(n_b[i] < n_b_min){
        b = i;
        n_b_min = n_b[i];
      }
    }
    if(b == K){
      std::cout << "Error: no best arm in minimumTrialArm" << std::endl;
      exit(0);
    }
    return std::make_pair(b, n_b_min);
  }
  uint minimumTrialArm(){
    return minimumTrialArmAndNum().first;
  }
  uint minimumTrialNum(){
    return minimumTrialArmAndNum().second;
  }
  virtual std::pair<uint,uint> selectNextPair(uint /*t*/){
    if(W.size()==0){
      std::cout << "Error: no arm left" << std::endl;
      exit(0);
    }else if(W.size()==1){ //winner determined
      uint i = *W.begin();
      return std::make_pair(i, i);
    }else{ //main routine
      const uint b = minimumTrialArm();
      W.erase(b); //aboid non-informative comparison
      const uint bprime = randomElementFromSet(W);
      W.insert(b);
      return std::make_pair(b, bprime);
    }
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint /*t*/){
    uint i = armPair.first;
    uint j = armPair.second;
    if(W.size()==1){ //in the exploiration phase: no update
      return;
    }
    n_b[i]+=1;
    if(result == 1){
      winMatrix[i][j]+=1; //i beat j
      w_b[i]+=1;
    }else if(result == 0){
      loseMatrix[i][j]+=1; //j beat i
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }

    //remove bad arm 
    std::map<uint, double> P;
    for(std::set<uint>::iterator it = W.begin(); it != W.end(); it++){
      uint i = *it;
      //std::cout << "i,w_b[i],n_b[i]=" << i << "," << w_b[i] << "," << n_b[i] << std::endl;
      if(n_b[i]==0){
        P[i] = 0.5;
      }else{
        P[i] = w_b[i]/(double)(n_b[i]);
      }
      //std::cout << "P[" << i << "]=" << P[i] << std::endl;
    }
    std::pair<uint, double> PMinIndexAndVal = mapMinIndexAndVal(P);
    std::pair<uint, double> PMaxIndexAndVal = mapMaxIndexAndVal(P);
    uint nstar = minimumTrialNum();
    double c = getConfidenceBound(nstar);
    if(PMinIndexAndVal.second + c < PMaxIndexAndVal.second - c){
      uint bprime = PMinIndexAndVal.first;
      W.erase(bprime);
      for(std::set<uint>::iterator it = W.begin(); it != W.end(); it++){
        const uint i = *it;
        n_b[i] -= (winMatrix[i][bprime]+loseMatrix[i][bprime]);
        w_b[i] -= winMatrix[i][bprime];
      }
    }
  }
  virtual std::string toString(){
    std::string str="BTM";
    str+=" with gamma="+dtos(gamma);
    return str;
  }
};

} //namespace
