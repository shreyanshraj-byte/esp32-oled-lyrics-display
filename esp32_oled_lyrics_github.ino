// ESP32 + 0.96" SSD1306 OLED - Word-by-word lyrics, sunny "feel-good" backgrounds chosen by the words
// Wiring: VDD->3V3, GND->GND, SDA->G21, SCK->G22

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define W 128
#define H 64
Adafruit_SSD1306 d(W, H, &Wire, -1);

// ================= EDIT HERE =================
// 1) Paste your lyrics between the brackets below, exactly as they are.
//    Words are split automatically. Lines like [Chorus] are skipped.
const char* LYRICS = R"LYR(
Paste your lyrics here
)LYR";

// 2) Paste the TIMES line from the Tap Timer's Serial Monitor here:
const int TIMES[] = {500};
const int DEFAULT_MS = 500;   // used for any word without a time
// =============================================

String tokens[300];
int NUM = 0;

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

int timeFor(int i) {
  int n = sizeof(TIMES) / sizeof(TIMES[0]);
  return (i < n) ? TIMES[i] : DEFAULT_MS;
}

const int BTN = 0;   // BOOT button starts the song
const int NUM_BG = 10;
int frameNo = 0;

// ---------- helpers ----------
void polygon(int cx, int cy, int n, float r, float a) {
  for (int i = 0; i < n; i++) {
    float a1 = a + i * TWO_PI / n, a2 = a + (i + 1) * TWO_PI / n;
    d.drawLine(cx + r * cos(a1), cy + r * sin(a1), cx + r * cos(a2), cy + r * sin(a2), WHITE);
  }
}

void ellipse(int cx, int cy, float rx, float ry) {
  for (int j = 0; j < 24; j++) {
    float t1 = j * TWO_PI / 24, t2 = (j + 1) * TWO_PI / 24;
    d.drawLine(cx + rx * cos(t1), cy + ry * sin(t1), cx + rx * cos(t2), cy + ry * sin(t2), WHITE);
  }
}

// ---------- FEEL-GOOD backgrounds ----------
void curve(int x0, int y0, int x1, int y1, int x2, int y2, int c) {
  float px = x0, py = y0;
  for (int i = 1; i <= 10; i++) {
    float t = i / 10.0, u = 1 - t;
    float x = u * u * x0 + 2 * u * t * x1 + t * t * x2;
    float y = u * u * y0 + 2 * u * t * y1 + t * t * y2;
    d.drawLine(px, py, x, y, c);
    px = x; py = y;
  }
}

void star(int x, int y, int big) {
  d.drawPixel(x, y, WHITE);
  if (big) { d.drawFastHLine(x - 2, y, 5, WHITE); d.drawFastVLine(x, y - 2, 5, WHITE); }
}

void bird(int x, int y, int up) {
  int w = up ? -5 : 3;
  d.drawLine(x - 8, y + w, x - 3, y, WHITE);
  d.drawLine(x - 8, y + w + 1, x - 3, y + 1, WHITE);
  d.fillCircle(x, y + 1, 2, WHITE);
  d.drawLine(x + 3, y, x + 8, y + w, WHITE);
  d.drawLine(x + 3, y + 1, x + 8, y + w + 1, WHITE);
}

void leaf(int x, int y, float a, int len) {
  int x1 = x + len * cos(a), y1 = y + len * sin(a);
  int x2 = x - len * cos(a), y2 = y - len * sin(a);
  int sx = 2 * cos(a + PI / 2), sy = 2 * sin(a + PI / 2);
  d.drawLine(x1, y1, x + sx, y + sy, WHITE); d.drawLine(x + sx, y + sy, x2, y2, WHITE);
  d.drawLine(x1, y1, x - sx, y - sy, WHITE); d.drawLine(x - sx, y - sy, x2, y2, WHITE);
  d.drawLine(x1, y1, x2, y2, WHITE);
}

void cloud(int x, int y) {
  d.fillCircle(x, y, 5, WHITE);
  d.fillCircle(x + 7, y - 3, 6, WHITE);
  d.fillCircle(x + 14, y, 5, WHITE);
  d.fillRect(x, y, 15, 5, WHITE);
}

void sunWithRays(int x, int y, int r, float spin) {
  d.fillCircle(x, y, r, WHITE);
  for (int k = 0; k < 10; k++) {
    float a = spin + k * TWO_PI / 10;
    d.drawLine(x + (r + 3) * cos(a), y + (r + 3) * sin(a), x + (r + 7) * cos(a), y + (r + 7) * sin(a), WHITE);
  }
}

void note(int x, int y) {                    // music note
  d.fillCircle(x, y, 2, WHITE);
  d.drawFastVLine(x + 2, y - 9, 9, WHITE);
  d.drawLine(x + 2, y - 9, x + 6, y - 5, WHITE);
}

void doubleNote(int x, int y) {
  d.fillCircle(x, y, 2, WHITE);
  d.fillCircle(x + 8, y - 2, 2, WHITE);
  d.drawFastVLine(x + 2, y - 9, 9, WHITE);
  d.drawFastVLine(x + 10, y - 11, 9, WHITE);
  d.drawLine(x + 2, y - 9, x + 10, y - 11, WHITE);
  d.drawLine(x + 2, y - 8, x + 10, y - 10, WHITE);
}

void butterfly(int x, int y, int open) {
  if (open) {
    d.fillCircle(x - 4, y - 2, 3, WHITE); d.fillCircle(x + 4, y - 2, 3, WHITE);
    d.fillCircle(x - 3, y + 3, 2, WHITE); d.fillCircle(x + 3, y + 3, 2, WHITE);
  } else {
    d.fillRect(x - 2, y - 4, 1, 8, WHITE); d.fillRect(x + 2, y - 4, 1, 8, WHITE);
  }
  d.drawFastVLine(x, y - 4, 9, WHITE);
  d.drawLine(x, y - 4, x - 2, y - 7, WHITE); d.drawLine(x, y - 4, x + 2, y - 7, WHITE);
}

void bigFlower(int x, int grow) {            // flower growing from the ground
  int h = min(grow, 34);
  d.drawLine(x, 63, x, 63 - h, WHITE);
  if (h > 12) leaf(x - 4, 52, 0.5, 4);
  if (h > 20) leaf(x + 4, 46, -0.5, 4);
  if (grow > 34) {
    int r = min((grow - 34) / 3, 3);
    for (int k = 0; k < 6; k++) {
      float a = k * TWO_PI / 6;
      d.fillCircle(x + (r + 2) * cos(a), 63 - h + (r + 2) * sin(a), r, WHITE);
    }
    d.fillCircle(x, 63 - h, 2, BLACK);
    d.drawCircle(x, 63 - h, 2, WHITE);
  }
}

// --- the backgrounds ---
void bgBirds(int f) {                        // flock flying across
  int up = (f / 4) % 2;
  for (int k = 0; k < 5; k++) {
    int x = (f * 2 + k * 30) % 170 - 20;
    int y = 8 + (k % 3) * 18 + 3 * sin(f * 0.1 + k);
    bird(x, y, (up + k) % 2);
  }
}

void bgSunSky(int f) {                       // sun + drifting clouds
  sunWithRays(18, 14, 7, f * 0.05);
  cloud((f + 60) % 170 - 20, 52);
  cloud((f / 2 + 20) % 170 - 20, 8);
}

void bgBreeze(int f) {                       // wind swirls + blown leaves
  for (int k = 0; k < 4; k++) {
    int sx = (f * 4 + k * 47) % 190 - 40, y = 8 + k * 15;
    for (int x = 0; x < 34; x++) d.drawPixel(sx + x, y + 2 * sin((sx + x) * 0.2), WHITE);
    d.drawCircle(sx + 36, y - 2, 3, WHITE);
  }
  for (int i = 0; i < 4; i++) {
    int x = (f * 3 + i * 40) % 170 - 20, y = 12 + i * 13 + 4 * sin(f * 0.15 + i);
    leaf(x, y, f * 0.2 + i, 3);
  }
}

void bgSunrise(int f) {                      // sun rising over the horizon
  int p = f % 150;
  int cy = 62 - min(p, 40);
  sunWithRays(104, cy, 9, 0);
  d.fillRect(0, 50, W, 14, BLACK);
  d.drawFastHLine(0, 50, W, WHITE);
  for (int k = 0; k < 3; k++) d.drawFastHLine(10 + k * 8, 54 + k * 3, 108 - k * 16, WHITE);
}

void bgSunburst(int f) {                     // rays spinning around the word
  for (int k = 0; k < 14; k++) {
    float a = f * 0.03 + k * TWO_PI / 14;
    d.drawLine(64 + 20 * cos(a), 32 + 20 * sin(a), 64 + 80 * cos(a), 32 + 80 * sin(a), WHITE);
  }
}

void bgBloom(int f) {                        // flowers growing & opening
  int g = (f % 120);
  bigFlower(14, g);
  bigFlower(114, g - 10);
  bigFlower(30, g - 25);
  bigFlower(98, g - 30);
}

void bgMusic(int f) {                        // music notes floating up
  for (int i = 0; i < 7; i++) {
    int x = 6 + i * 18 + 5 * sin(f * 0.1 + i);
    int y = H + 12 - ((f * (1 + i % 2) + i * 21) % (H + 24));
    if (i % 2) note(x, y); else doubleNote(x, y);
  }
}

void bgCloudsBirds(int f) {                  // clouds with two birds
  cloud((f / 2) % 170 - 20, 10);
  cloud((f / 2 + 85) % 170 - 20, 50);
  int up = (f / 4) % 2;
  int bx = 150 - (f * 2) % 170;
  bird(bx, 16, up);
  bird(bx + 16, 12, !up);
}

void bgButterflies(int f) {                  // butterflies fluttering
  for (int k = 0; k < 4; k++) {
    int x = (f + k * 40) % 160 - 16, y = 12 + (k % 2) * 36 + 6 * sin(f * 0.12 + k);
    butterfly(x, y, (f / 3 + k) % 2);
  }
}

void bgSparkle(int f) {                      // shining sparkles
  randomSeed(9);
  for (int i = 0; i < 22; i++) {
    int x = random(W), y = random(H);
    int s = (f / 3 + i) % 5;
    if (s < 3) { d.drawFastHLine(x - s, y, s * 2 + 1, WHITE); d.drawFastVLine(x, y - s, s * 2 + 1, WHITE); }
    else d.drawPixel(x, y, WHITE);
  }
  randomSeed(micros());
}

void drawBg(int which, int f) {
  switch (which) {
    case 0: bgBirds(f);       break;
    case 1: bgSunSky(f);      break;
    case 2: bgBreeze(f);      break;
    case 3: bgSunrise(f);     break;
    case 4: bgSunburst(f);    break;
    case 5: bgBloom(f);       break;
    case 6: bgMusic(f);       break;
    case 7: bgCloudsBirds(f); break;
    case 8: bgButterflies(f); break;
    case 9: bgSparkle(f);     break;
  }
}

// ===== Background picked by the WORD (edit this list if you like) =====
// If a word contains a keyword, that background is used.
struct Pick { const char* key; int bg; };
Pick picks[] = {
  {"bird", 0}, {"fly", 0}, {"high", 7},
  {"sun", 1},  {"sky", 7},
  {"breeze", 2}, {"wind", 2}, {"drift", 2},
  {"dawn", 3}, {"morning", 3},
  {"day", 4},  {"shine", 9}, {"star", 9},
  {"life", 5}, {"bloom", 5}, {"flower", 5}, {"tree", 5},
  {"butterfl", 8}, {"dragonfl", 8},
  {"feel", 6}, {"good", 6}, {"song", 6},
};
// Words with no keyword cycle through these:
int otherBgs[] = {6, 9, 7, 4, 8, 1};

int pickBg(int i) {
  String w = tokens[i];
  w.toLowerCase();
  for (unsigned k = 0; k < sizeof(picks) / sizeof(picks[0]); k++)
    if (w.indexOf(picks[k].key) >= 0) return picks[k].bg;
  return otherBgs[i % (sizeof(otherBgs) / sizeof(otherBgs[0]))];
}

// ---------- word in a box ----------
void drawWord(const char* w) {
  int len = strlen(w);
  int size = (len <= 9) ? 2 : 1;
  if (len > 19) len = 19;
  int tw = len * 6 * size - size, th = 8 * size;
  int bw = tw + 12, bh = th + 8;
  int bx = (W - bw) / 2, by = (H - bh) / 2;
  d.fillRoundRect(bx, by, bw, bh, 3, BLACK);
  d.drawRoundRect(bx, by, bw, bh, 3, WHITE);
  d.drawRoundRect(bx + 1, by + 1, bw - 2, bh - 2, 3, WHITE);
  d.setTextSize(size);
  d.setTextColor(WHITE);
  d.setCursor(bx + 6, by + 4);
  d.print(w);
}

void setup() {
  Wire.begin(21, 22);
  if (!d.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) delay(100);
  }
  pinMode(BTN, INPUT_PULLUP);
  Serial.begin(115200);
  parseLyrics();
  d.setTextWrap(false);
  d.clearDisplay();
  d.display();
}

void loop() {
  // Wait screen: press BOOT when the first word is sung
  d.clearDisplay();
  d.setTextSize(1);
  d.setTextColor(WHITE);
  d.setCursor(10, 20);
  d.print("Press BOOT / ENTER");
  d.setCursor(10, 36);
  d.print(NUM);
  d.print(" words loaded");
  d.display();
  while (Serial.available()) Serial.read();
  while (digitalRead(BTN) == HIGH && !Serial.available()) delay(1);
  while (Serial.available()) Serial.read();

  unsigned long t0 = millis(), due = 0;
  for (int i = 0; i < NUM; i++) {
    int bg = pickBg(i);                      // background chosen by the word
    due += timeFor(i);                       // keeps total timing accurate
    d.invertDisplay(true);                   // quick flash between words
    delay(20);
    d.invertDisplay(false);
    while (millis() - t0 < due) {
      d.clearDisplay();
      drawBg(bg, frameNo++);
      drawWord(tokens[i].c_str());
      d.display();
    }
  }
  d.clearDisplay();
  d.display();
  delay(1000);
}
