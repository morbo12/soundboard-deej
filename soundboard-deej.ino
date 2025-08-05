
const int NUM_SLIDERS = 4;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3};

int analogSliderValues[NUM_SLIDERS];

int prevSliderValues[NUM_SLIDERS] = {0};
const int SLIDER_NOISE_THRESHOLD = 2;       // Increased threshold to reduce noise sensitivity
const unsigned long MIN_SEND_INTERVAL = 50; // Minimum milliseconds between sends
unsigned long lastSendTime = 0;

void setup()
{
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    pinMode(analogInputs[i], INPUT);
  }

  Serial.begin(9600);

  // Initialize previous values with actual readings to prevent initial false triggers
  delay(100); // Allow analog inputs to stabilize
  updateSliderValues();
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    prevSliderValues[i] = analogSliderValues[i];
  }
}

void loop()
{
  updateSliderValues();

  // Only send if values changed significantly AND enough time has passed
  if (sliderValuesChanged() && (millis() - lastSendTime >= MIN_SEND_INTERVAL))
  {
    sendSliderValues();
    lastSendTime = millis();

    // Update previous values only after successful send
    for (int i = 0; i < NUM_SLIDERS; i++)
    {
      prevSliderValues[i] = analogSliderValues[i];
    }
  }
  // printSliderValues(); // For debug
  delay(10);
}

bool sliderValuesChanged()
{
  // Check if any slider has changed beyond the noise threshold
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    if (abs(analogSliderValues[i] - prevSliderValues[i]) > SLIDER_NOISE_THRESHOLD)
    {
      return true;
    }
  }
  return false;
}

void updateSliderValues()
{
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    // Take multiple readings and average them for noise reduction
    int reading1 = analogRead(analogInputs[i]);
    delay(1);
    int reading2 = analogRead(analogInputs[i]);
    delay(1);
    int reading3 = analogRead(analogInputs[i]);

    analogSliderValues[i] = (reading1 + reading2 + reading3) / 3;
  }
}

void sendSliderValues()
{
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    // Scale value from 0-1023 to 0-100
    int scaledValue = map(analogSliderValues[i], 0, 1023, 0, 100);
    builtString += String(scaledValue);

    if (i < NUM_SLIDERS - 1)
    {
      builtString += String("|");
    }
  }
  // builtString += String("#");
  Serial.println(builtString);
}

void printSliderValues()
{
  for (int i = 0; i < NUM_SLIDERS; i++)
  {
    int scaledValue = map(analogSliderValues[i], 0, 1023, 0, 100);
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(scaledValue) + String(" %");
    Serial.print(printedString);

    if (i < NUM_SLIDERS - 1)
    {
      Serial.print(" | ");
    }
    else
    {
      Serial.println();
    }
  }
}
