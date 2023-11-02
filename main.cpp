#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

class GalleryArt {
public:
    GalleryArt() : cap(0), hoveredImage(-1) {
        initializeGallery();
    }

    void run() {
        while (true) {
            cv::Mat frame;
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "Error: Could not read frame." << std::endl;
                break;
            }

            // Increase the frame size
            cv::Mat largerFrame(frame.rows * 2, frame.cols * 2, frame.type());
            cv::resize(frame, largerFrame, largerFrame.size());

            // Perform yellow badge detection based on color
            cv::Mat yellowMask = detectYellowBadge(largerFrame);

            // Find the centroid of the detected yellow badge
            cv::Point yellowBadgePosition = findCentroid(yellowMask);

            // Check which gallery image is being hovered over
            hoveredImage = getHoveredImage(yellowBadgePosition.x, yellowBadgePosition.y, largerFrame.cols, largerFrame.rows);

            // Display gallery images in the center
            displayGallery(largerFrame, 100, 100); // Set your desired image width and height

            // Draw a square around the detected yellow badge
            int badgeSize = 30; // Adjust the size as needed
            cv::rectangle(largerFrame, yellowBadgePosition - cv::Point(badgeSize, badgeSize), yellowBadgePosition + cv::Point(badgeSize, badgeSize), cv::Scalar(0, 0, 255), 2);

            // Display the frame with object detection results
            cv::imshow("Gallery Art", largerFrame);

            // Exit the loop when the 'q' key is pressed
            if (cv::waitKey(1) == 'q') {
                break;
            }
        }

        cap.release();
        cv::destroyAllWindows();
    }

private:
    cv::VideoCapture cap;
    std::vector<cv::Mat> galleryImages;
    int hoveredImage;

    void initializeGallery() {
        // Load your gallery images
        for (int i = 1; i <= 4; ++i) {
            std::string imageFile = "image" + std::to_string(i) + ".jpg";
            cv::Mat image = cv::imread(imageFile);
            if (!image.empty()) {
                galleryImages.push_back(image);
            }
        }
    }

    cv::Mat detectYellowBadge(const cv::Mat& frame) {
        cv::Mat hsvImage;
        cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

        // Define the lower and upper bounds for yellow color
        cv::Scalar lowerYellow = cv::Scalar(20, 100, 100); // Adjust lower bound for hue and saturation
        cv::Scalar upperYellow = cv::Scalar(40, 255, 255); // Keep upper bound for hue and saturation

        // Create a binary mask for yellow color
        cv::Mat yellowMask;
        cv::inRange(hsvImage, lowerYellow, upperYellow, yellowMask);

        return yellowMask;
    }

    cv::Point findCentroid(const cv::Mat& mask) {
        cv::Moments moments = cv::moments(mask, true);
        cv::Point centroid(moments.m10 / moments.m00, moments.m01 / moments.m00);
        return centroid;
    }

    int getHoveredImage(int x, int y, int frameWidth, int frameHeight) {
        for (int i = 0; i < galleryImages.size(); ++i) {
            int imageWidth = galleryImages[i].cols;
            int imageHeight = galleryImages[i].rows;
            int imageX = (frameWidth / 2) - (imageWidth / 2) + i * 150;
            int imageY = (frameHeight / 2) - (imageHeight / 2) + 50;

            if (x >= imageX && x < imageX + imageWidth && y >= imageY && y < imageY + imageHeight) {
                return i;
            }
        }

        return -1;
    }

    void displayGallery(cv::Mat& frame, int imageWidth, int imageHeight) {
        for (int i = 0; i < galleryImages.size(); ++i) {
            cv::Mat resizedImage;
            cv::resize(galleryImages[i], resizedImage, cv::Size(imageWidth, imageHeight));

            int imageX = (frame.cols / 2) - (imageWidth / 2) + i * 150;
            int imageY = (frame.rows / 2) - (imageHeight / 2) + 50;

            // Ensure that the destination roi is within the bounds of largerFrame
            cv::Rect roi(std::max(0, imageX), std::max(0, imageY), imageWidth, imageHeight);
            cv::Rect srcRect(0, 0, imageWidth, imageHeight);

            if (roi.x + roi.width > frame.cols) {
                roi.width = frame.cols - roi.x;
                srcRect.width = roi.width;
            }

            if (roi.y + roi.height > frame.rows) {
                roi.height = frame.rows - roi.y;
                srcRect.height = roi.height;
            }

            if (hoveredImage == i) {
                // Make the hovered image larger
                cv::resize(resizedImage, resizedImage, cv::Size(imageWidth * 1.5, imageHeight * 1.5));
            }

            if (roi.width > 0 && roi.height > 0) {
                cv::Mat galleryImage = resizedImage(srcRect);
                galleryImage.copyTo(frame(roi));
            }
        }
    }
};

int main() {
    GalleryArt galleryArt;
    galleryArt.run();

    return 0;
}
