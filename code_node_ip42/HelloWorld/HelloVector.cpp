#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
using namespace std;

class numbersClass{
	private: 
		int a;
		int b;
		
	public: 
		int addVal (void) {
			return a + b;
		}
		
		void setAB(int val1, int val2) {
			a = val1;
			b = val2;
		}
			
		numbersClass(int val1, int val2) {
				a = val1;
				b = val2;
			}
		~numbersClass(void) {
				cout << "destroied \n";
			}
		numbersClass() {
				a = 10;
				b = 1;
			}
};

int main() {	
	
	srand(time(NULL));
	
	vector <numbersClass> classVector(5);
	cout << "original lenght " << classVector.size() << endl;
	cout << endl;
	
	for (unsigned int i = 0; i < classVector.size(); i++) {
		classVector.at(i).setAB(rand()%10, rand()%10);
	}
	
	for (unsigned int i = 0; i < classVector.size(); i++) {
		cout << "somewhat random: " << classVector.at(i).addVal() << endl;
	}
	
	cout << endl;
	cout << "bevor erase " << endl;
	classVector.erase(classVector.begin()+3);
	cout << "after erase " << endl;
	
	cout << "lengt after erase " << classVector.size() << endl;
	cout << endl;
	
	for (unsigned int i = 0; i < classVector.size(); i++) {
		cout << "somewhat random: " << classVector.at(i).addVal() << endl;
	}
	
	classVector.push_back(numbersClass(rand()%10, rand()%10));
	
	for (unsigned int i = 0; i < classVector.size(); i++) {
		cout << "somewhat random: " << classVector.at(i).addVal() << endl;
	}
	
	cout << endl;
	 
	cout << "end of program \n" << endl;
	
	
	
	
}
