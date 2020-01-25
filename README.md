
<h2>Driving a Vacuum Fluorescent Display with an Arduino</h2>

<p>Written with a WeMos D1 Mini in mind mainly because of its small size and WiFi capabilities, see <a href="https://www.g7smy.co.uk/2018/09/rebuilding-the-vaccuum-florescent-display/">http://www.g7smy.co.uk/2018/09/rebuilding-the-vaccuum-florescent-display/</a> for details of the hardware.</p>

<p>The following libraries are used:</p>
<ul>
<li>Adafruit <a href="https://github.com/adafruit/Adafruit_NeoPixel">NeoPixel</a> library</li>
<li>Adafruit <a href="https://github.com/adafruit/Adafruit-MCP23008-library">MCP23008</a> I<sup>2</sup>C I/O Expander</li>
<li><a href="https://playground.arduino.cc/Main/ShiftOutX">ShiftOutX</a> v1.0 for the MAX6920 VFD Drivers</li>
<li><a href="https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi">ESP8266WiFi</a> for the WeMos WiFi connection.</li>
<li>The <a href="https://github.com/PaulStoffregen/Time">Time</a> library</li>
</ul>

<p>This software is in the 'requires more work' category, for me it works but you may experience difficulties. Think of it as a clue on your quest, one of those scrolls, rather than a ring of power.</p>
