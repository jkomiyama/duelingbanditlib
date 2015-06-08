#!/usr/bin/env python
# coding:utf-8

import sys

def readSushiAttributeFile(filename):
  lines = [l.strip() for l in file(filename, "r").readlines()]
  itemAttrs = {}
  for line in lines:
    elems = line.split("\t")
    item_id = elems[0]
    itemAttrs[item_id] = elems[1:]
  return itemAttrs

def readSushiOrderingFile(filename):
  lines = [l.strip() for l in file(filename, "r").readlines()]
  items = {}     # num of A
  itemXitems ={} # num of A>B
  for line in lines[1:]:
    items_l = line.split(" ")[2:]
    for item in items_l:
      if not items.has_key(item):
        items[item]=0
      items[item]+=1
    for i in range(len(items_l)):
      for j in range(i+1, len(items_l)):
        apair=(items_l[i], items_l[j])
        if not itemXitems.has_key(apair):
          itemXitems[apair] = 0
        itemXitems[apair]+=1
  for ii in items.keys():
    for ij in items.keys():
      if ii==ij:continue
      apair = (ii, ij)
      if not itemXitems.has_key(apair):
        itemXitems[apair]=0
  return items, itemXitems


#item x item matrix
def main(orderFile, attrFile):
  (items, itemXitems) = readSushiOrderingFile(orderFile)
  itemAttrs = readSushiAttributeFile(attrFile)
  item_list = list(items)
  item_list_name = map(lambda i:itemAttrs[i][0], item_list)
  item_list_name_f = map(lambda i:i[0], item_list_name)
  #print '\\ '+ " ".join(item_list_name)
  #print '\\ '+ " ".join(item_list)
  def getRatio(i1, i2):
    if i1==i2:
      return 0.5
    r1, r2 = itemXitems[(i1,i2)], itemXitems[(i2,i1)]
    if int(r1)+int(r2)==0:
      return 0.5
    return float(r1)/(float(r1)+float(r2))
  def getCompNum(i1, i2):
    if i1==i2:
      return 0.5
    r1, r2 = itemXitems[(i1,i2)], itemXitems[(i2,i1)]
    return int(r1)+int(r2)
  def itemAllCompNum(i1, item_list):
    n = 0
    for i2 in item_list: 
      if i1 != i2:
        n += getCompNum(i1, i2)
    return n
  cns = sorted([(itemAllCompNum(ii, item_list), itemAttrs[ii][0], ii) for ii in item_list])
  cns = cns[-16:] 
  items_sub = map(lambda x:x[2], cns)
  for i1 in items_sub:
    if all([i1==i2 or getRatio(i1, i2)>0.5 for i2 in items_sub]):
      pass#print i1,itemAttrs[i1][0],"is the Condorcet winner."
  for i1 in items_sub:
    print " ".join(["{:.3f}".format(getRatio(i1, i2)) for i2 in items_sub])
  sys.exit() 
  
 
#  print item_list, len(item_list),max(item_list);sys.exit() #note that item_list is not sorted
  print "#"+" ".join([itemAttrs[i][0] for i in item_list]);sys.exit()

  for i in range(len(item_list)):
    astr = ""
    astr += item_list[i]
    rs = []
    for j in range(len(item_list)):
      ii, ij = item_list[i], item_list[j]
      astr += " %.2f"
      rs.append( getRatio(ii, ij) )
    print astr % tuple(rs)
  sys.exit(0)
main("sushi3/sushi3b.5000.10.order", "sushi3/sushi3.idata")

