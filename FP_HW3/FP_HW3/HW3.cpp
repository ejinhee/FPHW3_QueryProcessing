#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Person.h"

#define TOKEN ","
#define stuHashBlockMaxSize 4096/32
#define profHashBlockMaxSize 4096/28
#define BlockSize 4096
#define StudentSize 32
#define ProfessorSize 28

#define Bfactor 509
using namespace std;

int stu_valid_size = 1;
int stu_allocated_block = 0;

int prof_valid_size = 1;
int prof_allocated_block = 0;
ifstream fin_stu("student_data.csv");
ifstream fin_prof("prof_data.csv");

ofstream fout_stu("students.db", ios_base::out | ios_base::binary);
ofstream fout_prof("professor.db", ios_base::out | ios_base::binary);





struct Block {
	int arr[1024] = { 1, };
};

Student getStudent() {
	char temp[50];
	float score;
	int stuid;
	int advid;
	char* token;
	char *context;

	Student stu;

	fin_stu.getline(temp, 50);
	token = strtok_s(temp, TOKEN, &context);
	stu.setName(token);
	token = strtok_s(NULL, TOKEN, &context);
	stuid = stoul(token, nullptr, 0);
	stu.setStuID(stuid);
	token = strtok_s(NULL, TOKEN, &context);
	score = strtof(token, 0);
	stu.setScore(score);
	token = strtok_s(NULL, TOKEN, &context);
	advid = stoul(token, nullptr, 0);
	stu.setAdvisorID(advid);

	fout_stu.write((char*)&stu, sizeof stu);
	return stu;
}
Professor getProfessor() {
	char temp[50];
	int salary;
	int prof_id;
	char* token;
	char* context;

	Professor prof;

	fin_prof.getline(temp, 50);
	token = strtok_s(temp, TOKEN, &context);
	prof.setName(token);
	token = strtok_s(NULL, TOKEN, &context);
	prof_id = stoul(token, nullptr, 0);
	prof.setProfID(prof_id);
	token = strtok_s(NULL, TOKEN, &context);
	salary = stoul(token, nullptr, 0);
	prof.setSalary(salary);

	fout_prof.write((char*)&prof, sizeof prof);

	return prof;
}
struct HashBlock {
	Student stu[128];
};
struct HashAddressTableElement {
	int prefix;
	int block_num;
	int record_num;
};
class NodeElement {
public:
	int blockNumber;
	float score;

	NodeElement(int blockNumber, float score) {
		this->blockNumber = blockNumber;
		this->score = score;
	}
};
class Node {
public:
	
	bool isLeaf;
	int thisblockNumber;
	int elementNumber;
	vector<NodeElement> element;
	int nextNode;
	int lastBlockNumber;

};
bool CompareObj(NodeElement &e1, NodeElement &e2) {
	return e1.score < e2.score;
}
int BtreeAllocatedBlock = 0;
class Btree {
public:
	Node* root;
	Btree() {
		root = new Node();
		root->isLeaf = true;
		root->nextNode = NULL;
		root->thisblockNumber = 0;
	}
	void insert(float score, int blockNumber, ofstream* bout) {
		if (root->isLeaf == true) {

			NodeElement* e = new NodeElement(blockNumber,score);
			root->element.push_back(*e);
			sort(root->element.begin(), root->element.end(), CompareObj);
			if (root->element.size() > Bfactor) {
				Node* node1 = new Node();
				Node* node2 = new Node();
				Node* rootNode = new Node();
				int mid_idx = root->element.size() / 2;

				node1->isLeaf = true;
				node1->thisblockNumber = ++BtreeAllocatedBlock;
				node1->elementNumber = mid_idx;
				node1->nextNode = BtreeAllocatedBlock + 1;
				node1->lastBlockNumber = 0;
				//node1->element
				Block b1, b2;
				bout->seekp(BlockSize* node1->thisblockNumber);
				bout->write((char*)&b1, sizeof b1);

				bout->seekp(BlockSize* node1->thisblockNumber);
				bout->write((char*)&node1->isLeaf, 1);
				bout->seekp(BlockSize* node1->thisblockNumber+1);
				bout->write((char*)&node1->thisblockNumber, 4);
				bout->seekp(BlockSize* node1->thisblockNumber + 5);
				bout->write((char*)&node1->elementNumber, 4);
				
				bout->seekp(BlockSize* node1->thisblockNumber + 9 );
				bout->write((char*)&node1->nextNode, 4);
				bout->seekp(BlockSize* node1->thisblockNumber + 13);
				bout->write((char*)&node1->lastBlockNumber, 4);
				for (int i = 0; i < mid_idx; i++) {
					bout->seekp(BlockSize* node1->thisblockNumber + 17 + (i * 8));
					bout->write((char*)&node1->element[i].blockNumber, 4);
					bout->seekp(BlockSize* node1->thisblockNumber + 17 + (i * 8) + 4);
					bout->write((char*)&node1->element[i].score, 4);
				}

				node2->isLeaf = true;
				node2->thisblockNumber = ++BtreeAllocatedBlock;
				node2->elementNumber = mid_idx;
				node2->nextNode = 0;
				node2->lastBlockNumber = 0;
				//node2->element
		
				bout->seekp(BlockSize* node2->thisblockNumber);
				bout->write((char*)&b2, sizeof b2);

				bout->seekp(BlockSize* node2->thisblockNumber);
				bout->write((char*)&node2->isLeaf, 1);
				bout->seekp(BlockSize* node2->thisblockNumber + 1);
				bout->write((char*)&node2->thisblockNumber, 4);
				bout->seekp(BlockSize* node2->thisblockNumber + 5);
				bout->write((char*)&node2->elementNumber, 4);

				bout->seekp(BlockSize* node2->thisblockNumber + 9);
				bout->write((char*)&node2->nextNode, 4);
				bout->seekp(BlockSize* node2->thisblockNumber + 13);
				bout->write((char*)&node2->lastBlockNumber, 4);
				for (int i = mid_idx; i < root->element.size(); i++) {
					bout->seekp(BlockSize* node2->thisblockNumber + 17 + ( (i-mid_idx) * 8));
					bout->write((char*)&node2->element[i].blockNumber, 4);
					bout->seekp(BlockSize* node2->thisblockNumber + 17 + ( (i-mid_idx) * 8) + 4);
					bout->write((char*)&node2->element[i].score, 4);
				}
				rootNode->isLeaf = false;
				rootNode->elementNumber = 1;
				NodeElement* midEle = new NodeElement(node1->thisblockNumber, rootNode->element[mid_idx].score);
				rootNode->element.push_back(*midEle);
				rootNode->lastBlockNumber = node2->thisblockNumber;
				rootNode->nextNode = 0;
				rootNode->thisblockNumber = 0;
				
				root = rootNode;
				delete node1, node2, rootNode,midEle;
			}

		}
	}
	void resize() {

	}
};
class HashAddressTable {
public:

	HashAddressTableElement *stuHashAddrTable, *profHashAddrTable;

	HashAddressTable() {

		stuHashAddrTable = new HashAddressTableElement[stu_valid_size];
		stuHashAddrTable[0].block_num = 0;
		stuHashAddrTable[0].prefix = 0;
		stuHashAddrTable[0].record_num = 0;
		profHashAddrTable = new HashAddressTableElement[prof_valid_size];
		profHashAddrTable[0].block_num = 0;
		profHashAddrTable[0].prefix = 0;
		profHashAddrTable[0].record_num = 0;
	}


	void stu_resize(ofstream* stu_hash_out) {

		ifstream ftemp("student.hash", ios_base::in | ios_base::binary);
		Student* stu_temp = new Student[stuHashBlockMaxSize];
		int idx; // 꽉찬 원소인덱스
		vector<int> v;
		for (int i = 0; i < stu_valid_size; i++) {
			if (stuHashAddrTable[i].record_num == stuHashBlockMaxSize) {
				v.push_back(i);
			}
		}

		if (v.size() == 1) {
			int fullBlock_num; // 꽉찬 블락넘버

			HashAddressTableElement* newTable = new HashAddressTableElement[stu_valid_size * 2];
			idx = v[0]; // 꽉찬 블락넘버를 가르키는 테이블의 인덱스
			fullBlock_num = stuHashAddrTable[idx].block_num;
			for (int i = 0; i < stu_valid_size; i++) {
				if (i == idx || i == idx + stu_valid_size)
					continue;
				newTable[i] = stuHashAddrTable[i];
				newTable[i + stu_valid_size] = stuHashAddrTable[i];
			}
			//꽉찬 블럭 레코드를 임시로 가져오기


			ftemp.seekg(BlockSize * stuHashAddrTable[idx].block_num);

			ftemp.read((char*)&stu_temp[0], BlockSize);
			ftemp.close();
			//stu_hash_out->open("student.hash", ios_base::out | ios_base::binary);
			Block newBlock1;
			Block newBlock2;
			newTable[idx].prefix = stuHashAddrTable[idx].prefix + 1;
			newTable[idx + stu_valid_size].prefix = newTable[idx].prefix;
			newTable[idx].record_num = 0;
			newTable[idx + stu_valid_size].record_num = 0;
			newTable[idx].block_num = stuHashAddrTable[idx].block_num;
			newTable[idx + stu_valid_size].block_num = stu_allocated_block + 1;
			stu_allocated_block++;

			stu_hash_out->seekp(BlockSize * stuHashAddrTable[idx].block_num, ios::beg);
			stu_hash_out->write((char*)&newBlock1, sizeof newBlock1); //기존 맥스블럭 초기화
			stu_hash_out->seekp(BlockSize * newTable[idx + stu_valid_size].block_num, ios::beg);
			stu_hash_out->write((char*)&newBlock2, sizeof newBlock2); // 새로운블럭할당





			for (int i = 0; i < stuHashBlockMaxSize; i++) {
				if (stu_temp[i].stu_id & (int)pow(2.0, stuHashAddrTable[idx].prefix)) {
					stu_hash_out->seekp((BlockSize * newTable[idx + stu_valid_size].block_num) + (StudentSize * newTable[idx + stu_valid_size].record_num), ios::beg);
					stu_hash_out->write((char*)&stu_temp[i], sizeof stu_temp[i]);
					newTable[idx + stu_valid_size].record_num++;
				}
				else {
					stu_hash_out->seekp((BlockSize * newTable[idx].block_num) + (StudentSize * newTable[idx].record_num), ios::beg);
					stu_hash_out->write((char*)&stu_temp[i], sizeof stu_temp[i]);
					newTable[idx].record_num++;
				}
			}

			//cout << stu_allocated_block << " ";
			delete[] stuHashAddrTable;
			stuHashAddrTable = newTable;


			stu_valid_size *= 2;

		}

		else {
			idx = v[0];
			int fullBlockNum = stuHashAddrTable[v[0]].block_num;
			Block newBlock3, newBlock4;

			for (int i = 0; i<v.size(); i++) {
				if (!(i % 2))
					stuHashAddrTable[v[i]].block_num = fullBlockNum;
				else {

					stuHashAddrTable[v[i]].block_num = stu_allocated_block + 1;

					//cout << stu_allocated_block << " ";
				}
				stuHashAddrTable[v[i]].prefix++;
				stuHashAddrTable[v[i]].record_num = 0;
			}

			stu_allocated_block++;


			ftemp.seekg(BlockSize * stuHashAddrTable[idx].block_num, ios::beg);

			ftemp.read((char*)&stu_temp[0], BlockSize);
			ftemp.close();

			//stu_hash_out->open("student.hash", ios_base::out | ios_base::binary);

			stu_hash_out->seekp(BlockSize * stuHashAddrTable[v[0]].block_num, ios::beg);
			stu_hash_out->write((char*)&newBlock3, sizeof newBlock3); //기존 맥스블럭 초기화
			stu_hash_out->seekp(BlockSize * stuHashAddrTable[v[1]].block_num, ios::beg);
			stu_hash_out->write((char*)&newBlock4, sizeof newBlock4); // 새로운블럭할당


			for (int i = 0; i < stuHashBlockMaxSize; i++) {
				if (stu_temp[i].stu_id & (int)pow(2.0, stuHashAddrTable[v[0]].prefix - 1)) {
					stu_hash_out->seekp((BlockSize * stuHashAddrTable[v[1]].block_num) + (StudentSize * stuHashAddrTable[v[1]].record_num), ios::beg);
					stu_hash_out->write((char*)&stu_temp[i], sizeof stu_temp[i]);
					stuHashAddrTable[v[1]].record_num++;
				}
				else {
					stu_hash_out->seekp((BlockSize * stuHashAddrTable[v[0]].block_num) + (StudentSize * stuHashAddrTable[v[0]].record_num), ios::beg);
					stu_hash_out->write((char*)&stu_temp[i], sizeof stu_temp[i]);
					stuHashAddrTable[v[0]].record_num++;
				}
			}



		}
		delete[] stu_temp;
		

	}
	void prof_resize(ofstream* prof_hash_out) {

		ifstream ftemp("professor.hash", ios_base::in | ios_base::binary);
		Professor* prof_temp = new Professor[profHashBlockMaxSize];
		int idx; // 꽉찬 원소인덱스
		vector<int> v;
		for (int i = 0; i < prof_valid_size; i++) {
			if (profHashAddrTable[i].record_num == profHashBlockMaxSize) {
				v.push_back(i);
			}
		}

		if (v.size() == 1) {
			int fullBlock_num; // 꽉찬 블락넘버

			HashAddressTableElement* newTable = new HashAddressTableElement[prof_valid_size * 2];
			idx = v[0]; // 꽉찬 블락넘버를 가르키는 테이블의 인덱스
			fullBlock_num = profHashAddrTable[idx].block_num;
			for (int i = 0; i < prof_valid_size; i++) {
				if (i == idx || i == idx + prof_valid_size)
					continue;
				newTable[i] = profHashAddrTable[i];
				newTable[i + prof_valid_size] = profHashAddrTable[i];
			}
			//꽉찬 블럭 레코드를 임시로 가져오기


			ftemp.seekg(BlockSize * profHashAddrTable[idx].block_num);

			ftemp.read((char*)&prof_temp[0], profHashBlockMaxSize * ProfessorSize);
			ftemp.close();
			//prof_hash_out->open("professor.hash", ios_base::out | ios_base::binary);
			Block newBlock1;
			Block newBlock2;
			newTable[idx].prefix = profHashAddrTable[idx].prefix + 1;
			newTable[idx + prof_valid_size].prefix = newTable[idx].prefix;
			newTable[idx].record_num = 0;
			newTable[idx + prof_valid_size].record_num = 0;
			newTable[idx].block_num = profHashAddrTable[idx].block_num;
			newTable[idx + prof_valid_size].block_num = prof_allocated_block + 1;
			prof_allocated_block++;

			prof_hash_out->seekp(BlockSize * profHashAddrTable[idx].block_num, ios::beg);
			prof_hash_out->write((char*)&newBlock1, sizeof newBlock1); //기존 맥스블럭 초기화
			prof_hash_out->seekp(BlockSize * newTable[idx + prof_valid_size].block_num, ios::beg);
			prof_hash_out->write((char*)&newBlock2, sizeof newBlock2); // 새로운블럭할당





			for (int i = 0; i < profHashBlockMaxSize; i++) {
				if (prof_temp[i].prof_id & (int)pow(2.0, profHashAddrTable[idx].prefix)) {
					prof_hash_out->seekp((BlockSize * newTable[idx + prof_valid_size].block_num) + (ProfessorSize * newTable[idx + prof_valid_size].record_num), ios::beg);
					prof_hash_out->write((char*)&prof_temp[i], sizeof prof_temp[i]);
					newTable[idx + prof_valid_size].record_num++;
				}
				else {
					prof_hash_out->seekp((BlockSize * newTable[idx].block_num) + (ProfessorSize * newTable[idx].record_num), ios::beg);
					prof_hash_out->write((char*)&prof_temp[i], sizeof prof_temp[i]);
					newTable[idx].record_num++;
				}
			}

			//cout << prof_allocated_block << " ";
			delete[] profHashAddrTable;
			profHashAddrTable = newTable;


			prof_valid_size *= 2;

		}

		else {
			idx = v[0];
			int fullBlockNum = profHashAddrTable[v[0]].block_num;
			Block newBlock3, newBlock4;

			for (int i = 0; i<v.size(); i++) {
				if (!(i % 2))
					profHashAddrTable[v[i]].block_num = fullBlockNum;
				else {

					profHashAddrTable[v[i]].block_num = prof_allocated_block + 1;

					//cout << prof_allocated_block << " ";
				}
				profHashAddrTable[v[i]].prefix++;
				profHashAddrTable[v[i]].record_num = 0;
			}

			prof_allocated_block++;


			ftemp.seekg(BlockSize * profHashAddrTable[idx].block_num, ios::beg);

			ftemp.read((char*)&prof_temp[0], profHashBlockMaxSize * ProfessorSize);
			ftemp.close();

			//prof_hash_out->open("Professor.hash", ios_base::out | ios_base::binary);

			prof_hash_out->seekp(BlockSize * profHashAddrTable[v[0]].block_num, ios::beg);
			prof_hash_out->write((char*)&newBlock3, sizeof newBlock3); //기존 맥스블럭 초기화
			prof_hash_out->seekp(BlockSize * profHashAddrTable[v[1]].block_num, ios::beg);
			prof_hash_out->write((char*)&newBlock4, sizeof newBlock4); // 새로운블럭할당


			for (int i = 0; i < profHashBlockMaxSize; i++) {
				if (prof_temp[i].prof_id & (int)pow(2.0, profHashAddrTable[v[0]].prefix - 1)) {
					prof_hash_out->seekp((BlockSize * profHashAddrTable[v[1]].block_num) + (ProfessorSize * profHashAddrTable[v[1]].record_num), ios::beg);
					prof_hash_out->write((char*)&prof_temp[i], sizeof prof_temp[i]);
					profHashAddrTable[v[1]].record_num++;
				}
				else {
					prof_hash_out->seekp((BlockSize * profHashAddrTable[v[0]].block_num) + (ProfessorSize * profHashAddrTable[v[0]].record_num), ios::beg);
					prof_hash_out->write((char*)&prof_temp[i], sizeof prof_temp[i]);
					profHashAddrTable[v[0]].record_num++;
				}
			}



		}
		delete[] prof_temp;

	}
};
bool stuHashInsert(Student* stu2, HashAddressTable* addr_table, ofstream* stu_hash_out) {

	Student stu;
	stu = *stu2;

	int idx = stu.stu_id % stu_valid_size;
	int block_number = addr_table->stuHashAddrTable[idx].block_num;
	int numOfRecord = addr_table->stuHashAddrTable[idx].record_num;

	stu_hash_out->seekp((BlockSize * addr_table->stuHashAddrTable[idx].block_num) + (StudentSize * addr_table->stuHashAddrTable[idx].record_num), ios::beg);
	stu_hash_out->write((char*)&stu, sizeof stu);
	for (int i = 0; i < stu_valid_size; i++) {
		if (addr_table->stuHashAddrTable[idx].block_num == addr_table->stuHashAddrTable[i].block_num)
			addr_table->stuHashAddrTable[i].record_num++;
	}
	if (addr_table->stuHashAddrTable[idx].record_num == stuHashBlockMaxSize)
		return true; // 블락 꽉차면
	return false;
}
bool profHashInsert(Professor* prof2, HashAddressTable* addr_table, ofstream* prof_hash_out) {
	Professor prof;
	prof = *prof2;

	int idx = prof.prof_id % prof_valid_size;
	int block_number = addr_table->profHashAddrTable[idx].block_num;
	int numOfRecord = addr_table->profHashAddrTable[idx].record_num;
	prof_hash_out->seekp((BlockSize * addr_table->profHashAddrTable[idx].block_num) + (ProfessorSize * addr_table->profHashAddrTable[idx].record_num), ios::beg);
	prof_hash_out->write((char*)&prof, sizeof prof);
	for (int i = 0; i < prof_valid_size; i++) {
		if (addr_table->profHashAddrTable[idx].block_num == addr_table->profHashAddrTable[i].block_num)
			addr_table->profHashAddrTable[i].record_num++;
	}
	if (addr_table->profHashAddrTable[idx].record_num == profHashBlockMaxSize)
		return true; // 블락 꽉차면
	return false;
}
HashAddressTable* addr_table = new HashAddressTable();


int main() {
	int stu_n;
	int prof_n;
	char newline[50];
	Student stu;
	Professor prof;

	fin_stu >> stu_n;
	fin_prof >> prof_n;
	fin_stu.getline(newline, 50);
	fin_prof.getline(newline, 50);
	//for (int i = 0; i < prof_n; i++) {
	//	prof = getProfessor();
	//}


	fout_stu.write((char*)&stu_n, 4);
	fout_prof.write((char*)&prof_n, 4);
	ofstream* stu_hash_out = new ofstream();
	ofstream* prof_hash_out = new ofstream();
	stu_hash_out->open("student.hash", ios_base::out | ios_base::binary);
	prof_hash_out->open("professor.hash", ios_base::out | ios_base::binary);
	Block b, b1;

	stu_hash_out->write((char*)&b, sizeof b);
	prof_hash_out->write((char*)&b1, sizeof b1);
	bool resize_check;
	for (int i = 0; i < 100; i++) {

		stu = getStudent();
		resize_check = stuHashInsert(&stu, addr_table, stu_hash_out);
		if (resize_check) {
			addr_table->stu_resize(stu_hash_out);
			//stu_hash_out->open("student.hash", ios_base::out | ios_base::binary);
		}
	}
	for (int i = 0; i < 100; i++) {
		prof = getProfessor();
		resize_check = profHashInsert(&prof, addr_table, prof_hash_out);
		if (resize_check) {
			addr_table->prof_resize(prof_hash_out);

		}
	}
	stu_hash_out->close();
	prof_hash_out->close();
	ifstream student_hash("student.hash", ios_base::in | ios_base::binary);
	ifstream professor_hash("professor.hash", ios_base::in | ios_base::binary);
	Professor p;
	professor_hash.seekg(56);
	professor_hash.read((char*)&p, sizeof p);
	

	
	ofstream faddr("HashAddressTable.hash", ios_base::out | ios_base::binary);
	faddr.write((char*)&stu_n, 4);
	faddr.write((char*)&prof_n, 4);
	faddr.write((char*)&stu_valid_size,4);
	faddr.write((char*)&prof_valid_size,4);
	for (int i = 0; i < stu_valid_size; i++) {
		faddr.write((char*)&addr_table->stuHashAddrTable[i].block_num,4);
		faddr.write((char*)&addr_table->stuHashAddrTable[i].record_num, 4);
	}
	for (int i = 0; i < prof_valid_size; i++) {
		faddr.write((char*)&addr_table->profHashAddrTable[i].block_num, 4);
		faddr.write((char*)&addr_table->profHashAddrTable[i].record_num, 4);
	}

	ofstream* bTreeOut = new ofstream();
	bTreeOut->open("student.idx", ios_base::out | ios_base::binary);
	Block bb;
	bTreeOut->write((char*)&bb, sizeof bb);

	
	fin_stu.close();
	fin_prof.close();
	fout_stu.close();
	fout_prof.close();	
	delete stu_hash_out;
	delete prof_hash_out;
	delete addr_table;
	
	return 0;

}