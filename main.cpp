#include <iostream>

#include "bandit/util.hpp"
#include "arm/armset.hpp"
#include "policy/policy.hpp"
#include "policy/policy_random.hpp"
#include "policy/policy_rmed.hpp"
#include "policy/policy_rucb_arxiv.hpp"
#include "policy/policy_rucb_icml.hpp"
#include "policy/policy_btm.hpp"
#include "policy/policy_if.hpp"
#include "policy/policy_savage.hpp"
#include "policy/policy_ucb.hpp"
#include "policy/policy_sparring.hpp"
#include "policy/policy_multisbm.hpp"
#include "bandit/simulator.hpp"
#include "cmdline.h"

using namespace std;
using namespace bandit;

uint policyNum(uint algorithmmode){
  if(algorithmmode==1){
    return 10; //all
  }else{
    cout << "Error: unknown arm mode" << std::endl;
    exit(0);
  }
  return 0;
}

void mainroutine(const uint T, const uint RunNum, const uint mode, const uint algorithmmode, const uint subSize, string filename, string outfile){
  std::vector<std::vector<double> > prefMatrix;
  if(mode == 1){
    prefMatrix = prefMatrixFromFile(filename);
  }else if(mode == 2){ //use submatrix
    prefMatrix = prefMatrixFromFile(filename);
    if(subSize < prefMatrix.size()){
      prefMatrix = prefSubMatrix(prefMatrix, subSize);
    }
  }else{
    std::cout << "Error: unknown mode" << std::endl;
    exit(0);
  }
  const uint K = prefMatrix.size(); //=arms.size();
  const uint P = policyNum(algorithmmode);  //=policies.size()
  RoundwiseLog log(P, T) ;

  ArmSet armset(prefMatrix, K);
  vector<PolicyPtr> policies;
  cout << "Simulating " << RunNum << " runs." << endl;
  for(uint run=0;run<RunNum;++run){
    cout << "run=" << run << endl;
    policies.clear();
    if(algorithmmode == 1){
      policies.push_back( PolicyPtr(new RMEDPolicy(K, T, 1)) );
      policies.push_back( PolicyPtr(new RMEDPolicy(K, T, 2)) );
      policies.push_back( PolicyPtr(new RMEDPolicy(K, T, 3)) );
      policies.push_back( PolicyPtr(new RUCBPolicy(K, T)) );
      policies.push_back( PolicyPtr(new RandomPolicy(K)) );
      policies.push_back( PolicyPtr(new BTMPolicy(K, T)) );
      policies.push_back( PolicyPtr(new IFPolicy(K, T)) );
      policies.push_back( PolicyPtr(new SavagePolicy(K, T, 1.0/T)) );
      MABPolicyPtr  leftMAB(new UCBMABPolicy(K, 2.50));
      MABPolicyPtr rightMAB(new UCBMABPolicy(K, 2.50));
      policies.push_back( PolicyPtr(new SparringPolicy(K, T, leftMAB, rightMAB)) );
      std::vector<MABPolicyPtr> sbm;
      for(uint si=0;si<K;++si){
        sbm.push_back(MABPolicyPtr(new UCBMABPolicy(K, 2.50)));
      }
      policies.push_back( PolicyPtr(new MultiSBMPolicy(K, T, sbm)) );
    }

    if(P != policies.size()){
      cout << "policy size does not match" << endl;
      exit(0);
    }
    Simulator<RoundwiseLog> sim(armset, policies);
    sim.run(log, T);
  }

  vector<string> policyNames;
  for(uint p=0;p<policies.size();++p){
    policyNames.push_back( policies[p]->toString() );
  }
  std::pair<double, double> regret_base12 = prefMatrixRegret(prefMatrix);
  std::cout << "writing data to " << string("out/")+outfile << std::endl;
  RoundwiseLogWriter::logWrite(log, policyNames, regret_base12, T, string("out/")+outfile);
}

int main(int argc, char *argv[]){
  cmdline::parser a;
  a.add<uint>("runnum", 'r', "run number", false, 100);
  a.add<uint>("roundnum", 't', "round number", false, 10000);
  a.add<uint>("mode", 'm', "mode (1:full matrix 2:submatrix of size -k)", false, 1);
  a.add<uint>("algorithmmode", 'a', "set of algorithms", false, 1);
  a.add<uint>("subsize", 'k', "submatrix size (mode=3)", false, 16);
  a.add<int>("seed", 's', "random number seed", false, -1); 
  a.add<string>("infile", 'i', "input filename (preference matrix)", false, "prefmats/pref_arxiv.txt");
  a.add<string>("outfile", 'o', "output filename", false, "out1.txt");
  a.parse_check(argc, argv);
  const uint RunNum = a.get<uint>("runnum"), T=a.get<uint>("roundnum"), subSize=a.get<uint>("subsize");
  uint mode = a.get<uint>("mode");
  uint algorithmmode = a.get<uint>("algorithmmode");

  const string prefmatfile = a.get<string>("infile"); 
  const string outfile = a.get<string>("outfile"); 
  int rngSeed = a.get<int>("seed"); 
  if(rngSeed != -1){ 
    cout << "rngSeed=" << rngSeed << endl; 
    randomEngine = std::mt19937(rngSeed); 
  } 

  cout << "DuelingBandit simulation T=" << T << " r=" << RunNum << " m=" << mode << " a=" << algorithmmode << " k=" << subSize << std::endl;
  cout << "reading file: "  << prefmatfile << std::endl;
  mainroutine(T, RunNum, mode, algorithmmode, subSize, prefmatfile, outfile);

  return 0;
}
