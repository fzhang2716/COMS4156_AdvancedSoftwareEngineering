#include<iostream>
#include<vector>
#include<string>

using namespace std;

int main()
{
    vector<string> list = {"Hello", "World", "4156", "Project"};
    for (auto& each: list) {
        cout << each << " ";
    }
    cout << endl;
}