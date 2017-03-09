#include <Bela.h>
#include <cmath>
#include <OSCServer.h>
#include <OSCClient.h>

OSCServer oscServer;
OSCClient oscClient;

float gFrequency = 110.0;
float gPhase;
float gInverseSampleRate;
int localPort = 7562;
int remotePort = 7563;
const char* remoteIp = "192.168.7.1";

// parse messages received by OSC Server
// msg is Message class of oscpkt: http://gruntthepeon.free.fr/oscpkt/
int parseMessage(oscpkt::Message msg){

    rt_printf("received message to: %s\n", msg.addressPattern().c_str());

    int intArg, intArg2;
    float floatArg;
    if (msg.match("/osc-test").popFloat(floatArg).popInt32(intArg).isOkNoMoreArgs()){
        rt_printf("received int %i and float %f\n", intArg, floatArg);
    }
    else
    if  (msg.match("/freq").popFloat(floatArg).popInt32(intArg).isOkNoMoreArgs()){
        rt_printf("received val %f and line %i\n", floatArg, intArg );
    // we added an else if condition to make sure the received freq value is within certain range
        if (floatArg <= 12000.0 && floatArg >= 40.0)
        {
                rt_printf("received float %f\n", floatArg);
                gFrequency=floatArg;

        }
    }
    else
    if  (msg.match("/freq").popInt32(intArg2).popInt32(intArg).isOkNoMoreArgs()){
        rt_printf("received val %i and line %i\n", intArg2, intArg);
    // we added an else if condition to make sure the received freq value is within certain range
        if (floatArg <= 12000.0 && floatArg >= 40.0)
        {
                rt_printf("received float %f\n", floatArg);
                gFrequency=floatArg;

        }
    }
    return intArg;
}

bool setup(BelaContext *context, void *userData)
{
        gInverseSampleRate = 1.0 / context->audioSampleRate;
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
        for(unsigned int n = 0; n < context->audioFrames; n++) {
                float out = 0.8 * sinf(gPhase);
                gPhase += 2.0 * M_PI * gFrequency * gInverseSampleRate;
                if(gPhase > 2.0 * M_PI)
                        gPhase -= 2.0 * M_PI;

                for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) {
                        // Two equivalent ways to write this code

                        // The long way, using the buffers directly:
                        // context->audioOut[n * context->audioOutChannels + channel] = out;

                        // Or using the macros:
                        audioWrite(context, n, channel, out);
                }
        }
        // receive OSC messages, parse them, and send back an acknowledgment
    while (oscServer.messageWaiting()){
        int count = parseMessage(oscServer.popMessage());
        oscClient.queueMessage(oscClient.newMessage.to("/osc-acknowledge").add(count).add(4.2f).add(std::string("OSC message received")).end());
    }
}

void cleanup(BelaContext *context, void *userData)
{

}

/**
\example OSC/render.cpp
Open Sound Control
------------------
This example shows an implementation of OSC (Open Sound Control) which was
developed at UC Berkeley Center for New Music and Audio Technology (CNMAT).
It is designed to be run alongside resources/osc/osc.js
The OSC server port on which to receive is set in `setup()`
via `oscServer.setup()`. Likewise the OSC client port on which to
send is set in `oscClient.setup()`.
In `setup()` an OSC message to address `/osc-setup`, it then waits
1 second for a reply on `/osc-setup-reply`.
in `render()` the code receives OSC messages, parses them, and sends
back an acknowledgment.
*/
