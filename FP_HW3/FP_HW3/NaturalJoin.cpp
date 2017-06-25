/*#include <iostream>
#include <fstream>
#include <vector>
#include "Person.h"

#define BlockSize 4096
#define StudentSize 32
#define NumOfStudentInBlock BlockSize/StudentSize
#define ProfessorSize 28
#define NumOfProfessorInBlock BlockSize/ProfessorSize
using namespace std;

class StudentXProfessor {
public:
	char stu_name[20];
	int stu_id;
	float stu_score;
	int prof_id;
	char prof_name[20];
	int prof_salary;
};
int main() {
	ifstream fin_stu("students.db");
	ifstream fin_prof("professor.db");
	ofstream fout("")
	int stu_n, prof_n;
	fin_stu.read((char*)&stu_n, 4);
	fin_prof.read((char*)&prof_n, 4);
	
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
	
	for (int i = 0; i < stu.size(); i++) {
		Student* student = new Student[stu[i]];
		fin_stu.seekg(4 + (NumOfStudentInBlock*StudentSize* i));
		fin_stu.read((char*)&student[0], StudentSize*stu[i]);
		for (int j = 0; j < prof.size(); j++) {
			Professor* professor = new Professor[prof[j]];
			fin_prof.seekg(4 + (NumOfProfessorInBlock*ProfessorSize*j));
			fin_prof.read((char*)&professor[0], ProfessorSize*prof[j]);
		}
	}
	
	
	
	return 0;

}*/