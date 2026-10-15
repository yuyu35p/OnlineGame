#pragma once
#include <mrs.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <vector>

class ServerAssignment01
{
public:
	void Execute();
	void Exit();


	// コールバック関数
	// データ受信時
	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);
	// コネクション切断時
	static void Disconnect(MrsConnection connection, void* connection_data);
	// コネクション接続時
	static void Accept(MrsServer server, void* server_data, MrsConnection connection);
	// エラー
	static void Error(MrsConnection connection, void* connection_data, MrsConnectionError status);

private:
	MrsServer mrsServer = nullptr;
	bool loop = true;
	std::vector<MrsConnection> connection; // 接続中クライアント
};