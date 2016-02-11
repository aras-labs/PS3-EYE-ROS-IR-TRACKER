; Auto-generated. Do not edit!


(cl:in-package ir_tracker_node_3D-msg)


;//! \htmlinclude irtracker.msg.html

(cl:defclass <irtracker> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (pose
    :reader pose
    :initarg :pose
    :type geometry_msgs-msg:Pose
    :initform (cl:make-instance 'geometry_msgs-msg:Pose)))
)

(cl:defclass irtracker (<irtracker>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <irtracker>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'irtracker)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name ir_tracker_node_3D-msg:<irtracker> is deprecated: use ir_tracker_node_3D-msg:irtracker instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <irtracker>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ir_tracker_node_3D-msg:header-val is deprecated.  Use ir_tracker_node_3D-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'pose-val :lambda-list '(m))
(cl:defmethod pose-val ((m <irtracker>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader ir_tracker_node_3D-msg:pose-val is deprecated.  Use ir_tracker_node_3D-msg:pose instead.")
  (pose m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <irtracker>) ostream)
  "Serializes a message object of type '<irtracker>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'pose) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <irtracker>) istream)
  "Deserializes a message object of type '<irtracker>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'pose) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<irtracker>)))
  "Returns string type for a message object of type '<irtracker>"
  "ir_tracker_node_3D/irtracker")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'irtracker)))
  "Returns string type for a message object of type 'irtracker"
  "ir_tracker_node_3D/irtracker")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<irtracker>)))
  "Returns md5sum for a message object of type '<irtracker>"
  "d3812c3cbc69362b77dc0b19b345f8f5")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'irtracker)))
  "Returns md5sum for a message object of type 'irtracker"
  "d3812c3cbc69362b77dc0b19b345f8f5")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<irtracker>)))
  "Returns full string definition for message of type '<irtracker>"
  (cl:format cl:nil "Header header~%geometry_msgs/Pose pose~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: geometry_msgs/Pose~%# A representation of pose in free space, composed of position and orientation. ~%Point position~%Quaternion orientation~%~%================================================================================~%MSG: geometry_msgs/Point~%# This contains the position of a point in free space~%float64 x~%float64 y~%float64 z~%~%================================================================================~%MSG: geometry_msgs/Quaternion~%# This represents an orientation in free space in quaternion form.~%~%float64 x~%float64 y~%float64 z~%float64 w~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'irtracker)))
  "Returns full string definition for message of type 'irtracker"
  (cl:format cl:nil "Header header~%geometry_msgs/Pose pose~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%# 0: no frame~%# 1: global frame~%string frame_id~%~%================================================================================~%MSG: geometry_msgs/Pose~%# A representation of pose in free space, composed of position and orientation. ~%Point position~%Quaternion orientation~%~%================================================================================~%MSG: geometry_msgs/Point~%# This contains the position of a point in free space~%float64 x~%float64 y~%float64 z~%~%================================================================================~%MSG: geometry_msgs/Quaternion~%# This represents an orientation in free space in quaternion form.~%~%float64 x~%float64 y~%float64 z~%float64 w~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <irtracker>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'pose))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <irtracker>))
  "Converts a ROS message object to a list"
  (cl:list 'irtracker
    (cl:cons ':header (header msg))
    (cl:cons ':pose (pose msg))
))
