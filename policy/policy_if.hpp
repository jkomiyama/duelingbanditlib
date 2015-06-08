#pragma once

#include "policy.hpp"

namespace bandit{

//Interleaved Filter (IF)
class IFPolicy : public Policy{
  const uint K;
  const uint T;
  const double delta;
  std::vector<std::vector<uint> > resultMatrix;
  std::set<uint> W; //active bandit arms
  std::set<uint> Wr; //active bandit arms that is not hatb and left in the current round robin
  uint hatb;
public:
  IFPolicy(uint K, uint T): K(K), T(T), delta(1.0 / ((double)(T)*K*K) ) {
    resultMatrix = std::vector<std::vector<uint> >(K, std::vector<uint>(K, 0));
    W.clear();
    for(uint i=0;i<K;++i){
      W.insert(i);
    }
    hatb = randomElementFromSet(W);
    Wr = W;
  }
  double getConfidenceSize(uint numComparison){
    if(numComparison == 0){
      return 1.0;
    }
    return sqrt(4.0 * log(1.0/delta) / (double)numComparison);
  }
  double getP(uint i, uint j){
    if ((resultMatrix[i][j]+resultMatrix[j][i]) == 0){
      return 0.5;
    }else{
      double p = resultMatrix[i][j] / (double)(resultMatrix[i][j]+resultMatrix[j][i]);
      return p;
    }
  }
  virtual std::pair<uint,uint> selectNextPair(uint /*t*/){
    if(W.size()==0){
      std::cout << "Error: no arm left" << std::endl;
      exit(0);
    }else if(W.size()==1){ //winner determined (exploitation phase)
      uint i = *W.begin();
      return std::make_pair(i, i);
    }else{ //main routine
      if(Wr.size()==0){
        std::cout << "Error: Wr exhausted" << std::endl;
        exit(0);
      }
      uint b = *Wr.begin();
      return std::make_pair(hatb, b);
    }
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint /*t*/){
    uint i = armPair.first;
    uint j = armPair.second;
    if(W.size()==1){ //already in the exploitation phase: do nothing
      return;
    }
    if(result == 1){
      resultMatrix[i][j]+=1; //i won
    }else if(result == 0){
      resultMatrix[j][i]+=1; //j won
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }
    Wr.erase(*Wr.begin());
    if(Wr.size() == 0){ //ended the current round robin
      std::set<uint> W_cp(W);
      for(std::set<uint>::iterator it = W_cp.begin(); it != W_cp.end();){
        const uint b = *it;
        double P = getP(hatb, b);
        double c = getConfidenceSize(resultMatrix[hatb][b]+resultMatrix[b][hatb]);
        it++;
        if(P-c >0.5){ //hatb is better than b
          W.erase(b);
        }
      }
      for(std::set<uint>::iterator it = W_cp.begin(); it != W_cp.end(); it++){
        const uint bprime = *it;
        double P = getP(hatb, bprime);
        double c = getConfidenceSize(resultMatrix[hatb][bprime]+resultMatrix[bprime][hatb]);
        if(P+c < 0.5){ //hatb is inferior to b
          for(std::set<uint>::iterator it2 = W_cp.begin(); it2 != W_cp.end();){
            const uint b = *it2;
            double P2 = getP(hatb, b);
            it2++;
            if(P2 > 0.5){ //hatb is better than b
              W.erase(b);
            }
          }
          W.erase(hatb);
          hatb = bprime;
          break;
        }
      }
      //next round robin
      Wr = W;
      Wr.erase(hatb);
    }
  }
  virtual std::string toString(){
    std::string str="IF";
    return str;
  }
};

} //namespace
