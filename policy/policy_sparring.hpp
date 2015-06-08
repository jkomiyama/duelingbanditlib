#pragma once

#include "policy.hpp"

namespace bandit{

//Sparring 
class SparringPolicy : public Policy{
  const uint K;
  const uint T;
  const MABPolicyPtr left;
  const MABPolicyPtr right;
public:
  SparringPolicy(uint K, uint T, MABPolicyPtr left, MABPolicyPtr right): K(K), T(T), left(left), right(right) {

  }
  virtual std::pair<uint,uint> selectNextPair(uint /*t*/){
    uint l = left->selectNextArm();
    uint m = right->selectNextArm();
    return std::make_pair(l, m);
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint /*t*/){
    uint l = armPair.first, m = armPair.second;
    if(result == 1){
      left->updateState(l, 1.0);
      right->updateState(m, 0.0);
    }else if(result == 0){
      left->updateState(l, 0.0);
      right->updateState(m, 1.0);
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }
  }
  virtual std::string toString(){
    std::string str="Sparring with \""+left->toString()+"\" and \""+right->toString()+"\"";
    return str;
  }
};

} //namespace
