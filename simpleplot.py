#!/usr/env/python
#coding:utf-8

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rc
import matplotlib as mpl
import sys, os, copy, math, re
from matplotlib.ticker import FixedLocator,NullFormatter

plt.rcParams['text.usetex'] = True
plt.rcParams['font.family'] = 'Times New Roman Bold'
plt.rcParams['font.size'] = 20
plt.rcParams['axes.linewidth'] = 1.5 

def thin(anarray):
  i = 1.0
  retarray = []
  while(i<=len(anarray)):
    retarray.append(anarray[int(i)-1])
    i*=1.2
  if int(i) != len(anarray):
    retarray.append(anarray[-1])
  return retarray

def readFile(afile):
  lines = [line.strip() for line in file(afile, "r").readlines()]
  algs = []
  data = []
  for line in lines:
    if line[0]=="#":
      if line.find("#policy") != -1:
        line = line.replace("RMED2F", "RMED2FH")
        algs.append(line.split(" ")[2])
    else:
      sps = line.split(" ")
      data.append(sps)
  data = map(list, zip(*data)) #transpose 2d array
  #data = map(thin, data) #reduce data points
  return (algs, data)

def colors(i):
  cs = ["blue", "green", "red", "magenta", "black", ]#"cyan", ]
  return cs[i%len(cs)]
 
def linestyles(i):
  #ls = ["-", "--", "-.", ":", ".", ",", "o", "^", "v", "<", ">", "s", "+", "x", "D", "d", "h", "H", "p", "|", "_"]
  ls = ["-", "-x", "-.", "-o", "-D", "-H"]#, "x", "."]
  return ls[i%len(ls)]

def getMarkerColor(alg):
  def myRGB(xs):
    xs_scaled = map(lambda x:chr(int(x*255.0)), xs)
    astr = "#"+xs_scaled[0][-2:]+xs_scaled[1][-2:]+xs_scaled[2][-2:]
    print astr
    return astr
  print alg
  if alg == "Random": return ("->", "0.10")
  elif alg == "RUCB": return ("-+", "0.2")
  elif alg == "RMED1": return ("-o", "b")
  elif alg == "RMED2FH a=1": return ("-*", "red")
  elif alg == "RMED2FH a=3": return ("-d", "c")
  elif alg == "RMED2FH a=10": return ("-s", "plum")
  elif alg == "RMED2FH": return ("-d", "c")
  elif alg == "RMED2FH a=30": return ("-^", (0.4, 0.85, 0.85))
  elif alg == "RMED2FH a=100": return ("-s", (0.4, 0.95, 0.95))
  elif alg == "RMED2": return ("-D", "darkgreen")
  elif alg == "BTM": return ("-v", "yellow")
  elif alg == "IF": return ("-h", "red")
  elif alg == "Savage": return ("-*", "plum")
  elif alg == "Sparring": return ("-s", "orange")
  elif alg == "MultiSBM": return ("-^", "purple")
  else:
    return ("-", "1.0")

def calculateMax(data):
  m = 0
  for i in range(3, len(data)):
    alglog = data[i]
    #print alglog
    m = max(max(map(float, alglog)), m)
  return m

def plotRegret((algs, data), mode, outfilename):
  plt.xlabel("t: round", fontsize=14)
  plt.ylabel("R(t): regret", fontsize=14)
  plt.ylim(ymin = 10, ymax=calculateMax(data))
  plt.yscale('log')
  plt.xscale('log')
  print algs
  for l in range(1, len(algs)+1, 1):
    i=l-1
    m,c = getMarkerColor(algs[i])
    alg = algs[i]
    print "alg=",alg
    plt.plot(data[0], data[l+2], m, color = c, label=alg, linewidth=1)
  plt.plot(data[0], data[1], "-", color = "gold", label="LB1", linewidth=3)
  plt.plot(data[0], data[2], "--", color = "red", label="TrueLB", linewidth=3)
  plt.legend(loc="upper left", fontsize=10)
  plt.savefig(outfilename)
  plt.show()

if __name__ == '__main__':
  if len(sys.argv)>=2:
    afile = sys.argv[1]
    result = readFile(afile)
    plotRegret(result, -1, "regret.eps")
  else:
    print "usage: this filename"

