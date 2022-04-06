/** 
 * ESP32 I2S Serial Plotter Example.
 * 
 * This example is based on the espressif-idf example and Public Domain.
 * @author maspetsberger 
 */

#include <driver/i2s.h>

const i2s_port_t I2S_PORT = I2S_NUM_0;

void setup() {
  Serial.begin(115200);
  Serial.println("Configuring I2S...");
  esp_err_t err;

  // The I2S config as per the example
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
      .sample_rate = 44100,                         // 16KHz
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // could only get it to work with 32bits
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // SEL =0, its config should be left
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
      .dma_buf_count = 4,                           // number of buffers
      .dma_buf_len = 8                              // 8 samples per buffer (minimum)
  };

  // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 2,    // BCKL (INMP441-SCK)
      .ws_io_num = 15,    // LRCL (INMP441-WS)
      .data_out_num = -1, // not used (only for speakers)
      .data_in_num = 13   // DOUT (INMP441-SD)
  };

  // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
  Serial.println("I2S driver installed.");
}

void loop() {
  int32_t digitalSample = 0; 
  size_t bytes_read = 0; 
  esp_err_t result = i2s_read(I2S_PORT, &digitalSample, sizeof(digitalSample), &bytes_read, /*portMAX_DELAY*/ 10); // no timeout 
  if (bytes_read > 0) { 
    Serial.println(digitalSample >> 16);
  }  
}
// actually we would need to call `i2s_driver_uninstall(I2S_PORT)` upon exit. 
