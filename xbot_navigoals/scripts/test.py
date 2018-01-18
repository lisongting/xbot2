#!/usr/bin/env python
#coding=utf-8
import yaml
a= {'wangpeng':[0,1],'lisongting':[1,2]}
f = open('pose.yaml','w')
yaml.dump(a,f)
f.close()