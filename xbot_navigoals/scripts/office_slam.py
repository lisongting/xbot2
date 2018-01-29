#!/usr/bin/env python
#coding=utf-8

import rospy, yaml, os

from xbot_msgs.msg import FaceResult
from std_msgs.msg import String, UInt32
from geometry_msgs.msg import Pose, PoseStamped
from actionlib_msgs.msg import GoalStatusArray
from move_base_msgs.msg import MoveBaseActionResult

class office_slam():
	"""docstring for office_slam"""
	def __init__(self):
		self.next_loop_pub = rospy.Publisher('/office/next_loop', UInt32, queue_size=1)
		self.goal_reached_pub = rospy.Publisher('/office/goal_reached', String, queue_size=1)
		self.move_base_goal_pub = rospy.Publisher('/move_base_simple/goal', PoseStamped, queue_size=1)
		self.move_base_result_sub = rospy.Subscriber('/move_base/result', MoveBaseActionResult, move_base_resultCB)
		self.goal_name_sub = rospy.Subscriber('/office/goal_name', String, self.goal_nameCB)
		self.coll_position_dict = dict()
		self.current_goal = []
		self.loop_times = UInt32()
		self.loop_times.data = 0
		f = open('/home/roc/ros_kinetic_ws/src/xbot2/xbot_navigoals/scripts/coll_position_dic.yaml', 'r')
		self.coll_position_dict = yaml.load(f)
		f.close()
		rospy.spin()

	def goal_nameCB(self, name):
		pos = self.coll_position_dict[name.data]
		goal = PoseStamped()
		goal.pose.position = pos[0]
		goal.pose.orientation = pos[1]
		self.move_base_goal_pub.publish(goal)
		self.current_goal = [name, goal]


	def move_base_resultCB(self, result):
		if result.status.status == 3:
			#success
			self.goal_reached_pub(self.current_goal[0])
			if self.current_goal[0].data == 'origin':
				self.loop_times.data += 1
				self.next_loop_pub.publish(self.loop_times)
		elif result.status.status == 4:
			#failed
			msg = String()
			msg.data = 'abort'
			self.goal_reached_pub(msg)








if __name__ == '__main__':
	rospy.init_node('office_slam_serve')
	try:
		rospy.loginfo('office slam initialized...')
		office_slam()
	except rospy.ROSInterruptException:
		rospy.loginfo('office slam initialize failed, please retry...')