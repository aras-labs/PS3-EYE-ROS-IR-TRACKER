
(cl:in-package :asdf)

(defsystem "stereo_image_publisher-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils :sensor_msgs-msg
               :std_msgs-msg
)
  :components ((:file "_package")
    (:file "stereo_camera" :depends-on ("_package_stereo_camera"))
    (:file "_package_stereo_camera" :depends-on ("_package"))
  ))