import rclpy
from rclpy.node import Node
from cv_bridge import CvBridge
from sensor_msgs.msg import Image
import cv2 as cv
from sensor_msgs.msg import CameraInfo

class node(Node):

    def __init__(self):
        self.cy=0
        self.cx=0
        self.depth_result=0

        super().__init__("koshish")
        self.bridge=CvBridge()
        self.sub=self.create_subscription(Image,
                                          "/camera/rgb",
                                          self.callback,
                                          10
        )
        self.depth_sub=self.create_subscription(Image,
                                            "/camera",
                                            self.depth_callback,
                                            10)
        self.info=self.create_subscription(CameraInfo,
                                           "/camera/camera_info",
                                           self.info_callback,
                                           10)
        
    def callback(self,msg):

        frame=self.bridge.imgmsg_to_cv2(msg,"bgr8")
        cv.imshow("window",frame)
        hsv=cv.cvtColor(frame,cv.COLOR_BGR2HSV)
        lower_red1 = (0, 50, 50)
        upper_red1 = (15, 255, 255)

        lower_red2 = (165, 50, 50)
        upper_red2 = (180, 255, 255)

        lower_blue = (100, 100, 50)
        upper_blue = (130, 255, 255)

        mask=cv.inRange(hsv,lower_blue,upper_blue)
        final=cv.bitwise_and(frame,frame,mask=mask)
        mask1=cv.inRange(hsv,lower_red1,upper_red1)
        mask2=cv.inRange(hsv,lower_red2,upper_red2)
        red_mask=cv.bitwise_or(mask1,mask2)
        
        final_mask=cv.bitwise_and(frame,frame,mask=red_mask)

        grey=cv.cvtColor(final_mask,cv.COLOR_BGR2GRAY)
        blur=cv.GaussianBlur(grey,(5,5),0)
        canny=cv.Canny(blur,100,200)

        contours,_=cv.findContours(canny,
                                  cv.RETR_EXTERNAL,
                                  cv.CHAIN_APPROX_SIMPLE
                                )
        for contour in contours:

            x, y, w, h=cv.boundingRect(contour)
            cv.rectangle(frame,
                        (x,y),
                        (x+w,y+h),
                        (0,255,0),
                        1)
            # M=cv.moments(contour)
            # if(M["m00"]!=0):
            #     cx= int(M["m10"]/M["m00"])
            #     cy= int(M["m01"]/M["m00"])

            #     cv.circle(frame,
            #           (cx,cy),
            #           2,
            #           (255,0,0),
            #           1)
            self.cx=int(x+w/2)
            self.cy=int(y+h/2)
            cv.circle(frame,
                    (int(x+w/2), int(y+h/2)),
                    2,
                    (255,0,0),
                    -1
                    )
        cv.imshow("detected",frame)
        if cv.waitKey(1) & 0xFF == ord('q'):
            cv.destroyAllWindows()
            rclpy.shutdown()
    
    def depth_callback(self,msg):

        self.depth_value=self.bridge.imgmsg_to_cv2(msg,desired_encoding="passthrough")
        self.depth_result=self.depth_value[self.cy,self.cx]
        if(self.depth_result==0):
            return
        else:
            self.get_logger().info(str(self.depth_result))

    def info_callback(self,msg):
        self.fx=msg.k[0]
        self.cx_info=msg.k[2]
        self.fy=msg.k[4]
        self.cy_info=msg.k[5]
        
        self.x=((self.cx-self.cx_info) * self.depth_result)/self.fx
        self.y=((self.cy-self.cy_info) * self.depth_result)/self.fy 

        self.get_logger().info(str(f"{self.x} {self.y}"))

def main(args=None):
    rclpy.init(args=args)
    node1=node()
    rclpy.spin(node1)
    rclpy.shutdown()

if __name__=="__main__":
    main()