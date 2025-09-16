// NTC sensor (10k) on A0 with 10k resistor divider
// LED on D13 as fan simulation (software PWM)

const int sensorPin = A0;   // NTC divider output
const int fanPin = 13;      // LED simulating fan

// NTC parameters (for 10k NTC thermistor)
const float R_FIXED = 10000.0;   // fixed resistor 10k (ohms)
const float R0 = 10000.0;        // NTC resistance at 25°C
const float T0 = 298.15;         // 25°C in Kelvin
const float BETA = 3950.0;       // Beta value (typical)

void setup() {
  pinMode(fanPin, OUTPUT);
  Serial.begin(9600);
}

// ---- Function to read real temperature from NTC ----
float readTemp() {
  int adc = analogRead(sensorPin);   // 0–1023
  if (adc <= 0) adc = 1;             // avoid /0
  if (adc >= 1023) adc = 1022;

  // Voltage divider: Rntc = R_FIXED * (1023/adc - 1)
  float Rntc = R_FIXED * (1023.0 / adc - 1.0);

  // Beta equation: 1/T = 1/T0 + (ln(R/R0)/BETA)
  float tKelvin = 1.0 / ( (1.0 / T0) + (log(Rntc / R0) / BETA) );
  return tKelvin - 273.15;   // Convert to °C
}

// ---- Software PWM for D13 ----
void softPWM(int duty) {
  int period = 20;                         // ms per cycle (~50 Hz)
  int onTime = (period * duty) / 100;
  int offTime = period - onTime;

  if (duty > 0) {
    digitalWrite(fanPin, HIGH);
    delay(onTime);
  }
  if (duty < 100) {
    digitalWrite(fanPin, LOW);
    delay(offTime);
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
    softPWM(duty);
  }
}
