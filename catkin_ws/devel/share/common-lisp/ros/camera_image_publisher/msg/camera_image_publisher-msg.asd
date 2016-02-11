
(cl:in-package :asdf)

(defsystem "camera_image_publisher-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :sensor_msgs-msg
               :std_msgs-msg
)
  :components ((:file "_package")
    (:file "mono_camera" :depends-on ("_package_mono_camera"))
    (:file "_package_mono_camera" :depends-on ("_package"))
  ))