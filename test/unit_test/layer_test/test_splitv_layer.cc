// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "test/unit_test/layer_test/layer_test.h"
#include "test/unit_test/unit_test_common.h"
#include "test/unit_test/utils/network_helpers.h"
#include "tnn/utils/dims_vector_utils.h"

namespace TNN_NS {

class SplitVLayerTest : public LayerTest,
                        public ::testing::WithParamInterface<std::tuple<int, int, int, int, int, DataType>> {};

INSTANTIATE_TEST_SUITE_P(LayerTest, SplitVLayerTest,
                         ::testing::Combine(BASIC_BATCH_CHANNEL_SIZE,
                                            // split axis
                                            testing::Values(1, 2, 3),
                                            // output cnt
                                            testing::Values(2, 3, 4, 6),
                                            // dtype
                                            testing::Values(DATA_TYPE_FLOAT)));

TEST_P(SplitVLayerTest, SplitVLayer) {
    // get param
    int batch          = std::get<0>(GetParam());
    int channel        = std::get<1>(GetParam());
    int input_size     = std::get<2>(GetParam());
    int axis           = std::get<3>(GetParam());
    int output_count   = std::get<4>(GetParam());
    DataType data_type = std::get<5>(GetParam());
    DeviceType dev     = ConvertDeviceType(FLAGS_dt);

    if (data_type == DATA_TYPE_INT8 && DEVICE_ARM != dev) {
        GTEST_SKIP();
    }

    std::vector<int> shapes = {batch, channel, input_size, input_size};
    int dim_size_to_split = shapes[axis];
    if (dim_size_to_split < output_count) {
        GTEST_SKIP();
    }

    // blob desc
    auto inputs_desc  = CreateInputBlobsDesc(batch, channel, input_size, 1, data_type);
    auto outputs_desc = CreateOutputBlobsDesc(output_count, data_type);

    // param
    SplitVLayerParam param;
    param.name   = "SplitV";
    param.axis   = axis;
    if (dim_size_to_split % output_count != 0) {
        // set slices for unbalanced cases
        int split_size = dim_size_to_split / output_count;
        int last_size  = dim_size_to_split - split_size * (output_count - 1);
        param.slices = std::vector<int>(output_count, split_size);
        param.slices[output_count-1] = last_size;
    }

    Run(LAYER_SPLITV, &param, nullptr, inputs_desc, outputs_desc);
}

}  // namespace TNN_NS
