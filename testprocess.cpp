#include <bits/stdc++.h>
#include<iostream>
#include <fstream>
#include<cmath>
#include<ctime>
#include<cstdlib>
using namespace std;
#include <windows.h>
#include<vector>
#include<cstring>
vector <string> pathval;

void process(string pro){
STARTUPINFO si;
PROCESS_INFORMATION pi;
ZeroMemory(&si, sizeof(si));
si.cb = sizeof(si);
if(!CreateProcess(pro.c_str(),NULL,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi))
	for(int i=0;i<pathval.size();i++){
	string str=pathval[i]+pro;
	if(CreateProcess(str.c_str(),NULL,NULL,NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,&si,&pi))break;
}
WaitForSingleObject(pi.hProcess,INFINITE);
TerminateProcess(pi.hProcess, 0);
CloseHandle(pi.hProcess);
CloseHandle(pi.hThread);
}
void path(){
	cout<<"PATH:";
	int i;
	for(i=0;i<pathval.size()-1;i++)cout<<pathval[i]<<";";
	cout<<pathval[i]<<endl;
}
void pathadd(string str){
	pathval.push_back(str);
}
int main(){
pathval.clear();
pathval.push_back("child/");
process("countdown.exe");
process("calc.exe");
return 0;
}
