#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
#include "mnist_cnn.h"
#include "x_test.h"

#define N_INPUTS  28*28
#define N_OUTPUTS 10
// preallocate a certain amount of memory for input, output, and intermediate arrays.
#define TENSOR_ARENA_SIZE 64*1024 

Eloquent::TinyML::TensorFlow::TensorFlow<N_INPUTS, N_OUTPUTS, TENSOR_ARENA_SIZE> tf;

void setup() {
    Serial.begin(115200);
    tf.begin(mnist_cnn);
}

void loop() {
    int i;
    float y_pred[10] ={0};
    float x_test[N_INPUTS];

    for (i=0; i<N_INPUTS; i++){
      x_test[i]= x_test_dat[i];   // read x_test data (uint8)
      x_test[i]= x_test[i]/255.0; // normalize x_test 
    }

    uint32_t start = micros(); 
     
    tf.predict(x_test, y_pred);

    uint32_t timeit = micros() - start;

    Serial.print("It took ");
    Serial.print(timeit);
    Serial.println(" micros to run inference");

    for (int i=0; i<10;i++) {
      Serial.print(y_pred[i]);
      Serial.print(i == 9 ? '\n' : ',');
    }
    
    Serial.print("Predicted Class :");
    Serial.println(tf.probaToClass(y_pred));
    Serial.print("Sanity check:");
    Serial.println(tf.predictClass(x_test));
}
