/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\
The platform for ultra-low latency audio and sensor processing
http://bela.io
A project of the Augmented Instruments Laboratory within the
Centre for Digital Music at Queen Mary University of London.
http://www.eecs.qmul.ac.uk/~andrewm
(c) 2016 Augmented Instruments Laboratory: Andrew McPherson,
  Astrid Bin, Liam Donovan, Christian Heinrichs, Robert Jack,
  Giulio Moro, Laurel Pardue, Victor Zappi. All rights reserved.
The Bela software is distributed under the GNU Lesser General Public License
(LGPL 3.0), available here: https://www.gnu.org/licenses/lgpl-3.0.txt
*/


#include <Bela.h>
#include <cmath>
#include <OSCServer.h>
#include <OSCClient.h>

OSCServer oscServer;
OSCClient oscClient;


// Set range for analog outputs designed for driving LEDs
const float kMinimumAmplitude = (1.5 / 5.0);
const float kAmplitudeRange = 1.0 - kMinimumAmplitude;

float gFrequency = 1.0;
float gPhase;
float gInverseSampleRate;
//make an outputfreq and input array for all the analog ins and outs on the board. inputs are not yet implemented. 
float outputfreq [8];
float input [8]; 
//this is the stuff for the osc server: 
int localPort = 7562;
int remotePort = 7563;
const char* remoteIp = "192.168.7.1";

// parse messages received by OSC Server
// msg is Message class of oscpkt: http://gruntthepeon.free.fr/oscpkt/
int parseMessage(oscpkt::Message msg){
    
    rt_printf("received message to: %s\n", msg.addressPattern().c_str());
    
    int intArg;
    float floatArg;
// this makes the bela expect a 32bit integer first for the channel # and a floating point argument for the frequency value second: 
    if (msg.match("/osc-test").popInt32(intArg).popFloat(floatArg).isOkNoMoreArgs()){
        rt_printf("received int %i and float %f\n", intArg, floatArg);
    }
    else if  (msg.match("/freq").popInt32(intArg).popFloat(floatArg).isOkNoMoreArgs()){
    	rt_printf("here\n");
    //else if  (msg.match("/freq").popFloat(floatArg).isOkNoMoreArgs()){
    	//rt_printf("here\n");
    // we added an else if condition to make sure the received freq value is within certain range
    	if (floatArg <= 12000 && floatArg >= 0.00001)
    	{
    		rt_printf("received float %f\n", floatArg);
    		//gFrequency=floatArg;
		// this makes the float write to the corresponding entry index in the array: 
    		outputfreq[intArg]=floatArg;
    	}
    }
    return intArg;

}

bool setup(BelaContext *context, void *userData)
{

	// Check if analog channels are enabled
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return false;
	}

	// Check that we have the same number of inputs and outputs.
	if(context->audioInChannels != context->audioOutChannels ||
			context->analogInChannels != context-> analogOutChannels){
		printf("Error: for this project, you need the same number of input and output channels.\n");
		return false;
	}

	gInverseSampleRate = 1.0 / context->analogSampleRate;
	gPhase = 0.0;
	
	oscServer.setup(localPort);
    oscClient.setup(remotePort, remoteIp);
   
	// the following code sends an OSC message to address /osc-setup
	// then waits 1 second for a reply on /osc-setup-reply
    bool handshakeReceived = false;
    oscClient.sendMessageNow(oscClient.newMessage.to("/osc-setup").end());
    oscServer.receiveMessageNow(1000);
    while (oscServer.messageWaiting()){
        if (oscServer.popMessage().match("/osc-setup-reply")){
            handshakeReceived = true;
        }
    }
    
    if (handshakeReceived){
        rt_printf("handshake received!\n");
    } else {
        rt_printf("timeout!\n");
    }

	return true;
}

void render(BelaContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->analogFrames; n++) {
		// Set LED to different phase for each matrix channel
		float relativePhase = 0.0;
		for(unsigned int channel = 0; channel < context->analogOutChannels; channel++) {
			float out = kMinimumAmplitude + kAmplitudeRange * 0.5f * (1.0f + sinf(gPhase + relativePhase));

			analogWrite(context, n, channel, out);

			// Advance by pi/4 (1/8 of a full rotation) for each channel
			relativePhase += M_PI * 0.25;
		}

        // Update and wrap phase of sine tone
		//this is the original code using gFrequency from the analog out example
		//gPhase += 2.0 * M_PI * gFrequency * gInverseSampleRate;
		//but we want to use our newly created output frequency array values, like this?
		gPhase += 2.0 * M_PI * outputfreq[intArg] * gInverseSampleRate;
		if(gPhase > 2.0 * M_PI)
			gPhase -= 2.0 * M_PI;
		
	// is this where updating the outputfreq array goes? 
	}
	
	while (oscServer.messageWaiting()){
        int count = parseMessage(oscServer.popMessage());
        oscClient.queueMessage(oscClient.newMessage.to("/osc-acknowledge").add(count).add(4.2f).add(std::string("OSC message received")).end());
    }

}


void cleanup(BelaContext *context, void *userData)
{

}
