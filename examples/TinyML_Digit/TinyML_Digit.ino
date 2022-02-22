#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
#include "digits_model.h"
#include "x_test.h"

#define N_INPUTS  8*8
#define N_OUTPUTS 10
// preallocate a certain amount of memory for input, output, and intermediate arrays.
#define TENSOR_ARENA_SIZE 8*1024 

Eloquent::TinyML::TensorFlow::TensorFlow<N_INPUTS, N_OUTPUTS, TENSOR_ARENA_SIZE> tf;

void setup() {
    Serial.begin(115200);
    tf.begin(digits_model);
}

void loop() {
    int i;
    float y_pred[10] ={0};

    uint32_t start = micros(); 
     
    tf.predict(x_test_dat, y_pred);

    uint32_t timeit = micros() - start;

    Serial.print("It took ");
    Serial.print(timeit);
    Serial.println(" micros to run inference");

    for (i=0; i<10;i++) {
      Serial.print(y_pred[i]);
      Serial.print(i == 9 ? '\n' : ',');
    }
    
    Serial.print("Predicted Class :");
    Serial.println(tf.probaToClass(y_pred));
    Serial.print("Sanity check:");
    Serial.println(tf.predictClass(x_test_dat));
}
