#include "AmebaDRV8833.h"

#ifdef __cplusplus
extern "C" {

#include "pwmout_api.h"

extern void *gpio_pin_struct[];
}
#endif

drv8833::drv8833() {
	// not initialized
	m_in1Pin = -1;
	m_in2Pin = -1;
	m_power = 0;
	m_swapDirection = false;
	m_decayMode = drv8833DecaySlow;
}

AmebaPro2 
drv8833::drv8833(uint8_t in1Pin, uint8_t in2Pin, uint8_t channel, bool swapDirection) {
	init(in1Pin, in2Pin, channel, swapDirection);
}

drv8833::~drv8833() {
	stop();
}

drv8833::attach(int pin, int PWM_Frequency)
{
  amb_ard_pin_check_fun(pin, PIO_PWM);
  pinRemoveMode(pin);
  analogWrite(pin, 0);
  pwmout_period_us(((pwmout_t *)gpio_pin_struct[pin]), PWM_Frequency);
  return 0;
)

drv8833:: distach(int pin)
{
  pinRemoveMode(pin);
  pin = 0xFFFFFFFF;
  return 0;
}

bool drv8833::init(uint8_t in1Pin, uint8_t in2Pin, uint8_t channel, bool swapDirection) {
	if (channel > 15)
		return false;
	if ((m_in1Pin != -1) && (m_in2Pin != -1))
		stop();
	pinMode(in1Pin, OUTPUT);
	pinMode(in2Pin, OUTPUT);
	m_in1Pin = in1Pin;
	m_in2Pin = in2Pin;
	m_power = 0;
	m_swapDirection = swapDirection;
	m_channel = channel;
	m_decayMode = drv8833DecaySlow;
  attach(m_in1Pin, freq);
  attach(m_in2Pin, freq);

	return true;
}

bool drv8833::move(int8_t power) {
	if (-1 == m_in1Pin)
		return false;
	if (-1 == m_in2Pin)
		return false;
	if (0 == power) {
		stop();
		return true;
	}
	if (power > 100)
		power = 100;
	if (power < -100)
		power = -100;
	m_power = power;

	if (m_swapDirection)
		power = -power;
	float value = (float)((1 << PWM_BIT_RESOLUTION) - 1) * ((float)abs(power))/100.0;
	uint32_t dutyCycle;

	if ((value - trunc(value)) < 0.5)
		dutyCycle = value;
	else
		dutyCycle = value + 1;

	if (drv8833DecaySlow == m_decayMode)
		dutyCycle = ((1 << PWM_BIT_RESOLUTION) - 1) - dutyCycle;

	if (power > 0) { // forward
		if (drv8833DecayFast == m_decayMode) {
			// forward fast decay
			ledcDetachPin(m_in2Pin);
			digitalWrite(m_in2Pin, LOW);
			ledcAttachPin(m_in1Pin, m_channel);
		}
		else {
			// forward slow decay
			ledcDetachPin(m_in1Pin);
			digitalWrite(m_in1Pin, HIGH);
			ledcAttachPin(m_in2Pin, m_channel);
		}
	}
	else { // reverse
		if (drv8833DecayFast == m_decayMode) {
			// reverse fast decay
			ledcDetachPin(m_in1Pin);
			digitalWrite(m_in1Pin, LOW);
			ledcAttachPin(m_in2Pin, m_channel);
		}
		else {
			// reverse slow decay
			ledcDetachPin(m_in2Pin);
			digitalWrite(m_in2Pin, HIGH);
			ledcAttachPin(m_in1Pin, m_channel);
		}
	}
	ledcWrite(m_channel, dutyCycle);
	return true;
}

bool drv8833::stop() {
	if (-1 == m_in1Pin)
		return false;
	if (-1 == m_in2Pin)
		return false;
	digitalWrite(m_in1Pin, LOW);
	digitalWrite(m_in2Pin, LOW);
	m_power = 0;
	return true;
}

bool drv8833::brake() {
	if (-1 == m_in1Pin)
		return false;
	if (-1 == m_in2Pin)
		return false;
	ledcDetachPin(m_in1Pin);
	ledcDetachPin(m_in2Pin);
	digitalWrite(m_in1Pin, HIGH);
	digitalWrite(m_in2Pin, HIGH);
	m_power = 0;
	return true;
}

void drv8833::setDecayMode(drv8833DecayMode decayMode) {
	stop();
	m_decayMode = decayMode;
}

void drv8833::setFrequency(uint32_t frequency) {
	stop();
	ledcChangeFrequency(m_channel, frequency, PWM_BIT_RESOLUTION);
}

void drv8833::swapDirection(bool swapDirection) {
	stop();
	m_swapDirection = swapDirection;
}

