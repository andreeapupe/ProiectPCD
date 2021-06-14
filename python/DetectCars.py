import time
import cv2
import numpy as np
import sys
import client

'''f = open("ProcessingTime.txt", "w")
f.close()'''
final_data = "INFO_INCOMING\n"
data_cardetect = 0


class LaneDetectionClass:
    def hls2rgb(self, img):
        return cv2.cvtColor(img, cv2.COLOR_HLS2RGB)

    def rgb2bgr(self, img):
        return cv2.cvtColor(img, cv2.COLOR_RGB2BGR)

    def bgr2rgb(self, img):
        return cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

    def rgb2hls(self, img):
        return cv2.cvtColor(img, cv2.COLOR_RGB2HLS)

    def grayscale(self, img):
        return cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)


    def compute_white_yellow(self, hls_img):
        img_hls_yellow_bin = np.zeros_like(hls_img[:, :, 0])
        img_hls_yellow_bin[((hls_img[:, :, 0] >= 15) & (hls_img[:, :, 0] <= 35))
                           & ((hls_img[:, :, 1] >= 30) & (hls_img[:, :, 1] <= 204))
                           & ((hls_img[:, :, 2] >= 115) & (hls_img[:, :, 2] <= 255))
                           ] = 1

        img_hls_white_bin = np.zeros_like(hls_img[:, :, 0])
        img_hls_white_bin[((hls_img[:, :, 0] >= 0) & (hls_img[:, :, 0] <= 255))
                          & ((hls_img[:, :, 1] >= 200) & (hls_img[:, :, 1] <= 255))
                          & ((hls_img[:, :, 2] >= 0) & (hls_img[:, :, 2] <= 255))
                          ] = 1


        img_hls_white_yellow_bin = np.zeros_like(hls_img[:, :, 0])
        img_hls_white_yellow_bin[(img_hls_yellow_bin == 1) | (img_hls_white_bin == 1)] = 1
        return img_hls_white_yellow_bin


    def blur(self, bin_img):

        kernel_size = 11

        return cv2.GaussianBlur(bin_img, (kernel_size, kernel_size), 0)

    def onlyLanes(self, input_img):

        rows, cols = input_img.shape[:2]

        bottom_left = [cols * 0.1, rows * 0.95]
        top_left = [cols * 0.4, rows * 0.55]
        bottom_right = [cols * 0.9, rows * 0.95]
        top_right = [cols * 0.6, rows * 0.55]
        vertices = np.array([[bottom_left, top_left, top_right, bottom_right]], dtype=np.int32)

        mask = np.zeros_like(input_img)
        cv2.fillPoly(mask, vertices, 255)

        input_img = cv2.bitwise_and(input_img, mask)

        return input_img

    def hough_lines(self, img):
        return cv2.HoughLinesP(img, rho=1, theta=np.pi / 180, threshold=20, minLineLength=50, maxLineGap=100)

    def draw_circles(self, image, lines):

        if not lines is None:
            for line in lines:
                for x1, y1, x2, y2 in line:
                    x3, y3 = int(0.5 * (x1 + x2)), int(0.5 * (y1 + y2))

                    slope = (y2 - y1) / (x2 - x1)

                    if slope >= 0:
                        cv2.circle(image, center=(x1, y1), radius=5, color=(255, 0, 0), thickness=-1)
                        cv2.circle(image, center=(x2, y2), radius=5, color=(255, 0, 0), thickness=-1)
                        cv2.circle(image, center=(x3, y3), radius=5, color=(255, 0, 0), thickness=-1)

                    else:
                        cv2.circle(image, center=(x1, y1), radius=5, color=(0, 0, 255), thickness=-1)
                        cv2.circle(image, center=(x2, y2), radius=5, color=(0, 0, 255), thickness=-1)
                        cv2.circle(image, center=(x3, y3), radius=5, color=(0, 0, 255), thickness=-1)

        return image

    def detect_cars(self, image):
        # Haar Cascade
        car_cascade = cv2.CascadeClassifier('cars.xml')

        gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

        cars = car_cascade.detectMultiScale(gray, 1.3, 2, minSize=(70, 70), maxSize=(200, 200))

        for (x, y, w, h) in cars:
            cv2.rectangle(image, (x, y), (x + w, y + h), (0, 0, 255), 2)

        return image

    def process(self, image):

        # Converting image from RGB to HLS
        img_hls = self.rgb2hls(np.uint8(image))

        # Extracting only white and yellow components
        white_yellow = self.compute_white_yellow(img_hls)

        # Applying region of interest mask
        regions = self.onlyLanes(white_yellow)

        # Remove noise from the image and make it smoother
        blurred = self.blur(regions)

        # Find lines by applying Hough Transform
        lines = self.hough_lines(blurred)

        # Draw circles on detected lanes
        temp = self.draw_circles(image, lines)

        # Detect cars from the image
        output = self.detect_cars(temp)

        return output


def process_video(video_input):
    inpt = video_input + '.mp4'
    otpt = video_input + '_output.avi'

    # Create an object of lane Detector class
    detector = LaneDetectionClass()

    cap = cv2.VideoCapture(inpt)

    # Frame dimensions
    frame_width = int(cap.get(3))
    frame_height = int(cap.get(4))

    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    out = cv2.VideoWriter(otpt, fourcc, 20, (frame_width, frame_height))

    # Count number of frames
    frames_count = 0

    while cap.isOpened():

        # Read the current frame from input video
        ret, image = cap.read()

        if ret:
            if frames_count%60 == 0:
                # start measuring time from now
                start = time.time()

                # If it is the first frame of video, do:
                if frames_count == 0:
                    # Start the ultimate timer for measuring processing time for entire video
                    etrm_strt = start

                img = detector.bgr2rgb(image)

                output_image = detector.process(img)

                end = time.time()

                # Write the processed image to output video file
                out.write(detector.rgb2bgr(output_image))

                # Print the time taken to process frames with 1s delay and save it to txt file
                '''f = open("ProcessingTime.txt", "a")

                print(end - start)
                f.write(repr(end-start) + '\n')
                f.close()'''
                data_cardetect = end-start
                print(data_cardetect)
                global final_data
                final_data += str(data_cardetect)
                final_data += "\n"



        else:
            etrm_end = time.time()

            print('Average Time : ' + str((etrm_end - etrm_strt) / frames_count) + 's')

            break

        frames_count += 1
    cap.release()
    out.release()

    return frames_count


if __name__ == '__main__':
    start = time.time()

    frames_count = process_video(sys.argv[1])
    end = time.time()
    client.run(final_data)
    print(final_data)

    print("Total : " + str((end - start)) + "s for " + str(frames_count) + "frames")
