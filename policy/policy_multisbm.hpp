#pragma once

#include "policy.hpp"

namespace bandit{

//MultiSBM 
class MultiSBMPolicy : public Policy{
  const uint K;
  const uint T;
  const std::vector<MABPolicyPtr> sbm;
  uint current;
public:
  MultiSBMPolicy(uint K, uint T, std::vector<MABPolicyPtr> sbm): K(K), T(T), sbm(sbm) {
    current = std::uniform_int_distribution<int>(0, K-1)(randomEngine); 
  }
  virtual std::pair<uint,uint> selectNextPair(uint /*t*/){
    uint l = current;
    uint m = sbm[l]->selectNextArm();
    return std::make_pair(l, m);
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint /*t*/){
    uint l = armPair.first;
    uint m = armPair.second;
    if(result == 1){
      sbm[l]->updateState(m, 0.0);
    }else if(result == 0){
      sbm[l]->updateState(m, 1.0);
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }
    current = armPair.second;
  }
  virtual std::string toString(){
    std::string str="MultiSBM with \""+sbm[0]->toString();
    return str;
  }
};

} //namespace
