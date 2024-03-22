/**
 * @file detect_objects.cc
 * @author Grace Jin
 * @brief This file contains the main function for the tinyScrubCam project. It 
 * implements the detection of objects from the camera feed and saves the images
 * to the filesystem. The images are saved with the detected object labels. A
 * HTTP server is also implemented to serve the images and a webpage to view the
 * images. The user can also delete all images in the directory.
 * @version 3.1
 * @date 2024-03-22
 * @copyright Copyright (c) 2024
 */

// include the necessary header files
#include <cstring>
#include <vector>
#include <string>
#include <cstdio>
#include <ctime>
#include "libs/base/filesystem.h"
#include "libs/base/gpio.h"
#include "libs/base/led.h"
#include "libs/camera/camera.h"
#include "libs/rpc/rpc_http_server.h"
#include "libs/tensorflow/detection.h"
#include "libs/tensorflow/utils.h"
#include "libs/tpu/edgetpu_manager.h"
#include "libs/tpu/edgetpu_op.h"
#include "third_party/freertos_kernel/include/FreeRTOS.h"
#include "third_party/freertos_kernel/include/task.h"
#include "third_party/mjson/src/mjson.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_error_reporter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_interpreter.h"
#include "third_party/tflite-micro/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "libs/base/http_server.h"
#include "libs/libjpeg/jpeg.h"
#include "libs/base/strings.h"
#include "libs/base/utils.h"

 
// Create the label dictionary for the detected objects
// Note: A lot of labels are missing from the coco17 dataset and are replaced 
// with "No_matching_result" 
// (Check research/object_detection/data/mscoco_label_map.pbtxt)
const char* labels[] = {
    "person", "bicycle", "car", "motorcycle", "airplane",
    "bus", "train", "truck", "boat", "traffic_light",
    "fire_hydrant", "No_matching_result", "stop_sign", "parking_meter", "bench",
    "bird", "cat", "dog", "horse", "sheep", "cow", "elephant",
    "bear", "zebra", "giraffe", "No_matching_result", "backpack", "umbrella", 
    "No_matching_result", "No_matching_result", "handbag",
    "tie", "suitcase", "frisbee", "skis", "snowboard", "sports_ball",
    "kite", "baseball_glove", "skateboard", "surfboard", "tennis_racket",
    "bottle", "No_matching_result", "wine_glass", "cup", "fork", "knife", "spoon", "bowl", "banana",
    "apple", "sandwich", "orange", "broccoli", "carrot", "hot_dog", "pizza", 
    "donut", "cake", "chair", "couch", "potted_plant", "bed", "No_matching_result", "dining_table",
    "No_matching_result", "No_matching_result", "toilet", "No_matching_result", "tv", "laptop", "mouse", "remote", "keyboard", "cell_phone",
    "microwave", "oven", "toaster", "sink", "refrigerator", "No_matching_result", "book", "clock",
    "clock", "vase", "scissors", "teddy_bear", "hair_drier", "toothbrush" // Last label
};

// Use the namespace coralmicro to define the functions make API calls
namespace coralmicro {
namespace {

// Define the path to the model file
constexpr char kModelPath[] =
    "/models/tf2_ssd_mobilenet_v2_coco17_ptq_edgetpu.tflite";
// An area of memory to use for input, output, and intermediate arrays.
constexpr int kTensorArenaSize = 8 * 1024 * 1024;
STATIC_TENSOR_ARENA_IN_SDRAM(tensor_arena, kTensorArenaSize);
bool DetectFromCamera(tflite::MicroInterpreter* interpreter, int model_width,
                      int model_height,
                      std::vector<tensorflow::Object>* results,
                      std::vector<uint8>* image) {
  CHECK(results != nullptr);
  CHECK(image != nullptr);
  auto* input_tensor = interpreter->input_tensor(0);
  
  // Set the format of the camera frame
  CameraFrameFormat fmt{CameraFormat::kRgb,   CameraFilterMethod::kBilinear,
                        CameraRotation::k270, model_width,
                        model_height,         false,
                        image->data()};

  // Trigger the camera to capture a frame
  CameraTask::GetSingleton()->Trigger();
  // Get the frame from the camera
  if (!CameraTask::GetSingleton()->GetFrame({fmt})) return false;
  // Compress the frame to JPEG format
  std::memcpy(tflite::GetTensorData<uint8_t>(input_tensor), image->data(),
              image->size());
  if (interpreter->Invoke() != kTfLiteOk) return false;

  // Get the detection results
  *results = tensorflow::GetDetectionResults(interpreter, 0.6f, 1);
  return true;
}

/// @brief Function to convert a vector of image file paths to a JSON string
/// @param imageFiles A vector of image file paths
/// @return std::string A JSON string containing the image file paths
std::string ConvertToJson(const std::vector<std::string>& imageFiles) {
    std::string json = "[";

    for (size_t i = 0; i < imageFiles.size(); ++i) {
        json += "\"" + imageFiles[i] + "\"";
        if (i < imageFiles.size() - 1) {
            json += ", ";
        }
    }

    json += "]";
    return json;
}

/// @brief Function to serve the list of image files in the /dir directory
/// @return HttpServer::Content A JSON response containing the list of image files
HttpServer::Content ServeImageList() {
  std::vector<std::string> imageFiles; 
  lfs_dir_t dir;
  lfs_info info;

  printf("Opening /dir to list images...\r\n");

  // Open the directory
  if (lfs_dir_open(Lfs(), &dir, "/dir") >= 0) {
    printf("Directory /dir opened successfully.\r\n");

    // Read directory entries
    while (lfs_dir_read(Lfs(), &dir, &info) > 0) {
      // Check if the entry is a file
      if (info.type == LFS_TYPE_REG) {
        std::string fileName = info.name;
        printf("Found image file: %s\r\n", fileName.c_str());
        imageFiles.push_back(fileName);
      }
    }

    // Close the directory
    lfs_dir_close(Lfs(), &dir);
    printf("Directory /dir closed.\r\n");
  } else {
    printf("Failed to open directory /dir.\r\n");
  }

  printf("Generating JSON response...\r\n");
  std::string jsonResponse = ConvertToJson(imageFiles);
  printf("JSON Response: %s\r\n", jsonResponse.c_str());

  std::vector<uint8_t> responseData(jsonResponse.begin(), jsonResponse.end());
  // only for debugging purpose
  std::string responseStr(responseData.begin(), responseData.end());
  printf("responseData as string: %s\r\n", responseStr.c_str());
  return responseData;
}

/// @brief Function to serve an image file from the /dir directory
/// @param uri The URI of the image file
/// @return HttpServer::Content The image file data
HttpServer::Content ServeImage(const char* uri) {
  std::string filePath = uri; // convert URI to file path in /dir/
  std::vector<uint8_t> imageData;
  if (LfsReadFile(filePath.c_str(), &imageData)){
    return imageData;
  }
  return {};
}
/// @brief Function to delete all images in the /dir directory
/// @return HttpServer::Content A JSON response indicating the status of the deletion
HttpServer::Content DeleteAllImagesInDir() {
  lfs_dir_t dir;
  lfs_info info;

  printf("Opening /dir to delete images... \r\n");

  // JSON response messages to indicate success or failure
  std::string success_response = "{\"status\":\"success\", \"message\":\"All images have been successfully deleted.\"}";
  std::string fail_response = "{\"status\":\"error\", \"message\":\"Images failed to be successfully deleted.\"}";

  // Open the directory
  if (lfs_dir_open(Lfs(), &dir, "/dir") >= 0) {
    printf("Directory /dir opened successfully! \r\n");
    // Read directory entries
    while (lfs_dir_read(Lfs(), &dir, &info) > 0) {
      // Check if the entry is a file and delete it
      if (info.type == LFS_TYPE_REG) {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "/dir/%s", info.name);
        if (lfs_remove(Lfs(), filePath) == LFS_ERR_OK) {
          printf("Deleted file: %s\r\n", filePath);
        } else {
          printf("Failed to delete file %s\r\n", filePath);
          std::vector<uint8_t> fail_responseData(fail_response.begin(), fail_response.end());
          return fail_responseData;
        }
      }
    }
    // Close the directory
    lfs_dir_close(Lfs(), &dir);
    printf("Directory /dir closed after deletion process. \r\n");
    std::vector<uint8_t> success_responseData(success_response.begin(), success_response.end());
    return success_responseData;
  } else { // Failed to open directory
    printf("Failed to open directory /dir\r\n");
    std::vector<uint8_t> fail_responseData(fail_response.begin(), fail_response.end());
    return fail_responseData;
  }
}

constexpr char kIndexFileName[] = "/Image_view.html";
constexpr char kCameraStreamUrlPrefix[] = "/dir/image.jpg";
/// @brief Function to handle HTTP requests for different URIs
/// @param uri The URI of the HTTP request
/// @return HttpServer::Content The response data for the HTTP request
HttpServer::Content UriHandler(const char* uri) {
  printf("Received HTTP request for URI: %s\r\n", uri);
  if (StrEndsWith(uri, "index.shtml") || StrEndsWith(uri, "Image_view.html")) {
    printf("Requesting the index page....\r\n");
    // Serve the HTML page for displaying the saved image
    return std::string(kIndexFileName);
  } else if (StrEndsWith(uri, "/image-list")) {
    return ServeImageList();
  } else if (StrStartsWith(uri, "/dir/")){
    return ServeImage(uri);
  } else if (StrEndsWith(uri, "/delete-all-images")) {
    return DeleteAllImagesInDir();
  }else {
    printf("URI not recognitzed.\r\n");
  }
  return {};
}


/// @brief Function to print the contents of a directory
/// @param dir	Pointer to the directory object
/// @param path The path of the directory
/// @param num_tabs The number of tabs to print before the file or directory name
void PrintDirectory(lfs_dir_t* dir, const char* path, int num_tabs) {
  constexpr int kMaxDepth = 3;
  if (num_tabs > kMaxDepth) {
    return;
  }
  
  // Read the directory entries
  lfs_info info; 
  while (lfs_dir_read(Lfs(), dir, &info) > 0) {
    if (info.name[0] == '.') {// Skip hidden files
      continue;
    }
    for (int i = 0; i < num_tabs; ++i) {// Print tabs for indentation
      printf("\t");
    }

    printf("%s", info.name);	

	// Check if the entry is a directory
    if (info.type == LFS_TYPE_DIR) {
      char subpath[LFS_NAME_MAX];
      printf("/\r\n");
      lfs_dir_t subdir;	
      snprintf(subpath, LFS_NAME_MAX, "%s/%s", path, info.name);
      CHECK(lfs_dir_open(Lfs(), &subdir, subpath) >= 0);
      PrintDirectory(&subdir, subpath, num_tabs + 1);
      CHECK(lfs_dir_close(Lfs(), &subdir) >= 0);
    } else {
      printf("\t\t%ld\r\n", info.size);
    }
  }
}


/// @brief Function to print the contents of the filesystem
void PrintFilesystemContents() {
  lfs_dir_t root;
  CHECK(lfs_dir_open(Lfs(), &root, "/") >= 0);
  printf("Printing filesystem:\r\n");
  PrintDirectory(&root, "", 0);
  printf("Finished printing filesystem.\r\n");
  CHECK(lfs_dir_close(Lfs(), &root) >= 0);
}

/// @brief Function to make a directory in the filesystem
/// @param path The path of the directory to create
/// @return bool True if the directory was created successfully, false otherwise
bool Mkdir(const char* path) {
  int ret = lfs_mkdir(Lfs(), path);
  if (ret == LFS_ERR_EXIST) {
    printf("Error dir exists");
    return false;
  }
  return (ret == LFS_ERR_OK);
}


/// @brief Function to write data to a file in the filesystem
/// @param path The path of the file to write to
/// @param data The data to write to the file
bool WriteToFile(const char* path, const uint8_t* data, size_t size) {
  lfs_file_t file;
  // Open the file with write access. Create the file if it doesn't exist
  if (lfs_file_open(Lfs(), &file, path, LFS_O_WRONLY | LFS_O_CREAT) < 0) {
    // Handle error in opening file
    return false;
  }
  // Write data to the file
if (static_cast<size_t>(lfs_file_write(Lfs(), &file, data, size)) != size) {
    // Handle error in writing data
    lfs_file_close(Lfs(), &file);
    return false;
  }
  // Close the file
  if (lfs_file_close(Lfs(), &file) < 0) {
    // Handle error in closing file
    return false;
  }

  return true;
}


/// @brief Function to capture a frame from the camera and compress it to JPEG 
/// format
/// @return std::vector<uint8_t> The JPEG data of the captured frame
std::vector<uint8_t> CaptureFrameJPEG () {
  std::vector<uint8_t> buf(CameraTask::kWidth * CameraTask::kHeight *
                             CameraFormatBpp(CameraFormat::kRgb));
  auto fmt = CameraFrameFormat{
    CameraFormat::kRgb,       CameraFilterMethod::kBilinear,
    CameraRotation::k270,       CameraTask::kWidth,
    CameraTask::kHeight,
    /*preserve_ratio=*/false, buf.data(),
    /*while_balance=*/true
  };

  // Get a frame from the camera
  CameraTask::GetSingleton()-> Trigger();
  if (!CameraTask::GetSingleton()->GetFrame({fmt})) {
    printf("Unable to get frame from camera\r\n");
    return {};
  }
  // Compress the frame to JPEG format
  std::vector<uint8_t> jpeg;
  JpegCompressRgb(buf.data(), fmt.width, fmt.height, /*quality=*/75, &jpeg);
  // [end-snippet:jpeg]
  return jpeg;
}

/// @brief  Function to get the next available image index for a given base
/// @param baseFilename The base filename for the images
/// @return int The next available image index
int GetNextImageIndex(const std::string& baseFilename) {
    lfs_dir_t dir;
    lfs_info info;
    int maxIndex = 0;

    char pattern[100];
    std::snprintf(pattern, sizeof(pattern), "%s_%%d.jpg", baseFilename.c_str());

    if (lfs_dir_open(Lfs(), &dir, "/dir") >= 0) {
        while (lfs_dir_read(Lfs(), &dir, &info) > 0) {
            if (info.type == LFS_TYPE_REG) {
                // Extract the index from the filename and update maxIndex
                int index;
                if (sscanf(info.name, pattern, &index) == 1) {
                    if (index > maxIndex) {
                        maxIndex = index;
                    }
                }
            }
        }
        lfs_dir_close(Lfs(), &dir);
    }
    return maxIndex + 1;  // Return the next available index
}


/// @brief Function to record an image to the filesystem
/// @param baseFilename The base filename for the images
/// @param image The image data to save
/// @return bool True if the image was saved successfully, false otherwise
bool Record(const std::string& baseFilename, const std::vector<uint8>& image) {
  lfs_info fileInfo;

  // Get the next available image index for naming the image file
  int index = GetNextImageIndex(baseFilename);
  char filePath[100]; 
  std::snprintf(filePath, sizeof(filePath), "/dir/%s_%d.jpg", baseFilename.c_str(), index);

  // Check if the image file already exists
  if (lfs_stat(Lfs(), filePath, &fileInfo) < 0) {
    printf("Image file does not exist. Capturing and saving a new image.\r\n");
    std::vector<uint8_t> jpegData = image;
    if (jpegData.empty()) {
      printf("Failed to capture an image.\r\n");
      return false;
    }
    // Save the image to the filesystem
    if (!WriteToFile(filePath, jpegData.data(), jpegData.size())) {
      printf("Failed to save the image.\r\n");
      return false;
    }
    printf("Image saved successfully!\r\n");
  } else {
    printf("Image file already exists. Skipping image capture.\r\n");
  }
  PrintFilesystemContents();
  return true;
}


/// @brief Function to establish a server to detect objects from the camera feed
/// @param interpreter The TensorFlow Lite interpreter
void DetectConsole(tflite::MicroInterpreter* interpreter) {
  printf("DetectConsole runs!\r\n");
  // Get the input tensor
  auto* input_tensor = interpreter->input_tensor(0);
  int model_height = input_tensor->dims->data[1];
  int model_width = input_tensor->dims->data[2];
  std::vector<uint8> image(model_height * model_width *
                           CameraFormatBpp(CameraFormat::kRgb));
  std::vector<tensorflow::Object> results;
  // Detect objects from the camera feed
  if (DetectFromCamera(interpreter, model_width, model_height, &results,
                       &image)) {
    std::string namePrediction = "";
    for (const auto& object : results) {
      int id = object.id; // Accessing the ID of the detected object
      float score = object.score; // Accessing the score
      // Accessing the bounding box coordinates
      float xmin = object.bbox.xmin;
      float xmax = object.bbox.xmax;
      float ymin = object.bbox.ymin;
      float ymax = object.bbox.ymax;
      // Now you can use the id, score, and bounding box as needed
      // For example, printing them:
      printf("Detected object ID: %d, Label: %s, Score: %f, BBox: [%f, %f, %f, %f]\r\n",
            id, labels[id], score, xmin, ymin, xmax, ymax);
      
      if (!namePrediction.empty()) {
        namePrediction += "_";
      }
      namePrediction += labels[id];
    }
  
    // If no objects are detected, print a message
    if (results.size() == 0) {
      printf("No result detected!\r\n");
    } else{ // If objects are detected, record the image
      Record(namePrediction, &image);
	  // Turn on the speaker for 10 seconds
      coralmicro::GpioSet(coralmicro::Gpio::kAA, true);
      vTaskDelay(pdMS_TO_TICKS(10000));
      coralmicro::GpioSet(coralmicro::Gpio::kAA, false);
    }
  } else {
    printf("Failed to detect image from camera.\r\n");
  }
}

[[noreturn]] void Main() {
  printf("Detection Camera Example!\r\n");
  // Turn on Status LED to show the board is on.
  LedSet(Led::kStatus, true);

  // Load the model
  std::vector<uint8_t> model;
  if (!LfsReadFile(kModelPath, &model)) {
    printf("ERROR: Failed to load %s\r\n", kModelPath);
    vTaskSuspend(nullptr);
  }

  // Initialize the Edge TPU
  auto tpu_context = EdgeTpuManager::GetSingleton()->OpenDevice();
  if (!tpu_context) {
    printf("ERROR: Failed to get EdgeTpu context\r\n");
    vTaskSuspend(nullptr);
  }
  // Register custom op
  tflite::MicroErrorReporter error_reporter;
  tflite::MicroMutableOpResolver<3> resolver;
  resolver.AddDequantize();
  resolver.AddDetectionPostprocess();
  resolver.AddCustom(kCustomOp, RegisterCustomOp());

  // Create an interpreter
  tflite::MicroInterpreter interpreter(tflite::GetModel(model.data()), resolver,
                                       tensor_arena, kTensorArenaSize,
                                       &error_reporter);
  if (interpreter.AllocateTensors() != kTfLiteOk) {
    printf("ERROR: AllocateTensors() failed\r\n");
    vTaskSuspend(nullptr);
  }
  if (interpreter.inputs().size() != 1) {
    printf("ERROR: Model must have only one input tensor\r\n");
    vTaskSuspend(nullptr);
  }

  // Starting Camera.
  CameraTask::GetSingleton()->SetPower(true);
  CameraTask::GetSingleton()->Enable(CameraMode::kTrigger);

  // create the /dir directory for images
  printf("Checking if '/dir' directory exists. \r\n");
  if (!LfsDirExists("/dir")) {
    printf("'/dir' directory doesn't exist. Creating directory. \r\n");
    if (!Mkdir("/dir")) {
      printf("Failed to create '/dir' directory.\r\n");
    } else {
      printf("'/dir' directory created successfully.\r\n");
    }
  }

  // host on the specific usb_ip
  std::string usb_ip;
  if (GetUsbIpAddress(&usb_ip)) {
    printf("Serving on http://%s\r\n", usb_ip.c_str());
  }

  // Start an HTTP server on USB
  HttpServer http_server;
  http_server.AddUriHandler(UriHandler);
  UseHttpServer(&http_server);

  // Initialize kAA (pin A1) as output to the amplifier
  GpioSetMode(coralmicro::Gpio::kAA, coralmicro::GpioMode::kOutput);
  coralmicro::GpioSet(coralmicro::Gpio::kAA, false);

  // Initialize kScl6 (pin D0) as input
  GpioSetMode(Gpio::kScl6, GpioMode::kInput);

  // Configure interrupt for kScl6 to trigger on rising edge
  GpioConfigureInterrupt(
    Gpio::kScl6, GpioInterruptMode::kIntModeRising,
    [handle = xTaskGetCurrentTaskHandle()]() {xTaskResumeFromISR(handle);});

  while (true) {
    vTaskSuspend(nullptr);
    DetectConsole(&interpreter);
  }
}

}  // namespace
}  // namespace coralmicro
extern "C" void app_main(void* param) {
  (void)param;
  coralmicro::Main();
}