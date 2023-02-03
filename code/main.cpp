#include "Taburoute.h"
//#include <conio.h>

const int w = 800;
const int h = 800;

extern vector<Nodo> V;

bool checkInteger(string input)
{
    bool isNeg=false;
    int itr=0;
    if(input.size()==0)
        return false;
    if(input[0]=='-')
    {
        isNeg=true;
        itr=1;
		return false;
    }
 
    for(int i=itr;i<input.size();i++)
    {
        if(!isdigit(input[i]))
            return false;
    }
    return true;
}

int main() {

	string istanza;
	string lambda;

	while(true){
		cout << "Type the file name (e.g. Istanza0.txt): ";
		cin >> istanza; 

		std::ifstream file(istanza);
		if (file.is_open()) {
			std::cout << "File " << istanza << " exists and is a valid text file." << std::endl;
			file.close();
			break;
		} else {
			std::cout << "File " << istanza << " does not exist or is not a valid text file." << std::endl;
		}
	}
	
	while(true){
		cout << "How many times the solution is calculated (Lambda)): ";
		cin >> lambda;
		if(checkInteger(lambda)==false)
			 std::cout << "Invalid input." << std::endl;
		else
			break;
	}
	
	//taburoute(9,false);
	taburoute(stoi(lambda), istanza);

	return 0;
}
