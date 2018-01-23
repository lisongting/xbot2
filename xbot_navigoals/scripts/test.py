#!/usr/bin/env python
#coding=utf-8
import yaml
f = open('coll_position_dic.yaml','r')
dict=yaml.load(f)
f.close()
print dict