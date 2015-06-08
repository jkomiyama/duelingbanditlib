#!/usr/bin/env python
# coding:utf-8

#a tool for merging multiple simulation results

import sys,os,re

def avg(elems):
  return sum(elems)/float(len(elems))

def splitavg(splits):
  l = len(splits[0])
  for sp in splits:
    if len(sp) != l:
      print "split size not match"
      sys.exit()
  sums = [0 for i in range(l)]
  for sp in splits:
    for i in range(l):
      sums[i] += float(sp[i])
  return map(lambda i:i/len(splits), sums)

def gather(filenames):
  lines_files = []
  for afile in filenames:
    lines_files.append([line.strip() for line in file(afile, "r").readlines() if len(line)>0])
  l = 0
  for i in range(len(lines_files)-1):
    if len(lines_files[i]) != len(lines_files[i+1]):
      print "line num does not match!"
      sys.exit()
  while l < len(lines_files[0]):
    if len(lines_files[0][l])==0:
      pass
    elif lines_files[0][l][0]=="#":
      print lines_files[0][l]
    else:
      splits = [lines_files[i][l].split(" ") for i in range(len(lines_files))]
      avgs = splitavg(splits)
      avgs[0] = int(avgs[0])
      print " ".join(map(str, avgs))
    l+=1

if __name__ == "__main__":
  gather(sys.argv[1:])
