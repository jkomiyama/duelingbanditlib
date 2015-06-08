#pragma once

#include "policy.hpp"

namespace bandit{

class RandomPolicy : public Policy{
  const uint K;
public:
  RandomPolicy(uint K): K(K) {
  }
  virtual std::pair<uint,uint> selectNextPair(uint){
    uint i = std::uniform_int_distribution<int>(0, K-1)(randomEngine); 
    uint j = std::uniform_int_distribution<int>(0, K-1)(randomEngine); 
    return std::make_pair(i, j);
  }
  virtual void updateState(std::pair<uint, uint>, uint, uint){
  }
  virtual std::string toString(){
    std::string str="Random";
    return str;
  }
};

} //namespace
