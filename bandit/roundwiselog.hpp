#pragma once

#include "util.hpp"
#include "../arm/armset.hpp"
#include "../policy/policy.hpp"

namespace bandit{

class RoundwiseLog{
public:
  uint P, T, run;
  std::vector<std::vector<double> > roundwiseRegrets;
  RoundwiseLog(uint P, uint T): P(P), T(T) {
    roundwiseRegrets = std::vector<std::vector<double> >(P, std::vector<double>(T, 0.0));
    run=0;
  }
  //start new run 
  void startRun(){
    run+=1;
  }
  //policy p at round t chose arm k and received reward r
  void record(uint p, uint t, std::pair<uint, uint> k, double regretDelta){
    roundwiseRegrets[p][t]+=regretDelta;
  }
};

class RoundwiseLogWriter{
public:
  static void logWrite(RoundwiseLog &log,
       std::vector<std::string> policyNames,
       std::pair<double, double> r12,
       uint T, std::string filename, bool regretPlot=true){
    const uint P = policyNames.size();
    std::ofstream ofs( filename );
    ofs << "#averaged result over " << log.run << " trials" << std::endl;
    for(uint p=0;p<P;++p){
      ofs << "#policy " << p << " " << policyNames[p] << std::endl;
    }
    ofs.setf(std::ios::fixed, std::ios::floatfield);

    ofs << "#results:" << std::endl;
    ofs << "#T" ;
    ofs << " Lower1" ;
    ofs << " Lower2" ;
    for(uint p=0;p<P;++p){
      if(regretPlot){
        ofs << " regret" << p ;
      }
    }
    ofs << std::endl;
    std::vector<double> cumulatedRegrets(P, 0.0);
    uint next = 1;
    for(uint t=0;t<T;++t){
      for(uint p=0;p<P;++p){
        cumulatedRegrets[p]+=log.roundwiseRegrets[p][t];
      }
      if( (t+1)==next ){
        ofs << (t+1);
        ofs << std::setprecision(2) << " " << (double)(r12.first  * std::log(t+1)) ;
        ofs << std::setprecision(2) << " " << (double)(r12.second * std::log(t+1)) ;
        for(uint p=0;p<P;++p){
          if(regretPlot){
           ofs << std::setprecision(2) << " " << cumulatedRegrets[p]/log.run ;
          }
        }
        ofs << std::endl;
        next = std::min(T-1, std::max<uint>((double)(next)*1.4, next+1));
      }
    }
  }
};

} //namespace
