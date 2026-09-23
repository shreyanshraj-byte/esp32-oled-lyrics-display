// TAP TIMER - records the timing of each word while the song plays.
// Tap on every new word: press ENTER in the Serial Monitor (or the BOOT button).
// The result appears in Tools > Serial Monitor (115200 baud).

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 d(128, 64, &Wire, -1);
const int BTN = 0;   // BOOT button

// ===== Paste the SAME lyrics as in your lyrics sketch =====
const char* LYRICS = R"LYR(
Paste your lyrics here
)LYR";
// ==========================================================

String tokens[300];
int NUM = 0;
unsigned long tapTimes[301];

void parseLyrics() {
  String s = LYRICS, cur = "";
  bool skip = false;
  for (int k = 0; k <= (int)s.length(); k++) {
    char c = (k < (int)s.length()) ? s[k] : '\n';
    if (c == '[') { skip = true; continue; }
    if (c == ']') { skip = false; continue; }
    if (skip) continue;
    if (c == '"' || c == '(' || c == ')' || c == '.' || c == '!' || c == '?') continue;
    if (c == ' ' || c == ',' || c == '\n' || c == '\r' || c == '\t') {
      if (cur.length() && NUM < 300) tokens[NUM++] = cur;
      cur = "";
    } else cur += c;
  }
}

void show(String top, String big) {
  d.clearDisplay();
  d.setTextSize(1);
  d.setCursor(0, 0);
  d.print(top);
  d.setTextSize(big.length() <= 10 ? 2 : 1);
  d.setCursor(0, 28);
  d.print(big);
  d.display();
}

// Returns the time of a tap (Enter key in Serial Monitor or BOOT button), 0 if none
unsigned long checkTap() {
  if (Serial.available()) {
    unsigned long t = millis();
    delay(5);
    while (Serial.available()) Serial.read();
    return t;
  }
  if (digitalRead(BTN) == LOW) {
    unsigned long t = millis();
    delay(25);
    while (digitalRead(BTN) == LOW) delay(1);
    return t;
  }
  return 0;
}

unsigned long waitTap() {
  unsigned long t;
  while ((t = checkTap()) == 0) delay(1);
  return t;
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN, INPUT_PULLUP);
  Wire.begin(21, 22);
  d.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  d.setTextColor(WHITE);
  d.setTextWrap(false);
  parseLyrics();
}

void loop() {
  while (Serial.available()) Serial.read();
  show(String(NUM) + " words. Tap", "on word 1");
  Serial.println("Ready! Press ENTER here on every new word.");
  for (int i = 0; i <= NUM; i++) {
    tapTimes[i] = waitTap();
    if (i < NUM) show("Word " + String(i + 1) + "/" + String(NUM) + " (tap next)", tokens[i]);
    else show("Done! Look at", "Serial Mon");
  }

  Serial.println("\n----- COPY THIS LINE INTO YOUR LYRICS SKETCH -----");
  Serial.print("const int TIMES[] = {");
  for (int i = 0; i < NUM; i++) {
    Serial.print(tapTimes[i + 1] - tapTimes[i]);
    if (i < NUM - 1) Serial.print(", ");
  }
  Serial.println("};");
  Serial.println("--------------------------------------------------");
  Serial.println("Press ENTER to record a new take.");

  waitTap();
}
