#include <EloquentTinyML.h>
#include "airdigit_cnn.h"
#include "x_test.h"

#define NUMBER_OF_INPUTS  24*3
#define NUMBER_OF_OUTPUTS 10
// preallocate a certain amount of memory for input, output, and intermediate arrays.
#define TENSOR_ARENA_SIZE 24*1024 

Eloquent::TinyML::TfLite<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> ml;

void setup() {
    Serial.begin(115200);
    ml.begin(airdigit_cnn);
}

void loop() {
    int i;
    float y_pred[10] ={0};

    uint32_t start = micros(); 
     
    ml.predict(x_test_dat, y_pred);

    uint32_t timeit = micros() - start;

    Serial.print("It took ");
    Serial.print(timeit);
    Serial.println(" micros to run inference");

    for (i=0; i<10;i++) {
      Serial.print(y_pred[i]);
      Serial.print(i == 9 ? '\n' : ',');
    }
    
    Serial.print("Predicted Class :");
    Serial.println(ml.probaToClass(y_pred));
    Serial.print("Sanity check:");
    Serial.println(ml.predictClass(x_test_dat));
}
