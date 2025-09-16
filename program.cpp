// NTC sensor (10k) on A0 with 10k resistor divider
// LED on D13 as fan simulation (software PWM)

const int sensor_pin = A0;   // NTC divider output
const int fan_pin = 13;      // LED simulating fan

// NTC parameters (for 10k NTC thermistor)
const float R_FIXED = 10000.0;   // fixed resistor 10k (ohms)
const float R0 = 10000.0;        // NTC resistance at 25°C
const float T0 = 298.15;         // 25°C in Kelvin
const float BETA = 3950.0;       // Beta value (typical)

void setup() {
  pinMode(fan_pin, OUTPUT);
  Serial.begin(9600);
}

// ---- Function to read real temperature from NTC ----
float readTemp() {
  int adc = analogRead(sensor_pin);   // 0–1023
  if (adc <= 0) adc = 1;             // avoid /0
  if (adc >= 1023) adc = 1022;

  // Voltage divider: Rntc = R_FIXED * (1023/adc - 1)
  float Rntc = R_FIXED * (1023.0 / adc - 1.0);

  // Beta equation: 1/T = 1/T0 + (ln(R/R0)/BETA)
  float t_kelvin = 1.0 / ( (1.0 / T0) + (log(Rntc / R0) / BETA) );
  return t_kelvin - 273.15;   // Convert to °C
}

// ---- Software PWM for D13 ----
void soft_PWM(int duty) {
  int period = 20;                         // ms per cycle (~50 Hz)
  int on_time = (period * duty) / 100;
  int off_time = period - on_time;

  if (duty > 0) {
    digitalWrite(fan_pin, HIGH);
    delay(on_time);
  }
  if (duty < 100) {
    digitalWrite(fan_pin, LOW);
    delay(off_time);
  }
}

// ---- Main loop ----
void loop() {
  float t = readTemp();    // get real temperature
  int duty = 0;            // fan duty %
  String state = "OFF";

  if (t >= 25 && t < 30) {
    duty = 50;
    state = "50%";
  } else if (t >= 30) {
    duty = 100;
    state = "100%";
  }

  // Print info
  Serial.print("Temp: ");
  Serial.print(t, 2);   // 2 decimals
  Serial.print(" °C | Fan: ");
  Serial.println(state);

  // Run PWM for ~1 second
  unsigned long start = millis();
  while (millis() - start < 1000) {
    soft_PWM(duty);
  }
}

