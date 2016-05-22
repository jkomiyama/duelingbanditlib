#pragma once

#include "policy.hpp"

namespace bandit{

//Relative Minimum Deterministic Empirical Divergence (RMED)
class RMEDPolicy : public Policy{
  const uint K;
  const double c;
  const uint T;
  const double alpha1;
  const double kweight;
  const bool RMED1;
  const bool RMED2;
  const bool RMED2Finite;
  int Tl; //initial exploration per arm
  std::vector<std::vector<uint> > resultMatrix;
  std::set<uint> LC, LRc, LN; //Note: LRc is the complementary set of L_R
  bool firstRoundInMainPhase; //for RMED2Finite
  std::vector<uint> bsi; //for RMED2Finite
public:
  RMEDPolicy(uint K, uint T, uint mode=1, double kweight=0.3, double alpha1 = 3.0)
   : K(K), c(1), T(T), alpha1(alpha1), kweight(kweight), RMED1(mode==1), RMED2(mode==2), RMED2Finite(mode==3) {
    const uint Ninit=0;
    resultMatrix = std::vector<std::vector<uint> >(K, std::vector<uint>(K, Ninit));
    if(mode > 3 || mode == 0){ 
      std::cout << "RMED unknown mode" << std::endl;
      exit(0);
    }
    firstRoundInMainPhase = true;
  }
  double empiricalWinningRate(uint i, uint j){
    if(i==j) return 0.5;
    double denominator = resultMatrix[i][j] + resultMatrix[j][i];
    if(denominator == 0){
      return 0.5;
    }
    return resultMatrix[i][j] / denominator;
  }
  uint mostLosingArm(uint i, std::set<uint> &opponents){ //find the arm that beats i the most
    uint cbest = -1;
    double worst_p = 1.0; 
    for(std::set<uint>::iterator it=opponents.begin(); it!=opponents.end(); ++it){
      uint j = *it;
      if(i==j) continue; //this should never satisfied...
      double pij = empiricalWinningRate(i, j);
      if(pij <= worst_p){ //
        worst_p = pij;
        cbest = j;
      }
    }
    return cbest;
  }
  uint smallestRegretArm(uint i_opt, uint i){ 
    uint cbest = i_opt;
    double smallest_v = 10000000.0; 
    for(uint j=0;j<K;++j){
      double pij = empiricalWinningRate(i, j);
      if(pij>=0.5) continue;
      double p_istar_i = empiricalWinningRate(i_opt, i);
      double p_istar_j = empiricalWinningRate(i_opt, j);
      double v = std::max(p_istar_i + p_istar_j - 1.0, 0.01);
      double denominator = kl(pij, 0.5);
      v /= denominator;
      if(v < smallest_v){
        smallest_v = v;
        cbest = j;
      }
    }
    return cbest; //note that cbest can be i
  }
  uint pairSelectedNum(uint i, uint j){
    return resultMatrix[i][j] + resultMatrix[j][i];
  }
  double logLikelihood(uint i){ //likelihood that arm i is the best
    if(i>=K){
      std::cerr << "i is not in [K] in RMEDPolicy::likelihood." << std::endl;
      exit(0);
    }
    double ll = 0.0; //exp(-ill) = likelihood
    for(uint j=0;j<K;++j){
      if(i==j){ continue; }
      double mu_ij = empiricalWinningRate(i, j);
      if(mu_ij < 0.5){
        //double N_ij  = pairSelectedNum(i, j);
        ll -= pairSelectedNum(i,j) * kl(mu_ij, 0.5);
      }
    }
    return ll;
  }
  std::set<uint> getOpponents(uint i){
    std::set<uint> ops;
    for(uint j=0;j<K;++j){
      if(i==j) continue;
      double pij = empiricalWinningRate(i, j);
      if(pij <= 0.50001){
        ops.insert(j);
      }
    }
    return ops;
  }
  virtual std::pair<uint,uint> selectNextPair(uint t){
    //check if exploration is needed or not
    for(uint i=0;i<K;++i){
      for(uint j=i+1;j<K;++j){
        if(i == j) continue;
        uint Nij = resultMatrix[i][j] + resultMatrix[j][i];
        if( (RMED1 && Nij == 0) || 
            (RMED2 && Nij < alpha1*log(log(t)+0.01) ) ||
            (RMED2Finite && Nij < alpha1*log(log(T)+0.01) ) ){
          return std::make_pair(i, j);
        }
      }
    }
    //main phase
    std::vector<double> lls = std::vector<double>(K, 0.0);
    for(uint i=0;i<K;++i){
      lls[i] = logLikelihood(i);
    }
    uint i_opt = vectorMaxIndex(lls);
    if(firstRoundInMainPhase && RMED2Finite){ //RMED2Finite fix opponent
      for(uint i=0;i<K;++i){
        bsi.push_back(smallestRegretArm(i_opt, i));
      }
      firstRoundInMainPhase = false;
    }
    uint i = (*LC.begin());
    std::set<uint> opponents = getOpponents(i);
    if(RMED1){
      if(opponents.find(i_opt) != opponents.end()){
        return std::make_pair(i, i_opt);
      }else if(opponents.size()>0){
        return std::make_pair(i, mostLosingArm(i, opponents));
      }else{
        return std::make_pair(i, i_opt);
      }
    }else if(RMED2){
      uint bs_i = smallestRegretArm(i_opt, i);
      //double w_smallest = empiricalWinningRate(i, bs_i);
      double Nib = pairSelectedNum(i, bs_i);
      if(opponents.find(i_opt) != opponents.end()){
        if(pairSelectedNum(i, i_opt) <= Nib / log(log(t)+0.01) ){
          return std::make_pair(i, i_opt);
        }
      }
      if(opponents.find(bs_i) != opponents.end()){
        return std::make_pair(i, bs_i);
      }else if(opponents.find(i_opt) != opponents.end()){
        return std::make_pair(i, i_opt);
      }else if(opponents.size()>0){
        return std::make_pair(i, mostLosingArm(i, opponents));
      }else{
        return std::make_pair(i, i_opt);
      }
    }else if(RMED2Finite){
      uint bs_i = bsi[i];
      //double w_smallest = empiricalWinningRate(i, bs_i);
      double Nib = pairSelectedNum(i, bs_i);
      if(opponents.find(i_opt) != opponents.end()){
        if(pairSelectedNum(i, i_opt) <= Nib / log(log(t)+0.01) ){
          return std::make_pair(i, i_opt);
        }
      }
      if(opponents.find(bs_i) != opponents.end()){
        return std::make_pair(i, bs_i);
      }else if(opponents.find(i_opt) != opponents.end()){
        return std::make_pair(i, i_opt);
      }else if(opponents.size()>0){
        return std::make_pair(i, mostLosingArm(i, opponents));
      }else{
        return std::make_pair(i, i_opt);
      }
    }
    std::cout << RMED1 << std::endl;
    std::cout << RMED2 << std::endl;
    std::cout << RMED2Finite << std::endl;
    std::cout << "Error: probably illegal mode in RMED" << std::endl;
    exit(0);
    return std::make_pair(0, 0);
  }
  virtual void updateState(std::pair<uint, uint> armPair, uint result, uint t){
    uint i = armPair.first;
    uint j = armPair.second;
    if(result == 1){
      resultMatrix[i][j]+=1; //i won
    }else if(result == 0){
      resultMatrix[j][i]+=1; //j won
    }else{
      std::cerr << "unknown result: " << result << std::endl;
      exit(0);
    }

    LC.erase(i);
    LRc.insert(i);
    std::vector<double> lls = std::vector<double>(K, 0.0);
    for(uint i=0;i<K;++i){
      lls[i] = logLikelihood(i);
    }
    double lls_opt = vectorMax(lls);
    for(std::set<uint>::iterator it=LRc.begin(); it!=LRc.end(); ++it){
      uint j = (*it);
      if(lls_opt - lls[j] <= log((double)t) + kweight*pow(K, 1.01) ){
        LN.insert(j);
      }
    }
    if(LC.empty()){
      LC = LN;
      LRc.clear();
      for(uint i=0;i<K;++i){
        if(LC.find(i) == LC.end()){
          LRc.insert(i);
        }
      }
      LN.clear();
    }
  }
  virtual std::string toString(){
    std::string str="";
    if(RMED1){
      str+="RMED1 with kweight="+dtos(kweight);
    }else if(RMED2){
      str+="RMED2 with kweight="+dtos(kweight)+" a1="+dtos(alpha1);
    }else if(RMED2Finite){
      str+="RMED2F with kweight="+dtos(kweight)+" a1="+dtos(alpha1);
    }
    return str;
  }
};

} //namespace
