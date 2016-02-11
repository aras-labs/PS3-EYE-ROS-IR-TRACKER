
(cl:in-package :asdf)

(defsystem "ir_tracker_node_3D-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :geometry_msgs-msg
               :std_msgs-msg
)
  :components ((:file "_package")
    (:file "irtracker" :depends-on ("_package_irtracker"))
    (:file "_package_irtracker" :depends-on ("_package"))
  ))