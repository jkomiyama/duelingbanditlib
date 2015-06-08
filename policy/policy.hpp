#pragma once

#include "../bandit/util.hpp"

namespace bandit{

//policy base class
class Policy{
public:
  //base functions should not be called
  virtual std::pair<uint, uint> selectNextPair(uint) = 0;
  virtual void updateState(std::pair<uint, uint>, uint, uint) = 0;
  virtual std::string toString() = 0;
};

typedef std::shared_ptr<Policy> PolicyPtr;

//used by MultiSBM / Sparring
//alpha=3
class MABPolicy{
public:
  //base functions should not be called
  virtual uint selectNextArm() = 0;
  virtual void updateState(uint, double) = 0;
  virtual std::string toString() = 0;
  virtual void debug () = 0;
};

typedef std::shared_ptr<MABPolicy> MABPolicyPtr;

} //namespace
