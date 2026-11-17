# E18-D80NK Proximity Sensor

- Input Voltage: 5V
- Current Consumption: 25-100 mA
- Response time <2ms
- Sensor type: Diffuse reflective type
- Sensing range: 3-80 cm
- Cable length: 45 cm

## Pin Configuration

The table below showcases the pin configuration of the IR proximity sensor. It has 3 output wires, which are generally color-coded with Red as VCC, Green as the ground, and Yellow being the Digital output.

| Pin            | Std Colour | Alt Colour |
|:--------------:|:----------:|:----------:|
| VCC            |            | Brown      |
| GND            | G          | Blue       |
| Digital Output | Yellow     | Black      |

The E18-D80NK is a variable range non-contact detection sensor. It comes with a transmitter and receiver in a single module setup. The transmitter emits a modulated IR signal which is reflected by the object in its field of view or the path and the reflected signal is then collected by the transmitter, providing a digital output to the microcontroller.

## Level shifting for 3.3V

Advice from Claude

[Prompt: I have a sensor that operates at 5V. Its output signal is normally hi, pulled up by an internal 10K resistor. It goes low when active. I need to feed this signal to a microcontroller which operates at 3.3V and is not 5V tolerant. So I plan to feed the signal via a voltage divider using two resistors. I need to know the best values for the resistors. My instinct was to go with 22K and 33K with the signal going through the 22K and the 33K connected to ground. But is that value likely to cause any problems with the ability of the sensor to pull the signal low?]

Your 22K/33K divider won't hurt the sensor's ability to pull low, but it gives a marginal logic-high at the microcontroller, because the sensor's internal 10K pull-up is part of the circuit.

**Low state:** The sensor only has to sink about 5V / (10K + 55K) ≈ 77 µA, which is trivial for any open-collector or open-drain output. The MCU sees roughly 0V.

**High state:** The 10K pull-up sits on top of your divider, so the voltage at the MCU pin is:

V = 5V × Rg / (10K + Rs + Rg) = 5 × 33 / (10 + 22 + 33) ≈ **2.54V**

That's well under 3.3V, so it's safe. But many 3.3V MCUs specify VIH as 0.7 × VDD ≈ 2.31V, which leaves only about 0.2V of margin. With 5% resistors and a sagging 5V rail, it could get flaky.

**Better option:** Since the internal 10K is already the top half of a divider, you don't need the series resistor. Just add one resistor from the signal line to ground, and the signal goes straight to the MCU pin:

- **Rg = 15K** gives 5 × 15 / 25 = **3.0V** high, about 200 µA sunk when low
- **Rg = 18K** gives 5 × 18 / 28 ≈ **3.2V** high, about 180 µA sunk when low

Don't go to 20K or above, since the high level would reach or exceed 3.3V once you allow for tolerances. Either value gives plenty of margin above VIH and stays under the pin's maximum. If you want some pin protection, add a 1K series resistor between the junction and the MCU, which barely changes the numbers.

If the sensor's output turns out to be push-pull rather than open-collector, the internal pull-up isn't the whole story, and a straight 10K/15K or 22K/33K divider would be right. For a cleaner and more robust fix, a BSS138-based bidirectional level shifter works regardless of the output type, and it also avoids loading the sensor.
