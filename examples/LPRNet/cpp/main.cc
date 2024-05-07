// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*-------------------------------------------
                Includes
-------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lprnet.h"
#include "image_utils.h"
#include "file_utils.h"
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

/*-------------------------------------------
                  Main Function
-------------------------------------------*/
int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("%s <model_path>\n", argv[0]);
        return -1;
    }

    const char* model_path = argv[1];


    int ret;
    rknn_app_context_t rknn_app_ctx;
    memset(&rknn_app_ctx, 0, sizeof(rknn_app_context_t));

    ret = init_lprnet_model(model_path, &rknn_app_ctx);
    if (ret != 0) {
        printf("init_lprnet_model fail! ret=%d model_path=%s\n", ret, model_path);
        return -1;
    }

    // conectémonos a un stream RTSP mediante OpenCV
    cv::VideoCapture cap("rtsp://camaras:Melosilla123.@192.168.1.108:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif");
    if (!cap.isOpened()) {
        std::cout << "Error al abrir el stream RTSP" << std::endl;
        return -1;
    }

    int i = 0;
    lprnet_result result;
    result.plate_name = "";
    bool hecho = false;
    while (hecho == false) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) {
            std::cout << "Error al capturar el frame" << std::endl;
            break;
        }
        std::cout << "frame " << i << std::endl;
        ret = inference_lprnet_model_mat(&rknn_app_ctx, &frame, &result);
        if (ret != 0) {
            printf("inference_lprnet_model fail! ret=%d\n", ret);
            goto out;
        }
        i++;
    }

    // image_buffer_t src_image;
    // while (i < 10) {
    //     memset(&src_image, 0, sizeof(image_buffer_t));
    //     ret = read_image(image_path, &src_image);
    //     if (ret != 0) {
    //         printf("read image fail! ret=%d image_path=%s\n", ret, image_path);
    //         return -1;
    //     }

    //     ret = inference_lprnet_model(&rknn_app_ctx, &src_image, &result);
    //     if (ret != 0) {
    //         printf("inference_lprnet_model fail! ret=%d\n", ret);
    //         goto out;
    //     }

    //     std::cout << "resultado: " << result.plate_name << std::endl;
    //     i++;
    // }

out:
    ret = release_lprnet_model(&rknn_app_ctx);
    if (ret != 0) {
        printf("release_lprnet_model fail! ret=%d\n", ret);
    }

    // if (src_image.virt_addr != NULL) {
    //     free(src_image.virt_addr);
    // }

    return 0;
}
