/*#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include "Person.h"

#define TOKEN ","
#define BlockSize 4096
#define StudentSize 32
#define ProfessorSize 28
#define NumOfStudentInBlock BlockSize/StudentSize
#define NumOfProfessorInBlock BlockSize/ProfessorSize
using namespace std;
int stu_n, prof_n, stu_valid_size, prof_valid_size;
ifstream queryfile("query.dat");
ifstream fin("HashAddressTable.hash", ios_base::in | ios_base::binary);
ofstream res("query.res");
class StudentXProfessor {
public:
	char stu_name[20];
	int stu_id;
	float stu_score;
	int prof_id;
	char prof_name[20];
	int prof_salary;
};
struct HashAddressTableElement {
	int block_num;
	int record_num;
};
HashAddressTableElement* hat_stu;
HashAddressTableElement* hat_prof;
void searchStudent(int key) {
	ifstream fin("student.hash", ios_base::in, ios_base::binary);
	int idx = key % stu_valid_size;
	int blockNumber = hat_stu[idx].block_num;
	int recordNumber = hat_stu[idx].record_num;

	Student* student = new Student[recordNumber];
	fin.seekg(BlockSize*blockNumber);
	fin.read((char*)&student[0], StudentSize *recordNumber);
	fin.close();

	for (int i = 0; i < recordNumber; i++) {
		if (student[i].stu_id == key) {
			res << "Student ID : " << student[i].stu_id << endl;
			res << "Student Name : " << student[i].stu_name << endl;
			res << "Student Advisor ID : " << student[i].stu_advisorid << endl;
			res << "Student Score : " << student[i].stu_score << endl<<endl;
			return;
		}
	}
	res << " No Match" << endl;
}
void searchProfessor(int key) {
	ifstream fin2("professor.hash", ios_base::in, ios_base::binary);
	int idx = key % prof_valid_size;
	int blockNumber = hat_prof[idx].block_num;
	int recordNumber = hat_prof[idx].record_num;

	Professor* professor = new Professor[recordNumber];
	fin2.seekg(BlockSize*blockNumber);
	fin2.read((char*)&professor[0], ProfessorSize *recordNumber);
	fin2.close();

	for (int i = 0; i < recordNumber; i++) {
		if (professor[i].prof_id == key) {
			res << "Professor ID : " << professor[i].prof_id << endl;
			res << "Professor Name : " << professor[i].prof_name << endl;
			res << "Professor Salary : " << professor[i].prof_salary << endl<<endl;
			return;
		}
	}
	res << " No Match" << endl;
}
void search_Exact(string s, string table_name, string attribute, int key) {
	res << "-----Search-Exact Result-----" << endl;
	if (table_name == "Students")
		searchStudent(key);
	else if (table_name == "Professors")
		searchProfessor(key);
}
void naturalJoin() {
	ifstream fin_stu("students.db",ios_base::in | ios_base::binary);
	ifstream fin_prof("professor.db", ios_base::in | ios_base::binary);
	int stu_number, prof_number;
	fin_stu.read((char*)&stu_number, 4);
	fin_prof.read((char*)&prof_number, 4);

	vector<int> stu, prof;
	int remain = stu_n;

	while (remain - NumOfStudentInBlock > 0) {
		stu.push_back(NumOfStudentInBlock);
		remain -= NumOfStudentInBlock;;
	}
	stu.push_back(remain);

	remain = prof_n;

	while (remain - NumOfProfessorInBlock > 0) {
		prof.push_back(NumOfProfessorInBlock);
		remain -= NumOfProfessorInBlock;;
	}
	prof.push_back(remain);
	Professor* professor;
	Student *student;
	res << "----- Join Result -----" << endl;
	for (int i = 0; i < prof.size(); i++) {
		professor = new Professor[prof[i]];
		//StudentXProfessor* sp = new StudentXProfessor();
		fin_prof.seekg(4 + (NumOfProfessorInBlock*ProfessorSize* i));
		fin_prof.read((char*)&professor[0], ProfessorSize*prof[i]);

		for (int j = 0; j < stu.size(); j++) {
			student = new Student[stu[j]];
			fin_stu.seekg(4 + (NumOfStudentInBlock*StudentSize*j));
			fin_stu.read((char*)&student[0], StudentSize*stu[j]);
			
			for (int k = 0; k < prof[i]; k++) {
				for (int l = 0; l < stu[j]; l++) {
					if (student[l].stu_advisorid == professor[k].prof_id) {
						//sp->prof_id = professor[k].prof_id;
						//copy(professor[k].prof_name, professor[k].prof_name + 20, sp->prof_name);
						//sp->prof_salary = professor[k].prof_salary;
						//sp->stu_id = student[l].stu_id;
						//copy(student[l].stu_name, student[l].stu_name + 20, sp->stu_name);
						//sp->stu_score = student[l].stu_score;
						//res.write((char*)&sp[0], sizeof sp[0]);

						res << "Professor Name : " << professor[k].prof_name << endl;
						res << "Professor ID : " << professor[k].prof_id << endl;
						res << "Student Name : " << student[l].stu_name << endl;
						res << "Student ID : " << student[l].stu_id <<endl;
						res << endl;
						
					}
				}
			}
			delete[] student;
		}
		delete[] professor;
		//delete[] sp;

	}

}
char* token;
char *context;
char temp[50];
bool isFirst = true;
void query(string oper) {
	if (oper == "Search-Exact") {
		if (isFirst) {
			fin.read((char*)&stu_n, 4);
			fin.read((char*)&prof_n, 4);
			fin.read((char*)&stu_valid_size, 4);
			fin.read((char*)&prof_valid_size, 4);
			hat_stu = new HashAddressTableElement[stu_valid_size];
			hat_prof = new HashAddressTableElement[prof_valid_size];

			int stu_size = sizeof(hat_stu[0]) * stu_valid_size;
			fin.read((char*)&hat_stu[0], stu_size);
			int prof_size = sizeof(hat_prof[0]) * prof_valid_size;
			fin.read((char*)&hat_prof[0], prof_size);

			isFirst = false;
		}
		
		char tableName [20] ;
		char attributeName[20];
		int key;
		
		
		token = strtok_s(NULL, TOKEN, &context);
		copy(token, token + 20, tableName);
		token = strtok_s(NULL, TOKEN, &context);
		copy(token, token + 20, attributeName);
		token = strtok_s(NULL, TOKEN, &context);
		key= stoul(token, nullptr, 0);
	
		search_Exact(oper, tableName, attributeName, key);
	}
	else if (oper == "Search-Range") {

	}
	else if (oper == "Join") {
	
		naturalJoin();
	}
}

int main() {
	
	
	//query("Search-Exact");
	int n;
	
	char oper[20];
	queryfile >> n;
	queryfile.getline(temp, 50);
	for (int i = 0; i < n; i++) {
		queryfile.getline(temp, 50);
		token = strtok_s(temp, TOKEN, &context);
		copy(token, token + 20, oper);
		query(oper);
	}



	queryfile.close();
	fin.close();
	res.close();
	return 0;
}*/