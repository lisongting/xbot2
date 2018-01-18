#!/usr/bin/env python
#coding=utf-8

import rospy, sys, termios, tty
import yaml

from geometry_msgs.msg import Pose, PoseStamped
from visualization_msgs.msg import Marker, MarkerArray

class office_lady():
	"""docstring for office_lady"""
	def __init__(self):
		self.total_coll = input('please input total number of colleagues:\n')
		self.num_coll = 0
		self.coll_position_dic = {}
		self.marker=Marker()
		self.marker.color.r=1.0
		self.marker.color.g=0.0
		self.marker.color.b=0.0
		self.marker.color.a=1.0
		self.marker.ns='office_lady'
		self.marker.scale.x=1
		self.marker.scale.y=0.1
		self.marker.scale.z=0.1
		self.marker.header.frame_id='map'
		self.marker.type=Marker.SPHERE_LIST
		self.marker.action=Marker.ADD
		self.markers = MarkerArray()
		self.markers_pub = rospy.Publisher('/coll_position',MarkerArray,queue_size=1)
		self.goal_sub = rospy.Subscriber('/mark_coll_position',PoseStamped, self.mark_coll_positionCB)
		# f = open('param/col_pos.yaml')
		# self.col_poses = yaml.load(f)
		# if col_poses is not NULL:
		# 	#push col_poses.pos to markerarray
		# else:
		# 	print 'please input the pose of every colleague with 2DNavi goal...'
		# 	print 'the first colleague is '+ col_poses.pose[0]
		
		tip = 'please input NO '+ num_coll + 'colleague name:\n'
		self.name = input(tip)
		tip = 'please input the pose of NO' + num_coll +' with 2DNavi goal tool in rviz'
		print tip

	def mark_coll_positionCB(self, pos):
		if self.num_coll < self.total_coll:
			self.coll_position_dic[self.name] = pos.pose
			self.num_coll++
			print 'added '+num_coll+' colleagues'
			print coll_position_dic
			self.marker.header.stamp =rospy.Time.now()
			self.marker.pose = pos.pose
			self.marker.id = num_coll - 1
			self.markers.pushback(self.marker)
			markers_pub.publish(self.markers)
			tip = 'please input NO '+ num_coll + 'colleague name:\n'
			self.name = input(tip)
			tip = 'please input the pose of NO' + num_coll +' with 2DNavi goal tool in rviz'
			print tip
		else:
			f=open('coll_position_dic.yaml', 'w')
			yaml.dump(self.coll_position_dic, f)
			f.close()
			sys.exit()






if __name__ == '__main__':
	rospy.init_node('office_lady_serve')
	try:
		rospy.loginfo('office lady initialized...')
		office_lady()
	except rospy.ROSInterruptException:
		rospy.loginfo('office lady initialize failed, please retry...')