#include "DataOverlay.h"
#include <string>

using namespace std;

DataOverlay* DataOverlay::overlay = nullptr;

DataOverlay::DataOverlay()
{
	assert(overlay == nullptr);
	overlay = this;
}

void DataOverlay::render(Renderer& _renderer)
{
	std::string rtt = "RTT: AVG: " + to_string(rttAvgLastSecond) + " MIN: " + to_string(rttMinLastSecond) + 
		" MAX: " + to_string(rttMaxLastSecond) + " CURRENT: " + to_string(rttCurrent);
	std::string messages = "MESSAGES: SENT LAST SEC: " + to_string(messagesSentLastSecond) + " RECEIVED LAST SEC: " + to_string(messagesReceivedLastSecond) +
		" TOTAL SENT: " + to_string(totalMessagesSent) + " TOTAL RECEIVED: " + to_string(totalMessagesReceived);
	std::string packets = "PACKETS: SENT LAST SEC: " + to_string(packetsSentLastSecond) + " RECEIVED LAST SEC: " + to_string(packetsReceivedLastSecond) +
		" TOTAL SENT: " + to_string(totalPacketsSent) + " TOTAL RECEIVED: " + to_string(totalPacketsReceived);
	std::string kibibytes = "KIBIBYTES: SENT LAST SEC: " + to_string(kibibytesSentLastSecond) + " RECEIVED LAST SEC: " + to_string(kibibytesReceivedLastSecond);
	std::string bytes = "BYTES: TOTAL SENT: " + to_string(totalBytesSent) + " TOTAL RECEIVED: " + to_string(totalBytesReceived);
	std::string other = "PACKETS LOST: " + to_string(totalPacketsLost) + " (" + to_string(packetLossPercent) + "%)" + " INPUT MISPREDICTIONS: " + to_string(totalInputMispredictions) + " FPS: " + to_string(fps);

	Point nextTextPoint(0, WINH - OVERLAY_MARGIN);
	_renderer.render_text(nextTextPoint, Color::Red, OVERLAY_MESSAGE_TEXT_SIZE, message.c_str());
	nextTextPoint.y_ += UI_SPACING;
	_renderer.render_text(nextTextPoint, Color::White, OVERLAY_TEXT_SIZE, rtt.c_str());
	nextTextPoint.y_ += UI_SPACING;
	_renderer.render_text(nextTextPoint, Color::White, OVERLAY_TEXT_SIZE, messages.c_str());
	nextTextPoint.y_ += UI_SPACING;
	_renderer.render_text(nextTextPoint, Color::White, OVERLAY_TEXT_SIZE, packets.c_str());
	nextTextPoint.y_ += UI_SPACING;
	_renderer.render_text(nextTextPoint, Color::White, OVERLAY_TEXT_SIZE, kibibytes.c_str());
	nextTextPoint.y_ += UI_SPACING;
	_renderer.render_text(nextTextPoint, Color::White, OVERLAY_TEXT_SIZE, bytes.c_str());
	nextTextPoint.y_ += UI_SPACING;
	_renderer.render_text(nextTextPoint, Color::White, OVERLAY_TEXT_SIZE, other.c_str());
}

void DataOverlay::onTick(const Time& _dt)
{
	++fpsCounter;
	fpsTimer += _dt;
	if (fpsTimer > Time(1.0f / FPS_UPDATES_PER_SECOND)) {
		fpsTimer -= Time(1.0f / FPS_UPDATES_PER_SECOND);
		fps = fpsCounter * FPS_UPDATES_PER_SECOND;
		fpsCounter = 0;
	}

	messageTimer -= _dt;
	if (messageTimer <= 0.0f) {
		message = "";
		messageTimer = 0.0f;
	}

	timer += _dt;
	if (timer >= Time(1.0f)) {
		timer -= Time(1.0f);

		kibibytesSentLastSecond = bytesSentThisSecond / 1000.0f;
		bytesSentThisSecond = 0.0f;
		kibibytesReceivedLastSecond = bytesReceivedThisSecond / 1000.0f;
		bytesReceivedThisSecond = 0.0f;

		packetsSentLastSecond = packetsSentThisSecond;
		packetsSentThisSecond = 0;
		packetsReceivedLastSecond = packetsReceivedThisSecond;
		packetsReceivedThisSecond = 0;

		messagesSentLastSecond = messagesSentThisSecond;
		messagesSentThisSecond = 0;
		messagesReceivedLastSecond = messagesReceivedThisSecond;
		messagesReceivedThisSecond = 0;

		rttAvgLastSecond = rttAvgThisSecond;
		rttAvgThisSecond = 0.0f;
		rttMinLastSecond = rttMinThisSecond;
		rttMinThisSecond = HUGE_FLOAT;
		rttMaxLastSecond = rttMaxThisSecond;
		rttMaxThisSecond = 0.0f;
		rttCounter = 0;
	}
}

void DataOverlay::onSend(const size_t bytes, const overlay_t messages)
{
	overlay_t b = static_cast<overlay_t>(bytes);
	bytesSentThisSecond += b;
	totalBytesSent += b;
	messagesSentThisSecond += messages;
	totalMessagesSent += messages;
	++packetsSentThisSecond;
	++totalPacketsSent;
}

void DataOverlay::onPacketLoss()
{
	printf("Packet lost!\n");
	++totalPacketsLost;
	packetLossPercent = static_cast<float>(totalPacketsLost) / static_cast<float>(totalPacketsReceived);
}

void DataOverlay::onReceive(const size_t bytes, const overlay_t messages, const Time& rtt)
{
	overlay_t b = static_cast<overlay_t>(bytes);
	bytesReceivedThisSecond += b;
	totalBytesReceived += b;
	messagesReceivedThisSecond += messages;
	totalMessagesReceived += messages;
	++packetsReceivedThisSecond;
	++totalPacketsReceived;

	float ms = rtt.as_milliseconds();
	if (ms < rttMinThisSecond) rttMinThisSecond = ms;
	if (ms > rttMaxThisSecond) rttMaxThisSecond = ms;
	float rttOld = rttAvgThisSecond * rttCounter++;
	rttAvgThisSecond = (rttOld + ms) / rttCounter;
	rttCurrent = rtt.as_milliseconds();
}

void DataOverlay::onMispredict()
{
	++(overlay->totalInputMispredictions);
}

void DataOverlay::displayMessage(const char* msg)
{
	overlay->message = msg;
	overlay->messageTimer = MESSAGE_DISPLAY_TIME;
}
