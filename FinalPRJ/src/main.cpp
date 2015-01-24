#include <iostream>
#include "CPDSService.h"

using namespace std;

int main() {
	// init CPDS
	CPDSService* CPDS = new CPDSService();
	while (true) {
		string msg;
		cin >> msg;
		if (msg == "x") {
			break;
		}
	}
	CPDS->~CPDSService();
	return 0;
}
