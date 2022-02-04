#pragma once
#include "Config.h"

//Per second : Kibibytes, packets, RTT(min, max, avg), frames
//Total : packet loss%, bytes sent, bytes received, messages sent, messages received, input mispredicts, packets sent, packets received

class DataOverlay
{
	static DataOverlay* overlay;
	Time timer;
	Time fpsTimer;
	Time messageTimer;
	std::string message;

	overlay_t totalPacketsLost = 0;
	overlay_t totalBytesSent = 0, totalBytesReceived = 0;
	overlay_t totalMessagesSent = 0, totalMessagesReceived = 0;
	overlay_t totalPacketsSent = 0, totalPacketsReceived = 0;
	overlay_t totalInputMispredictions = 0;
	float packetLossPercent = 0.0f;
	
	float bytesSentThisSecond = 0.0f, bytesReceivedThisSecond = 0.0f;
	float kibibytesSentLastSecond = 0.0f, kibibytesReceivedLastSecond = 0.0f;

	overlay_t packetsSentThisSecond = 0, packetsReceivedThisSecond = 0;
	overlay_t packetsSentLastSecond = 0, packetsReceivedLastSecond = 0;

	overlay_t messagesSentThisSecond = 0, messagesReceivedThisSecond = 0;
	overlay_t messagesSentLastSecond = 0, messagesReceivedLastSecond = 0;

	float rttCurrent = 0.0f;
	float rttMinThisSecond = 0.0f, rttAvgThisSecond = 0.0f;
	float rttMinLastSecond = 0.0f, rttAvgLastSecond = 0.0f;
	float rttMaxLastSecond = 0.0f, rttMaxThisSecond = HUGE_FLOAT;
	overlay_t rttCounter = 0;

	overlay_t fpsCounter = 0;
	float fps = 0.0f;
public:
	DataOverlay();
	void render(Renderer& _renderer);
	void onTick(const Time& _dt);
	void onSend(const size_t bytes, const overlay_t messages);
	void onPacketLoss();
	void onReceive(const size_t bytes, const overlay_t messages, const Time& rtt);
	static void onMispredict();
	static void displayMessage(const char* msg);
};

