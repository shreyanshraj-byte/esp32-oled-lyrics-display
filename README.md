# ESP32 OLED Lyrics Display

A small ESP32 project that shows song lyrics on a 0.96" OLED screen, one word at a time, in sync with the music. Each word appears inside a framed box with an animated background behind it, and a quick flash marks the change between words.

The project comes with a tap timer, so you can sync any song yourself: play the song and press Enter on every new word. The timings are printed out, ready to paste into the display sketch.

<!-- Photos and video: edit this file on GitHub and drag them in below this line -->


## Features

- Lyrics shown word by word, synced to the song
- A different animated background for each word
- Short words use large text, longer words switch to smaller text automatically
- Paste lyrics as plain text, with no manual formatting
- Section headers such as `[Chorus]` are skipped automatically
- Timing is stored as a running total, so small errors on one word do not push the rest of the song out of sync
- Starts on a button press, so it can be lined up with the music by hand


## Hardware

| Component | Quantity |
|---|---|
| ESP32 development board (ESP32-WROOM-32, 30 or 38 pin) | 1 |
| 0.96" SSD1306 OLED, 128x64, I2C (4 pin) | 1 |
| Jumper wires | 4 |
| Micro USB data cable | 1 |


## Wiring

| OLED pin | ESP32 pin |
|---|---|
| GND | GND |
| VDD / VCC | 3V3 |
| SCK / SCL | GPIO 22 |
| SDA | GPIO 21 |

Power the OLED from 3V3, not 5V. If the screen stays blank, check that SDA and SCK are not swapped and that every wire is pushed fully onto its pin.


## Software Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP32 support. Go to **File > Preferences** and add this to *Additional boards manager URLs*:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
   Then open **Boards Manager** and install **esp32 by Espressif Systems**.
3. Open **Library Manager**, install **Adafruit SSD1306**, and choose **Install All** so that **Adafruit GFX** is installed too.
4. Select **Tools > Board > esp32 > ESP32 Dev Module** and the correct COM port.


## Files

| File | Purpose |
|---|---|
| `esp32_oled_lyrics_github.ino` | The lyrics display with animated backgrounds |
| `esp32_tap_timer_github.ino` | Records how long each word lasts while the song plays |


## Usage

### Step 1: Record the timing

1. Open `esp32_tap_timer_github.ino`.
2. Replace `Paste your lyrics here` with your lyrics. Keep the lines `const char* LYRICS = R"LYR(` and `)LYR";` above and below them.
3. Upload the sketch, then close the Arduino IDE so the COM port is free.
4. Open the [Chrome serial terminal](https://googlechromelabs.github.io/serial-terminal/), set the baud rate to **115200**, click **Connect**, and choose your ESP32's port.
5. Press **RST** on the ESP32 and wait for `Ready! Press ENTER here on every new word.`
6. Click inside the terminal, start the song, and press **Enter** each time a new word begins. Press Enter once more after the last word.
7. Copy the line that starts with `const int TIMES[] = {`.

To record another take, press Enter again, or press RST on the board.

### Step 2: Display the lyrics

1. Open `esp32_oled_lyrics_github.ino`.
2. Paste the same lyrics used in Step 1.
3. Replace `const int TIMES[] = {500};` with the line copied in Step 1.
4. Disconnect the Chrome serial terminal, then upload the sketch.
5. The screen will show `Press BOOT / ENTER`. Start the song and press **BOOT** as the first word is sung.


## How Lyrics Are Split

- Spaces and commas separate words. Each word is one tap.
- Words joined by hyphens, such as `sa-re-ga`, count as a single word.
- Anything inside square brackets, such as `[Verse 1]`, is ignored.
- Quotes, brackets, periods, question marks and exclamation marks are removed.
- Up to 300 words are supported.


## Tips

- **Fast songs:** play the song at 0.5x speed while tapping. Then, in the display sketch, change `TIMES[i]` to `TIMES[i] / 2` in the `timeFor()` function.
- **Fixing a single word:** each number in `TIMES` belongs to one word, in order. Make a number smaller if that word stays too long, or larger if it disappears too early.
- **Everything slightly early or late:** adjust when you press BOOT at the start.
- **Running without a computer:** the sketch stays on the ESP32 after unplugging, so it can run from a phone charger or power bank.


## Troubleshooting

| Problem | Fix |
|---|---|
| `Could not open COM3, the port is busy` | Disconnect or close the Chrome serial terminal, then upload again |
| Stuck at `Connecting...` while uploading | Hold the BOOT button until the upload percentage appears |
| No COM port shown | Use a data cable, not a charge-only cable, and install the CP210x or CH340 USB driver |
| OLED is blank | Check the wiring, press RST, or change `0x3C` to `0x3D` in `setup()` |
| Screen turns off when the board is touched | A jumper wire is loose; lay the board flat and reseat the wires |
| Timing numbers are extremely large | Use the latest `esp32_tap_timer_github.ino` from this repository |


## A Note on Lyrics

Song lyrics are protected by copyright, so none are included in this repository. Add the lyrics for your own song locally before uploading to the board, and avoid committing them to a public repository.



## License

Released under the MIT License. See `LICENSE` for details.
