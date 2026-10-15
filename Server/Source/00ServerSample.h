#pragma once
#include <mrs.hpp>
#include <iostream>
#include <thread>
#include <string>

class ServerSample
{
public:
	void Execute();
	void Exit();

	// 
	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);
	static void Accept(MrsServer server, void* server_data, MrsConnection connection);
	static void Disconnect(MrsConnection connection, void* connection_data);
	static void Error(MrsConnection connection, void* connection_data, MrsConnectionError status);

private:
	MrsServer mrsServer = nullptr;
	bool loop = true;
};

