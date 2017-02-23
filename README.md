# belaproject
this is the repo for a s17 independent study developping hardware with the bela 

notes 2/8/17

bbb is acting as a web server 192.168.7.2 
editing code on bbb through browser, gets compiled on bbb and then run on bbb
bbb has an OSC server and client waiting for something 

running osc.js in node on the laptop to talk to bbb 

managed to send custom osc message to bela using oscsend 

issues getting osc from force to bela 

homeworks: look into led metering / layout / order bela / fix all the circuit related issues in http://blog.bela.io/2016/11/15/analog-synth/

related links: 

https://forum.bela.io/d/69-serial-communication-or-similar/8

https://www.adafruit.com/products/2868  

https://github.com/BelaPlatform/Bela/blob/master/examples/05-Communication/OSC/render.cpp

2/13/17

how does bela address LEDs / conversely, can we still program the BBB to directly address LEDstrips  

SSL certificates expired on the Force so python script couldnt handshake with the force via webaudio but itll work 

2/23/17

received bela - editor works on current setup 
downloaded fritzing and bela part 
how many outputs can it drive from a 5v power supply (max current draw)

learned the difference between sinf and sinf_neon (part of the math_neon)

figure out easiest way to send osc message from laptop on windows
twisted: pipe network traffic from one port to another 



