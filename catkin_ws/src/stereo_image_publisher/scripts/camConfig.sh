v4l2-ctl -d /dev/video0 -c auto_exposure=1;
v4l2-ctl -d /dev/video1 -c auto_exposure=1;

v4l2-ctl -d /dev/video0 -c exposure=13;
v4l2-ctl -d /dev/video1 -c exposure=13;

v4l2-ctl -d /dev/video0 -c gain_automatic=0;
v4l2-ctl -d /dev/video1 -c gain_automatic=0; 

v4l2-ctl -d /dev/video0 -c gain=0; 
v4l2-ctl -d /dev/video1 -c gain=0; 



