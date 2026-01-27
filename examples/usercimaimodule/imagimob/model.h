/*
* Imagimob Studio 5.0.1820+4321c8be86ece6ffb339e47f960600a022788a61
* Copyright © 2023- Imagimob AB, All Rights Reserved.
* 
* Generated at 08/20/2024 05:44:33 UTC. Any changes will be lost.
* 
* Model ID  ad879df4-9fed-4ba0-90d8-f6c78a5462e3
* 
* Memory    Size                      Efficiency
* Buffers   8000 bytes (RAM)          80 %
* State     808 bytes (RAM)           100 %
* Readonly  94744 bytes (Flash)       100 %
* 
* Backend              tensorflow
* Keras Version        2.15.0
* Backend Model Type   Sequential
* Backend Model Name   conv1d-medium-balanced-3
* 
* Class Index | Symbol Label
* 0           | unlabelled
* 1           | standing
* 2           | running
* 3           | walking
* 4           | sitting
* 5           | jumping
* 
* Layer                          Shape           Type       Function
* Sliding Window (data points)   [50,3]          float      dequeue
*    window_shape = [50,3]
*    stride = 30
*    buffer_multiplier = 2
* Input Layer                    [50,3]          float      dequeue
*    shape = [50,3]
* Convolution 1D                 [25,16]         float      dequeue
*    filters = 16
*    kernel_size = 3
*    dilation_rate = 1
*    strides = 2
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,3,16]
* Batch Normalization            [25,16]         float      dequeue
*    epsilon = 0.001
*    trainable = True
*    scale = True
*    center = True
*    axis = 2
*    gamma = float[16]
*    beta = float[16]
*    mean = float[16]
*    variance = float[16]
* Activation                     [25,16]         float      dequeue
*    activation = relu
*    trainable = True
* Convolution 1D                 [25,32]         float      dequeue
*    filters = 32
*    kernel_size = 3
*    dilation_rate = 1
*    strides = 1
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,16,32]
* Convolution 1D                 [25,32]         float      dequeue
*    filters = 32
*    kernel_size = 3
*    dilation_rate = 1
*    strides = 1
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,32,32]
* Batch Normalization            [25,32]         float      dequeue
*    epsilon = 0.001
*    trainable = True
*    scale = True
*    center = True
*    axis = 2
*    gamma = float[32]
*    beta = float[32]
*    mean = float[32]
*    variance = float[32]
* Activation                     [25,32]         float      dequeue
*    activation = relu
*    trainable = True
* Max pooling 1D                 [12,32]         float      dequeue
*    pool_size = 2
*    strides = 2
*    padding = valid
*    trainable = True
* Convolution 1D                 [12,64]         float      dequeue
*    filters = 64
*    kernel_size = 3
*    dilation_rate = 1
*    strides = 1
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,32,64]
* Convolution 1D                 [12,64]         float      dequeue
*    filters = 64
*    kernel_size = 3
*    dilation_rate = 1
*    strides = 1
*    padding = same
*    activation = linear
*    use_bias = False
*    trainable = True
*    weight = float[3,64,64]
* Batch Normalization            [12,64]         float      dequeue
*    epsilon = 0.001
*    trainable = True
*    scale = True
*    center = True
*    axis = 2
*    gamma = float[64]
*    beta = float[64]
*    mean = float[64]
*    variance = float[64]
* Activation                     [12,64]         float      dequeue
*    activation = relu
*    trainable = True
* Max pooling 1D                 [6,64]          float      dequeue
*    pool_size = 2
*    strides = 2
*    padding = valid
*    trainable = True
* Global average pooling 1D      [64]            float      dequeue
*    trainable = True
* Dense                          [6]             float      dequeue
*    units = 6
*    use_bias = True
*    activation = linear
*    trainable = True
*    weight = float[64,6]
*    bias = float[6]
* Activation                     [6]             float      dequeue
*    activation = softmax
*    trainable = True
* 
* Exported functions:
* 
* int IMAI_dequeue(float *restrict data_out)
*    Description: Dequeue features. RET_SUCCESS (0) on success, RET_NODATA (-1) if no data is available, RET_NOMEM (-2) on internal memory error
*    Parameter data_out is Output of size float[6].
* 
* int IMAI_enqueue(const float *restrict data_in)
*    Description: Enqueue features. Returns SUCCESS (0) on success, else RET_NOMEM (-2) when low on memory.
*    Parameter data_in is Input of size float[3].
* 
* void IMAI_init(void)
*    Description: Initializes buffers to initial state. This function also works as a reset function.
* 
* 
* Disclaimer:
*   The generated code relies on the optimizations done by the C compiler.
*   For example many for-loops of length 1 must be removed by the optimizer.
*   This can only be done if the functions are inlined and simplified.
*   Check disassembly if unsure.
*   tl;dr Compile using gcc with -O3 or -Ofast
*/

#ifndef _IMAI_MODEL_H_
#define _IMAI_MODEL_H_
#ifdef _MSC_VER
#pragma once
#endif

#include <stdint.h>

typedef struct {    
    char *name;
    double TP; // True Positive or Correct Positive Prediction
    double FN; // False Negative or Incorrect Negative Prediction
    double FP; // False Positive or Incorrect Positive Prediction
    double TN; // True Negative or Correct Negative Prediction
    double TPR; // True Positive Rate or Sensitivity, Recall
    double TNR; // True Negative Rate or Specificity, Selectivity
    double PPV; // Positive Predictive Value or Precision
    double NPV; // Negative Predictive Value
    double FNR; // False Negative Rate or Miss Rate
    double FPR; // False Positive Rate or Fall-Out
    double FDR; // False Discovery Rate
    double FOR; // False Omission Rate
    double F1S; // F1 Score
} IMAI_stats;

/*
* Tensorflow Test Set
* 
* (ACC) Accuracy 87.442 %
* (F1S) F1 Score 87.305 %
* 
* Name of class                                            (unlabelled)          jumping          running          sitting         standing          walking
* (TP) True Positive or Correct Positive Prediction                 117              351              813              970             1757             1305
* (FN) False Negative or Incorrect Negative Prediction              135                0              102              271               49              206
* (FP) False Positive or Incorrect Positive Prediction               24               66                2                0              539              132
* (TN) True Negative or Correct Negative Prediction                5800             5659             5159             4835             3731             4433
* (TPR) True Positive Rate or Sensitivity, Recall               46.43 %         100.00 %          88.85 %          78.16 %          97.29 %          86.37 %
* (TNR) True Negative Rate or Specificity, Selectivity          99.59 %          98.85 %          99.96 %         100.00 %          87.38 %          97.11 %
* (PPV) Positive Predictive Value or Precision                  82.98 %          84.17 %          99.75 %         100.00 %          76.52 %          90.81 %
* (NPV) Negative Predictive Value                               97.73 %         100.00 %          98.06 %          94.69 %          98.70 %          95.56 %
* (FNR) False Negative Rate or Miss Rate                        53.57 %           0.00 %          11.15 %          21.84 %           2.71 %          13.63 %
* (FPR) False Positive Rate or Fall-Out                          0.41 %           1.15 %           0.04 %           0.00 %          12.62 %           2.89 %
* (FDR) False Discovery Rate                                    17.02 %          15.83 %           0.25 %           0.00 %          23.48 %           9.19 %
* (FOR) False Omission Rate                                      2.27 %           0.00 %           1.94 %           5.31 %           1.30 %           4.44 %
* (F1S) F1 Score                                                59.54 %          91.41 %          93.99 %          87.74 %          85.67 %          88.53 %
*/


#define IMAI_TEST_AVG_ACC 0.8744239631336406 // Accuracy
#define IMAI_TEST_AVG_F1S 0.8730489003654032 // F1 Score

#define IMAI_TEST_STATS { \
 {name: "(unlabelled)", TP: 117, FN: 135, FP: 24, TN: 5800, TPR: 0.4642857142857, TNR: 0.9958791208791, PPV: 0.8297872340425, NPV: 0.9772535804549, FNR: 0.5357142857142, FPR: 0.0041208791208, FDR: 0.1702127659574, FOR: 0.0227464195450, F1S: 0.5954198473282, }, \
 {name: "standing", TP: 1757, FN: 49, FP: 539, TN: 3731, TPR: 0.9728682170542, TNR: 0.8737704918032, PPV: 0.7652439024390, NPV: 0.9870370370370, FNR: 0.0271317829457, FPR: 0.1262295081967, FDR: 0.2347560975609, FOR: 0.0129629629629, F1S: 0.8566552901023, }, \
 {name: "running", TP: 813, FN: 102, FP: 2, TN: 5159, TPR: 0.8885245901639, TNR: 0.9996124782018, PPV: 0.9975460122699, NPV: 0.9806120509408, FNR: 0.1114754098360, FPR: 0.0003875217981, FDR: 0.0024539877300, FOR: 0.0193879490591, F1S: 0.9398843930635, }, \
 {name: "walking", TP: 1305, FN: 206, FP: 132, TN: 4433, TPR: 0.8636664460622, TNR: 0.9710843373493, PPV: 0.9081419624217, NPV: 0.9555938779909, FNR: 0.1363335539377, FPR: 0.0289156626506, FDR: 0.0918580375782, FOR: 0.0444061220090, F1S: 0.8853459972862, }, \
 {name: "sitting", TP: 970, FN: 271, FP: 0, TN: 4835, TPR: 0.7816277195809, TNR: 1, PPV: 1, NPV: 0.9469251860556, FNR: 0.2183722804190, FPR: 0, FDR: 0, FOR: 0.0530748139443, F1S: 0.8774310266847, }, \
 {name: "jumping", TP: 351, FN: 0, FP: 66, TN: 5659, TPR: 1, TNR: 0.9884716157205, PPV: 0.8417266187050, NPV: 1, FNR: 0, FPR: 0.0115283842794, FDR: 0.1582733812949, FOR: 0, F1S: 0.9140625, }, \
}

#ifdef IMAI_STATS_ENABLED
static const IMAI_stats IMAI_test_stats[] = IMAI_TEST_STATS;
#endif

/*
* Tensorflow Train Set
* 
* (ACC) Accuracy 97.441 %
* (F1S) F1 Score 97.278 %
* 
* Name of class                                            (unlabelled)          jumping          running          sitting         standing          walking
* (TP) True Positive or Correct Positive Prediction                 477             3470             2643             4773             4614             4092
* (FN) False Negative or Incorrect Negative Prediction              318               33               21               19               89               47
* (FP) False Positive or Incorrect Positive Prediction               60               70               53               15              155              174
* (TN) True Negative or Correct Negative Prediction               19741            17023            17879            15789            15738            16283
* (TPR) True Positive Rate or Sensitivity, Recall               60.00 %          99.06 %          99.21 %          99.60 %          98.11 %          98.86 %
* (TNR) True Negative Rate or Specificity, Selectivity          99.70 %          99.59 %          99.70 %          99.91 %          99.02 %          98.94 %
* (PPV) Positive Predictive Value or Precision                  88.83 %          98.02 %          98.03 %          99.69 %          96.75 %          95.92 %
* (NPV) Negative Predictive Value                               98.41 %          99.81 %          99.88 %          99.88 %          99.44 %          99.71 %
* (FNR) False Negative Rate or Miss Rate                        40.00 %           0.94 %           0.79 %           0.40 %           1.89 %           1.14 %
* (FPR) False Positive Rate or Fall-Out                          0.30 %           0.41 %           0.30 %           0.09 %           0.98 %           1.06 %
* (FDR) False Discovery Rate                                    11.17 %           1.98 %           1.97 %           0.31 %           3.25 %           4.08 %
* (FOR) False Omission Rate                                      1.59 %           0.19 %           0.12 %           0.12 %           0.56 %           0.29 %
* (F1S) F1 Score                                                71.62 %          98.54 %          98.62 %          99.65 %          97.42 %          97.37 %
*/


#define IMAI_TRAIN_AVG_ACC 0.9744125072829676 // Accuracy
#define IMAI_TRAIN_AVG_F1S 0.9727809277039343 // F1 Score

#define IMAI_TRAIN_STATS { \
 {name: "(unlabelled)", TP: 477, FN: 318, FP: 60, TN: 19741, TPR: 0.6, TNR: 0.9969698500075, PPV: 0.8882681564245, NPV: 0.9841467670372, FNR: 0.4, FPR: 0.0030301499924, FDR: 0.1117318435754, FOR: 0.0158532329627, F1S: 0.7162162162162, }, \
 {name: "standing", TP: 4614, FN: 89, FP: 155, TN: 15738, TPR: 0.9810759089942, TNR: 0.9902472786761, PPV: 0.9674984273432, NPV: 0.9943766980476, FNR: 0.0189240910057, FPR: 0.0097527213238, FDR: 0.0325015726567, FOR: 0.0056233019523, F1S: 0.9742398648648, }, \
 {name: "running", TP: 2643, FN: 21, FP: 53, TN: 17879, TPR: 0.9921171171171, TNR: 0.9970443899174, PPV: 0.9803412462908, NPV: 0.9988268156424, FNR: 0.0078828828828, FPR: 0.0029556100825, FDR: 0.0196587537091, FOR: 0.0011731843575, F1S: 0.9861940298507, }, \
 {name: "walking", TP: 4092, FN: 47, FP: 174, TN: 16283, TPR: 0.9886446001449, TNR: 0.9894269915537, PPV: 0.9592123769338, NPV: 0.9971218616044, FNR: 0.0113553998550, FPR: 0.0105730084462, FDR: 0.0407876230661, FOR: 0.0028781383955, F1S: 0.9737061273051, }, \
 {name: "sitting", TP: 4773, FN: 19, FP: 15, TN: 15789, TPR: 0.9960350584307, TNR: 0.9990508731966, PPV: 0.9968671679197, NPV: 0.9987980769230, FNR: 0.0039649415692, FPR: 0.0009491268033, FDR: 0.0031328320802, FOR: 0.0012019230769, F1S: 0.9964509394572, }, \
 {name: "jumping", TP: 3470, FN: 33, FP: 70, TN: 17023, TPR: 0.9905795032829, TNR: 0.9959047563330, PPV: 0.9802259887005, NPV: 0.9980651969981, FNR: 0.0094204967170, FPR: 0.0040952436669, FDR: 0.0197740112994, FOR: 0.0019348030018, F1S: 0.9853755501916, }, \
}

#ifdef IMAI_STATS_ENABLED
static const IMAI_stats IMAI_train_stats[] = IMAI_TRAIN_STATS;
#endif

/*
* Tensorflow Validation Set
* 
* (ACC) Accuracy 98.266 %
* (F1S) F1 Score 98.225 %
* 
* Name of class                                            (unlabelled)          jumping          running          sitting         standing          walking
* (TP) True Positive or Correct Positive Prediction                 165              232             1210             1564              521             1296
* (FN) False Negative or Incorrect Negative Prediction               53                1                4                0               13               17
* (FP) False Positive or Incorrect Positive Prediction               14               35               11               11                4               13
* (TN) True Negative or Correct Negative Prediction                4844             4808             3851             3501             4538             3750
* (TPR) True Positive Rate or Sensitivity, Recall               75.69 %          99.57 %          99.67 %         100.00 %          97.57 %          98.71 %
* (TNR) True Negative Rate or Specificity, Selectivity          99.71 %          99.28 %          99.72 %          99.69 %          99.91 %          99.65 %
* (PPV) Positive Predictive Value or Precision                  92.18 %          86.89 %          99.10 %          99.30 %          99.24 %          99.01 %
* (NPV) Negative Predictive Value                               98.92 %          99.98 %          99.90 %         100.00 %          99.71 %          99.55 %
* (FNR) False Negative Rate or Miss Rate                        24.31 %           0.43 %           0.33 %           0.00 %           2.43 %           1.29 %
* (FPR) False Positive Rate or Fall-Out                          0.29 %           0.72 %           0.28 %           0.31 %           0.09 %           0.35 %
* (FDR) False Discovery Rate                                     7.82 %          13.11 %           0.90 %           0.70 %           0.76 %           0.99 %
* (FOR) False Omission Rate                                      1.08 %           0.02 %           0.10 %           0.00 %           0.29 %           0.45 %
* (F1S) F1 Score                                                83.12 %          92.80 %          99.38 %          99.65 %          98.39 %          98.86 %
*/


#define IMAI_VALIDATION_AVG_ACC 0.9826635145784082 // Accuracy
#define IMAI_VALIDATION_AVG_F1S 0.9822456227311898 // F1 Score

#define IMAI_VALIDATION_STATS { \
 {name: "(unlabelled)", TP: 165, FN: 53, FP: 14, TN: 4844, TPR: 0.7568807339449, TNR: 0.9971181556195, PPV: 0.9217877094972, NPV: 0.9891770471717, FNR: 0.2431192660550, FPR: 0.0028818443804, FDR: 0.0782122905027, FOR: 0.0108229528282, F1S: 0.8312342569269, }, \
 {name: "standing", TP: 521, FN: 13, FP: 4, TN: 4538, TPR: 0.9756554307116, TNR: 0.9991193306913, PPV: 0.9923809523809, NPV: 0.9971434849483, FNR: 0.0243445692883, FPR: 0.0008806693086, FDR: 0.0076190476190, FOR: 0.0028565150516, F1S: 0.9839471199244, }, \
 {name: "running", TP: 1210, FN: 4, FP: 11, TN: 3851, TPR: 0.9967051070840, TNR: 0.9971517348524, PPV: 0.9909909909909, NPV: 0.9989623865110, FNR: 0.0032948929159, FPR: 0.0028482651475, FDR: 0.0090090090090, FOR: 0.0010376134889, F1S: 0.9938398357289, }, \
 {name: "walking", TP: 1296, FN: 17, FP: 13, TN: 3750, TPR: 0.9870525514089, TNR: 0.9965453095934, PPV: 0.9900687547746, NPV: 0.9954871250331, FNR: 0.0129474485910, FPR: 0.0034546904065, FDR: 0.0099312452253, FOR: 0.0045128749668, F1S: 0.9885583524027, }, \
 {name: "sitting", TP: 1564, FN: 0, FP: 11, TN: 3501, TPR: 1, TNR: 0.9968678815489, PPV: 0.9930158730158, NPV: 1, FNR: 0, FPR: 0.0031321184510, FDR: 0.0069841269841, FOR: 0, F1S: 0.9964956992672, }, \
 {name: "jumping", TP: 232, FN: 1, FP: 35, TN: 4808, TPR: 0.9957081545064, TNR: 0.9927730745405, PPV: 0.8689138576779, NPV: 0.9997920565606, FNR: 0.0042918454935, FPR: 0.0072269254594, FDR: 0.1310861423220, FOR: 0.0002079434393, F1S: 0.928, }, \
}

#ifdef IMAI_STATS_ENABLED
static const IMAI_stats IMAI_validation_stats[] = IMAI_VALIDATION_STATS;
#endif

#define IMAI_API_QUEUE

// All symbols in order
#define IMAI_SYMBOL_MAP {"unlabelled", "standing", "running", "walking", "sitting", "jumping"}

// Model GUID (16 bytes)
#define IMAI_MODEL_ID {0xf4, 0x9d, 0x87, 0xad, 0xed, 0x9f, 0xa0, 0x4b, 0x90, 0xd8, 0xf6, 0xc7, 0x8a, 0x54, 0x62, 0xe3}

// First nibble is bit encoding, second nibble is number of bytes
#define IMAGINET_TYPES_NONE	(0x0)
#define IMAGINET_TYPES_FLOAT32	(0x14)
#define IMAGINET_TYPES_FLOAT64	(0x18)
#define IMAGINET_TYPES_INT8	(0x21)
#define IMAGINET_TYPES_INT16	(0x22)
#define IMAGINET_TYPES_INT32	(0x24)
#define IMAGINET_TYPES_INT64	(0x28)
#define IMAGINET_TYPES_QDYN8	(0x31)
#define IMAGINET_TYPES_QDYN16	(0x32)
#define IMAGINET_TYPES_QDYN32	(0x34)

// data_in [3] (12 bytes)
#define IMAI_DATA_IN_COUNT (3)
#define IMAI_DATA_IN_TYPE float
#define IMAI_DATA_IN_TYPE_ID IMAGINET_TYPES_FLOAT32
#define IMAI_DATA_IN_SCALE (1)
#define IMAI_DATA_IN_OFFSET (0)
#define IMAI_DATA_IN_IS_QUANTIZED (0)

// data_out [6] (24 bytes)
#define IMAI_DATA_OUT_COUNT (6)
#define IMAI_DATA_OUT_TYPE float
#define IMAI_DATA_OUT_TYPE_ID IMAGINET_TYPES_FLOAT32
#define IMAI_DATA_OUT_SCALE (1)
#define IMAI_DATA_OUT_OFFSET (0)
#define IMAI_DATA_OUT_IS_QUANTIZED (0)

#define IMAI_KEY_MAX (39)



// Return codes
#define IMAI_RET_SUCCESS 0
#define IMAI_RET_NODATA -1
#define IMAI_RET_NOMEM -2

// Exported methods
int IMAI_dequeue(float *restrict data_out);
int IMAI_enqueue(const float *restrict data_in);
void IMAI_init(void);

#endif /* _IMAI_MODEL_H_ */