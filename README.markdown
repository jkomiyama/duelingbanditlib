    DuelingBanditLib: a simple dueling bandit library 
    
              __|___|__
               ( @ @ )
               _\ _ /_               ___
              / \\_// \             /___\
             |   \_/   |        ___/_____\___
             | |  . *| |          (|||||||)
             | |  .  | |         __\_____/__
             |_|_____|_|        /...........\
             / \__8__/ \       |..|.......|..|
            )\\\     ///       |..|.......|..|
             \ |  |  |         |..|.......|../
    JRO       \|  |  |         |..|.......|./
     .   '     |  |  |   .  '  |..UUUUUUUUU_)
               |__|__|         /_\|\_/ \_/| /   .
              (__) (__)        \\\|   |   |/
                                  |   |   |  .
     .   '  .     .    `  .       |   |   |    '
                                  |   |   |  .
     .   .   '   `   .     .      |___|___|
    .        '       `       .   (___) (___)  .

- - -
1\. [About](#about)  
2\. [Environment](#environment)  
3\. [Quick run](#quick)  
4\. [Misc](#misc)  
- - -


<a name="about"></a>

## 1\. About
  This is a C++ package for dueling bandit simulations. 
  This package is designed to be
  1. Simple : easy to understand and extend, but not optimized for speed. 
  2. Independent : does not require external library.

+ Policies:
 -  RMED [1]
 -  IF [2]
 -  BTM [3]
 -  SAVAGE [4]
 -  RUCB [5]
 -  Sparring [6]
 -  MultiSBM [6]
     
<a name="environment"></a>

## 2\. Environment
  This program supports a linux/GNU C++ environment. We do not check windows/MacOSX.
  
  You need:
  - C++0x: modern C++ compiler (preferably GNU C++ (g++))
  -  waf (included) [7]: build script
  -  cmdline.h (included) [8]: command line parser
  
<a name="quick"></a>

## 3\. Quick run
  Type 
  
    ./compile
    ./build/main -r 10
    
  to run 10 test simulation runs. The result of the runs will be written in out/example1.txt

  This package also includes a simple plot tool (simpleplot.py) that is dependent on Python/Matplotlib.
  If your environment is g++/Python ready, try
  
    ./example.sh

<a name="misc"></a>

## 4\. Misc
  The ascii art is from [9].
  
## 5\. Acknowledgement
  The author thanks Huasen Wu for pointing out our misimplementation (fixed) of the Condorcet SAVAGE algortihm.

  
##References
    [1] Junpei Komiyama, Junya Honda, Hisashi Kashima, Hiroshi Nakagawa: Regret Lower Bound and Optimal Algorithm in Dueling Bandit Problem. In COLT, 2015
    [2] Yisong Yue, Josef Broder, Robert Kleinberg, and Thorsten Joachims: The k-armed dueling bandits problem. J. Comput. Syst. Sci., 78(5):1538–1556, 2012.
    [3] Yisong Yue and Thorsten Joachims. Beat the mean bandit. In ICML, pp 241–248, 2011.
    [4] Tanguy Urvoy, Fabrice Clerot, Raphael Feraud, and Sami Naamane. Generic exploration and k-armed voting bandits. In ICML, pp 91–99, 2013 
    [5] Masrour Zoghi, Shimon Whiteson, Remi Munos, and Maarten de Rijke. Relative upper confidence bound for the k-armed dueling bandit problem. In ICML, pp 10–18, 2014.
    [6] Nir Ailon, Zohar Shay Karnin, and Thorsten Joachims. Reducing dueling bandits to cardinal bandits. In ICML, pp 856–864, 2014
    [7] Waf - The meta build system: https://code.google.com/p/waf/
    [8] Hideyuki Tanaka: cmdline https://github.com/tanakh/cmdline
    [9] Duel (ascii art): http://ascii.co.uk/art/duel

##Author
  Junpei Komiyama (junpei.komiyama atmark gmail.com)

  This software is released under the MIT License, see LICENSE.txt.

  Please acknowledge its use with a citation: 
+  Junpei Komiyama, Junya Honda, Hisashi Kashima, Hiroshi Nakagawa. Regret Lower Bound and Optimal Algorithm in Dueling Bandit Problem. In Proceedings of the 28th Annual Conference on Learning Theory (COLT 2015), pages 1141-1154
, July 2015
  
