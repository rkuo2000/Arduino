#include <EloquentTinyML.h>
#include "tinyml_airdigit.h"
#include "x_test.h"

#define NUMBER_OF_INPUTS  90
#define NUMBER_OF_OUTPUTS 10
// preallocate a certain amount of memory for input, output, and intermediate arrays.
#define TENSOR_ARENA_SIZE 16*1024 

  
Eloquent::TinyML::TfLite<NUMBER_OF_INPUTS, NUMBER_OF_OUTPUTS, TENSOR_ARENA_SIZE> ml;
    
void setup() {  
    Serial.begin(115200);
    ml.begin(tinyml_airdigit);
    Serial.println(sizeof(tinyml_airdigit));
}

void loop() { 
    float y_pred[10] ={0};

    uint32_t lastTime = micros(); 
    
    ml.predict(x_test_dat, y_pred);

    uint32_t inferTime = micros() - lastTime;

    Serial.print("The inference took ");
    Serial.print(inferTime);
    Serial.println(" us");

    for (int i=0; i<10;i++) {
      Serial.print(y_pred[i]);
      Serial.print(i==9 ? '\n' : ',');
    }
    
    Serial.print("Predicted Class :");
    Serial.println(ml.probaToClass(y_pred));
    Serial.print("Sanity check:");
    Serial.println(ml.predictClass(x_test_dat));
}
