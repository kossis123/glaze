import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2 as cv


class ContourDetector(Node):
    def __init__(self):
        super().__init__("koshish")
        self.bridge = CvBridge()
        self.s_ = self.create_subscription(
            Image,
            "/camera/rgb",
            self.callback,
            10
        )

    def callback(self, msg):
        frame = self.bridge.imgmsg_to_cv2(msg, "bgr8")
        hsv=cv.cvtColor(frame,cv.COLOR_BGR2HSV)


        lower_blue = (100, 100, 50)
        upper_blue = (130, 255, 255)
        
        mask1=cv.inRange(hsv,lower_blue,upper_blue)
        masked=cv.bitwise_and(frame,frame,mask=mask1)
        cv.imshow("windows",masked)
        cv.imshow("mask",mask1)
        
        lower_red1 = (0, 50, 50)
        upper_red1 = (15, 255, 255)

        lower_red2 = (165, 50, 50)
        upper_red2 = (180, 255, 255)

        mask2=cv.inRange(hsv,lower_red1,upper_red1)
        mask3=cv.inRange(hsv,lower_red2,upper_red2)
        final_mask=cv.bitwise_or(mask2,mask3)
        masked_red=cv.bitwise_and(frame,frame,mask=final_mask)
        cv.imshow("red_mask",masked_red)

        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        blur = cv.GaussianBlur(gray, (5, 5), 0)
        edges = cv.Canny(blur, 50, 150)

        contours, _ = cv.findContours(
            edges,
            cv.RETR_EXTERNAL,
            cv.CHAIN_APPROX_SIMPLE
        )

        for contour in contours:
            area = cv.contourArea(contour)

            # if area < 1000:
            #     continue

            cv.drawContours(frame, [contour], -1, (0, 255, 0), 2)

            x, y, w, h = cv.boundingRect(contour)
            cv.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)

            M = cv.moments(contour)

            if M["m00"] != 0:
                cx = int(M["m10"] / M["m00"])
                cy = int(M["m01"] / M["m00"])
                cv.circle(frame, (cx, cy), 5, (0, 0, 255), -1)

            cv.putText(
                frame,
                f"Area: {int(area)}",
                (x, y - 10),
                cv.FONT_HERSHEY_SIMPLEX,
                0.6,
                (0, 255, 255),
                2
            )
   

        cv.imshow("Original", frame)
        cv.imshow("Edges", edges)

        if cv.waitKey(1) & 0xFF == ord('q'):
            cv.destroyAllWindows()
            rclpy.shutdown()


def main(args=None):
    rclpy.init(args=args)
    node1 = ContourDetector()
    try:
        rclpy.spin(node1)
    except KeyboardInterrupt:
        pass
    finally:
        node1.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == "__main__":
    main()