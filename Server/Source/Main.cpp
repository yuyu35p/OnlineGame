#include "00ServerSample.h"
#include "01ServerAssignment.h"
#include "02ServerAssignment.h"
#include "03ServerAssignment.h"
#include "04ServerAssignment.h"
#include "05ServerAssignment.h"
#include "06ServerAssignment.h"
#include "07ServerAssignment.h"
#include "08ServerAssignment.h"
#include "09ServerAssignment.h"
#include <iostream>
#include <thread>
#include <string>
#include <iostream>


int main()
{
	bool loop = true;
	do
	{
		std::cout << "00：MRS Sample" << std::endl;
		std::cout << "01：MRSサーバと通信" << std::endl;
		std::cout << "02：MRSキャラクター移動" << std::endl;
		std::cout << "03：MRSデータの判別" << std::endl;
		std::cout << "04：MRS複数ユーザとの通信" << std::endl;
		std::cout << "05：Winsockサーバと接続" << std::endl;
		std::cout << "06：Winsockサーバと通信" << std::endl;
		std::cout << "07：Winsockデータの判別" << std::endl;
		std::cout << "08：Winsock複数ユーザとの通信" << std::endl;
		std::cout << "09：Winsockノンブロッキング処理" << std::endl;
		std::cout << "10：サーバ側なし" << std::endl;
		std::cout << "99：サーバ終了" << std::endl;
		std::cout << "課題番号を入力してください。" << std::endl;
		int input = 0;
		std::cin >> input;
		switch (input)
		{
		case 0:
		{
			ServerSample sample = ServerSample();
			sample.Execute();
			break;
		}
		case 1:
		{
			ServerAssignment01 serverAssignment01 = ServerAssignment01();
			serverAssignment01.Execute();
			break;
		}
		case 2:
		{
			ServerAssignment02 serverAssignment02 = ServerAssignment02();
			serverAssignment02.Execute();
			break;
		}
		case 3:
		{
			ServerAssignment03 serverAssignment03 = ServerAssignment03();
			serverAssignment03.Execute();
			break;
		}
		case 4:
		{
			ServerAssignment04 serverAssignment04 = ServerAssignment04();
			serverAssignment04.Execute();
			break;
		}
		case 5:
		{
			ServerAssignment05 serverAssignment05 = ServerAssignment05();
			serverAssignment05.Execute();
			break;
		}
		case 6:
		{
			ServerAssignment06 serverAssignment06 = ServerAssignment06();
			serverAssignment06.Execute();
			break;
		}
		case 7:
		{
			ServerAssignment07 serverAssignment07 = ServerAssignment07();
			serverAssignment07.Execute();
			break;
		}
		case 8:
		{
			ServerAssignment08 serverAssignment08 = ServerAssignment08();
			serverAssignment08.Execute();
			break;
		}
		case 9:
		{
			ServerAssignment09 serverAssignment09 = ServerAssignment09();
			serverAssignment09.Execute();
			break;
		}
		case 10:
		{
			// 課題10はwebサーバへアクセスするためプログラムは不要
			break;
		}
		case 99:
		{
			loop = false;
			break;
		}
		}
	} while (loop);
}