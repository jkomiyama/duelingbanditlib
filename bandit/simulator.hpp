#pragma once

#include "util.hpp"
#include "../arm/armset.hpp"
#include "../policy/policy.hpp"
#include "../bandit/roundwiselog.hpp"

namespace bandit{


template<class Log = RoundwiseLog>
class Simulator{
  ArmSet armset;
  std::vector<PolicyPtr> policies;
  std::pair<double, double> rcoef;
public:
  Simulator(ArmSet armset, const std::vector<PolicyPtr> &policies):
    armset(armset), policies(policies) {
  }
  void debugPrint(){
    std::cout << armset.toString() << std::endl;
    for(uint p=0;p<policies.size();++p){
      std::cout << policies[p]->toString() << std::endl;
    }  
  }
  void run(Log &log, const uint T){
    debugPrint();
    log.startRun();
    for(uint t=1;t<=T;++t){
    /*
      if((t%10000)==0){
        std::cout << "t=" << t << std::endl;
        std::cout << armset.toString() << std::endl;
      }
      */
      if((t%1000000)==0){
        std::cout << "t=" << t << std::endl;
      }
      for(uint p=0;p<policies.size();++p){
        execSingleRound(log, p, t);
      }
    }
  }
  void execSingleRound(Log &log, uint p, uint t){
    std::pair<uint, uint> armPair = policies[p]->selectNextPair(t);
    uint reward = armset.pull(armPair);
    policies[p]->updateState(armPair, reward, t); 
    double regret = armset.regretIncrease(armPair); 
    log.record(p, t, armPair, regret);
  }
};


} //namespace
