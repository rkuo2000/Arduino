#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
#include "tinyml_airdigit.h"
#include "x_test.h"

#define N_INPUTS  90
#define N_OUTPUTS 10
// preallocate a certain amount of memory for input, output, and intermediate arrays.
#define TENSOR_ARENA_SIZE 16*1024 

  
Eloquent::TinyML::TensorFlow::TensorFlow<N_INPUTS, N_OUTPUTS, TENSOR_ARENA_SIZE> tf;
    
void setup() {  
    Serial.begin(115200);
    tf.begin(tinyml_airdigit);
    Serial.println(sizeof(tinyml_airdigit));
}

void loop() { 
    float y_pred[10] ={0};

    uint32_t lastTime = micros(); 
    
    tf.predict(x_test_dat, y_pred);

    uint32_t inferTime = micros() - lastTime;

    Serial.print("The inference took ");
    Serial.print(inferTime);
    Serial.println(" us");

    for (int i=0; i<10;i++) {
      Serial.print(y_pred[i]);
      Serial.print(i==9 ? '\n' : ',');
    }
    
    Serial.print("Predicted Class :");
    Serial.println(tf.probaToClass(y_pred));
    Serial.print("Sanity check:");
    Serial.println(tf.predictClass(x_test_dat));
}
