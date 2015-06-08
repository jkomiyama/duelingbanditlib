#pragma once

#include "../bandit/util.hpp"

namespace bandit{

std::vector<std::vector<double> > prefMatrixFromFile(std::string filename){
  uint K = 0;
  std::vector<std::vector<double> > prefMatrix;
  std::vector<std::string> lines = readlines(filename);
  for(uint i=0;i<lines.size();++i){
    std::string line = lines[i];
    if( (line.size()==0) || (line[0]=='#') ){
      continue;
    }  
    std::vector<std::string> sps = split(line, ' ');
    if(K==0){
      K = sps.size();
    }else if(K != sps.size()){
      std::cout << "Error: line size does not match in prefMatrixFromFile" << std::endl;
      exit(0);
    }
    std::vector<double> prefLine;
    for(uint j=0;j<K;++j){
      prefLine.push_back(stof(sps[j]));
    }
    prefMatrix.push_back(prefLine);
  }
  if(K != prefMatrix.size()){
    std:: cout << K << "," << prefMatrix.size() << std::endl; 
    std::cout << "Error: prefMatrix is not square in prefMatrixFromFile" << std::endl;
    exit(0);
  }
  return prefMatrix;
}

uint prefMatrixCondorcetWinner(const std::vector<std::vector<double> > &prefMatrix){
  const uint K = prefMatrix.size();
  for(uint i=0;i<K;++i){
    bool winner = true;
    for(uint j=0;j<K;++j){
      if(prefMatrix[i][j]<0.5){
        winner = false;
        break;
      }
    }
    if(winner){ return i; }
  }
  std::cout << "Error: prefMatrix has no condorcet winner" << std::endl;
  exit(0);
  return K;
}

double regretOfPair(const std::vector<std::vector<double> > &prefMatrix, uint condorcet, uint i, uint j){
  return (prefMatrix[condorcet][i] + prefMatrix[condorcet][j] - 1.0)/2.0;
}

//first  - x1 log(T) where x1 is the regret against the Condorcet winner
//second - x2 log(T) true lower bound
std::pair<double,double> prefMatrixRegret(const std::vector<std::vector<double> > &prefMatrix){
  uint condorcet = prefMatrixCondorcetWinner(prefMatrix);
  double r1=0, r2=0;
  const uint K = prefMatrix.size();
  for(uint i=0;i<K;++i){
    if(i==condorcet){ continue; }
    double diff_r1 = regretOfPair(prefMatrix, condorcet, condorcet, i) / kl(prefMatrix[i][condorcet], 0.5);
    r1 += diff_r1;
    double diff_r2 = diff_r1;
    for(uint j=0;j<K;++j){
      if(prefMatrix[i][j]<0.5){
        double d = kl(prefMatrix[i][j], 0.5);
        if(d>0.000001){
          if(diff_r2 >  regretOfPair(prefMatrix, condorcet, i, j)/d){
          }
          diff_r2 = std::min(diff_r2, regretOfPair(prefMatrix, condorcet, i, j)/d);
        }
      }
    }
    r2 += diff_r2;
  }
  return std::make_pair(r1, r2);
}

std::vector<std::vector<double> > prefSubMatrix(const std::vector<std::vector<double> > &prefMatrix, const uint SK){
  const uint K = prefMatrix.size();
  if(SK>K){
    std::cout << "Submatrix size is bigger than the base matrix size" << std::endl;
    exit(0);
  }
  const uint winner = prefMatrixCondorcetWinner(prefMatrix);
  std::set<uint> subElems, nonSubElems;
  subElems.insert(winner);
  for(uint i=0;i<K;++i){
    if(i != winner){
      nonSubElems.insert(i);
    }
  }
  while(subElems.size() < SK){
    const uint i = randomElementFromSet(nonSubElems);
    nonSubElems.erase(i);
    subElems.insert(i);
  }
  std::vector <double> subElemsVector;
  std::copy(subElems.begin(), subElems.end(), std::back_inserter(subElemsVector)); 
  std::vector<std::vector<double> > subMatrix = std::vector<std::vector<double> >(SK, std::vector<double>(SK, 0));
  for(uint i=0;i<SK;++i){
    for(uint j=0;j<SK;++j){
      subMatrix[i][j] = prefMatrix[subElemsVector[i]][subElemsVector[j]];
    }
  }
  return subMatrix;
}

class ArmSet{
  const double epsilon = 0.00001;
  const std::vector<std::vector<double> > p;
  const uint K;
  std::uniform_real_distribution<double> unif;
  const uint winner; //condorcet winner
public:
  ArmSet(std::vector<std::vector<double> > preferenceMatrix, uint K)
   : p(preferenceMatrix), K(K), unif(0.0, 1.0), winner(prefMatrixCondorcetWinner(preferenceMatrix)) {
    if(preferenceMatrix.size() != K){
      std::cerr << "matrix size is not K" << std::endl;
      exit(0);
    }
    for(uint i=0;i<K;++i){
      if(preferenceMatrix[i].size() != K){
        std::cerr << "matrix row " << i << " size is not K" << std::endl;
        exit(0);
      }
    }
    for(uint i=0;i<K;++i){
      for(uint j=0;j<K;++j){
        double diff = preferenceMatrix[i][j] + preferenceMatrix[j][i] - 1;
        if(diff*diff > epsilon){
          std::cerr << "preference matrix is not antisymmetric" << std::endl;
          std::cout << "i,j=" << i << "," << j << std::endl;
          std::cout << preferenceMatrix[i][j] << "," << preferenceMatrix[j][i] << std::endl;
          exit(0);
        }
      }
    }
    std::cout << "condorcet winner is arm " << winner << std::endl;
  }
  void checkArms(const std::pair<uint, uint> &armPair){
    uint i = armPair.first;
    uint j = armPair.second;
    if(i>=K){
      std::cerr << "arm i is not in [K]" << std::endl;
      exit(0);
    }
    if(j>=K){
      std::cerr << "arm j is not in [K]" << std::endl;
      exit(0);
    }
  }
  uint pull(const std::pair<uint, uint> &armPair){
    checkArms(armPair);
    double r = unif(randomEngine);
    uint i = armPair.first;
    uint j = armPair.second;
    if(r<p[i][j]){
      return 1;
    }else{
      return 0;
    }
  }
  double getExpectedReward(const std::pair<uint, uint> &armPair){
    checkArms(armPair);
    return p[armPair.first][armPair.second];
  }
  double regretIncrease(const std::pair<uint, uint> &armPair){
    checkArms(armPair);
    uint i = armPair.first;
    uint j = armPair.second;
    return (p[winner][i] + p[winner][j] - 1.0)/2.0;
  }
  std::vector<std::vector<double> > getPreferenceMatrix(){
    return p;
  }
  std::string toString(){
    std::string astr = "Dueling armset K="+itos(K)+"\n";
    astr += "condorcet winner=" + itos(winner) + "\n";
    for(uint i=0;i<K;++i){
      for(uint j=0;j<K;++j){
        astr += dtos(p[i][j]) + " ";
        //astr += dtos(p[i][j]) + "(" + dtos(regretIncrease(std::make_pair(i,j))) +") ";
      }
      astr += "\n";
    }
    return astr;
  }
};

} //namespace
