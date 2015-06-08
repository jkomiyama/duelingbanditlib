#pragma once

#include "policy.hpp"

namespace bandit{

//Relative Upper Confidence Bound (RUCB)
class RUCBPolicy : public Policy{
  const uint K;
  const uint T;
  const double alpha;
  std::vector<std::vector<uint> > W; //W[i][j] = num that i beats j
  std::vector<std::vector<uint> > WpWT; //W + W^T
public:
  RUCBPolicy(uint K, uint T, double alpha = 0.51): K(K), T(T), alpha(alpha) {
    W = std::vector<std::vector<uint> >(K, std::vector<uint>(K, 0));
    WpWT = std::vector<std::vector<uint> >(K, std::vector<uint>(K, 0));
  }
  uint findC(const std::vector<std::vector<double> > &U){ //find arm that is as strong as the other arms
    std::vector<uint> cs;
    for(uint i=0;i<K;++i){
      bool lost = false;
      for(uint j=0;j<K;++j){
        if( (i!=j) && U[i][j]<0.5 ){
          lost = true;
          break;
        }
      }
      if(!lost){
        cs.push_back(i);
      }
    }
    if(cs.size()>=1){
      return cs[std::uniform_int_distribution<int>(0, cs.size()-1)(randomEngine)];
    }else{ //no condorcet winner
      return std::uniform_int_distribution<int>(0, K-1)(randomEngine);
    }
  }
  uint findStrongestOpponent(const std::vector<std::vector<double> > &U, uint c){
    uint j = 0;
    double v = 0;
    for(uint i=0;i<K;++i){
      if(U[i][c] > v){
        v = U[i][c];
        j = i;
      }
    }
    return j;
  }
  virtual std::pair<uint,uint> selectNextPair(uint t){
    //build U
    std::vector<std::vector<double> > U = std::vector<std::vector<double> >(K, std::vector<double>(K, 0));
    for(uint i=0;i<K;++i){
      for(uint j=0;j<K;++j){
        if(i==j){
          U[i][j] = 0.5;
        }else if(WpWT[i][j]==0){
          U[i][j] = 1.0;
        }else{
          U[i][j] = (double)(W[i][j])/(WpWT[i][j]) + sqrt(alpha * log(t) / (double)(WpWT[i][j]) );
        }
      }
    }
    uint c = findC(U);
    uint d = findStrongestOpponent(U, c);
    return std::make_pair(c,d);
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint /*t*/){
    uint i = armPair.first;
    uint j = armPair.second;
    WpWT[i][j] += 1;
    WpWT[j][i] += 1;
    if(result == 1){
      W[i][j]+=1; //i beat j
    }else if(result == 0){
      W[j][i]+=1; //j beat i
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }

  }
  virtual std::string toString(){
    std::string str="RUCB";
    str+=" with alpha="+dtos(alpha);
    return str;
  }
};

} //namespace
