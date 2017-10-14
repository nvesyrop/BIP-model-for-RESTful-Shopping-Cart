#ifndef CP_HPP
#define CP_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <HelloPackageStru.hpp>
#include <boost/lexical_cast.hpp>
#include <stdarg.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

using namespace std;

void printQI(QI* t) {
	cout << *t;
}

void my_print(const std::string message1, const std::string message2) {
	std::cout << message1 << " " << message2;
}
void my_print(const int message1, const std::string message2) {
	std::cout << message1 << " " << message2;
}

int const_compareStrings(const string& message1, const string& message2) {
	return message1.compare(message2);
}

void QI::marshalling(char* c) const {
	/*cout<<'\n'<<this<<" QI_marshalling "<<endl;*/
	std::ostringstream os;
	this->printItem(os);
	strncpy(c, os.str().c_str(), os.str().size());
	/*		cout<<'\n'<<this<<" QI_marshalled to "<<c<<endl;*/
}
;
long QI::size() const {
	std::ostringstream os;
	this->printItem(os);
	/*cout<<this<<" QI size ";
	 cout<<os.str().size()<<endl;*/
	return os.str().size();
}
;

string* stringStar(string s) {
	return new string(s);
}

void setStringStar(string*& sptr, const string s) {
	*sptr = s;
}
string* setNewStringStar(string*& sptr, const string s) {
	delete (sptr);
	return stringStar(s);
}

/*
 * Appends in a string all parameters in q
 */
string serialize(string& s, const StringQueue q) {
	list<string*>::const_iterator iter, end;

	forlist(q.Q)
	{
		if (s.compare("") != 0) {
			s += ";";
		}
		s += **iter;
	}
	return s;
}
string serialize(string& s, const string q) {
	if (s.compare("") != 0) {
		s += ";";
	}
	return s += q;
}

string getRequestedOperation(StringQueue& parametersReqIn) {
	return getQueue(parametersReqIn, 0);
}
int getRequestedBinding(StringQueue& parametersReqIn) {
	return string2int(getQueue(parametersReqIn, 1));
}
string abstractMatchTask(string& inputMsg) {
	string task = "";
	// the first string of the input msg

	const char *c = inputMsg.c_str();
	//till the first ,
	while ((*c) != ';') {
		task.push_back(*c);
		c++;
	}
	return task;
}

/*
 * Extracts parameters (as strings) out of a string and places them in Queue q
 */
StringQueue deserialize(StringQueue& q, string& s) {
	string *ss = stringStar("");

	const char *c = s.c_str();
	while ((*c) != '\0') {
		if ((*c) == ';') {
			add(ss, q);
			ss = stringStar("");
		} else {
			(*ss).push_back(*c);
			//cout << " append " << *c<< " = "<<*ss;
		}
		c++;
	}
	add(ss, q);
	//delete ss;
	return q;
}

/*
 * Extracts parameters (as strings) out of a string and places them in Queue q
 */
void addToStringQueue(string s, AQueue<string>& q) {
	string *ss = stringStar(s);
	add(ss, q);

//	const char *c = s.c_str();
//
//	while ((*c) != '\0') {
//		if ((*c) == ';') {
//			add(ss, q);
//			ss = stringStar("");
//		} else {
//			(*ss).push_back(*c);
//			//cout << " append " << *c<< " = "<<*ss;
//		}
//		c++;
//	}
//	add(ss, q);
	return;
}

/******************** QI ******************/

Conn* connStar(const string& s1, const string& s2, const int& c, const int& s,
		const string& t) {
	return new Conn(s1, s2, c, s, t);
}

std::ostream& operator<<(std::ostream& os, const QI& q) {
	q.printItem(os);
	return os;
}
/************ QQueue *********************/

void printQQueue(const QQueue& q) {
	cout << '\n' << " printQQueue " << q << '\n';
}

void printStringQueue(const StringQueue &q) {
	cout << '\n' << " printStringQueue " << q << '\n';
}
/**
 * Returns the first available id.
 * NOTE: If there is not an available id, returns -1
 */
int getNextId(const QQueue& q, const int& maxId) {
	int n = -1;
	bool exists;
	for (int c = 0; c < maxId; c++) {
		list<QI*>::const_iterator iter, end;
		exists = false;
		forlist(q.Q)
		{
			QI* it = (QI*) *iter;
			if ((it->id) == c) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			n = c;
			break;
		}
	}
	/*cout << " next id " << n << '\n';*/
	return n;
}
void QQueue::marshalling(char* c) {
	/*cout<<'\n'<<*this<<" marshalling ";*/
	std::ostringstream os;
	this->printItem(os);
	strncpy(c, os.str().c_str(), os.str().size());
	/*cout<<'\n'<<*this<<" marshalled to "<<c<<endl;*/
}

long QQueue::size() const {
	std::ostringstream os;
	this->printItem(os);
	return os.str().size();
}

/******** Conn operations ******************/

/*
 * returns the number of bytes (characters) of the textual representation of q
 */

/***************** QUEUE **************************/

void QQueue::printItem(std::ostream& os) const {
	os << "[";
	list<QI*>::const_iterator iter, end;
	forlist (this->Q)
	{
		if (iter != this->Q.begin()) {
			os << ",";
		}
		QI* qi = (QI*) (*iter);
		os << *qi;
	}
	os << "]";
}
std::ostream& operator<<(std::ostream& os, const QQueue& q) {
	q.printItem(os);
	return os;
}

/****************** CONN ********************************/

void Conn::unmarshalling(const char* c) {
	//cout<<'\n'<<c<<" Conn_unmarshalling ";
	this->address1 = this->address2 = this->create_time = "";

	clearQueue(this->reqMsg);
	clearQueue(this->respMsg);

	if (*c != '[') {
		//cout<<'\n'<<c<<" Conn_unmarshalled to "<<this;
		return;
	}
	int par = 0, j = 1;
	stringstream os1, os2, os4;

	while (c[j] != ']') {
		if (c[j] == ']') {
			break;
		} else if (c[j] == ',') {
			par++;
		} else if (par == 0) {
			this->address1.push_back(c[j]);
		} else if (par == 1) {
			this->address2.push_back(c[j]);
		} else if (par == 2) {
			os1 << c[j];
		} else if (par == 3) {
			os2 << c[j];
		} else if (par == 4) {
			this->create_time.push_back(c[j]);
		} else if (par == 5) {
			os4 << c[j];
		} else if (par == 6) {
			StringQueue_unmarshalling(&(this->reqMsg), c + j);
			int size = StringQueue_sizeof(this->reqMsg) - 1;
			/*cout << this->reqMsg << endl;*/
			j += size;
		} else if (par == 7) {
			StringQueue_unmarshalling(&(this->respMsg), c + j);
			j += (StringQueue_sizeof(this->respMsg) - 1);
		}
		j++;
	}
	/*cout<<os1.str()<<" // "<<os2.str()<<" // "<<os4.str()<<endl;*/
	this->id = boost::lexical_cast<int>(os1.str());
	this->status = boost::lexical_cast<int>(os2.str());
	this->time = boost::lexical_cast<int>(os4.str());
	//cout<<'\n'<<c<<" Conn_unmarshalled to "<<*this;
}

void Conn::printItem(std::ostream& os) const {
	os << "[" << this->address1 << "," << this->address2 << "," << this->id
			<< "," << this->status << "," << this->create_time << ","
			<< this->time << "," << this->reqMsg << "," << this->respMsg << "]";
}

/******************* SESS2REPL *******************************/

void Sess2Repl::printItem(std::ostream& os) const {
	os << "[" << this->id << "," << this->time << "," << this->replicaAddress
			<< "]";
}

void printSess2Repl(Sess2Repl *t) {
	t->printItem(cout);
}

void Sess2Repl::unmarshalling(const char* c) {
	//cout<<endl<<c<<" Sess2Repl unmarshalling ... "<<endl;

	this->replicaAddress = "";
	this->id = -1;
	this->time = 0;
	if (*c != '[') {
		return;
	}
	int par = 0, j = 1; //passimg '['
	stringstream osid;
	stringstream ostime;
	while (true) {
		if (c[j] == ']') {
			break;
		} else if (c[j] == ',') {
			par++;
		} else if (par == 0) {
			osid << c[j];
		} else if (par == 1) {
			ostime << c[j];
		} else if (par == 2) {
			this->replicaAddress.push_back(c[j]);
		}
		j++;
	}
	/*cout<<ostime.str()<<" // "<<osid.str()<<endl;*/
	this->time = boost::lexical_cast<int>(ostime.str());
	this->id = boost::lexical_cast<int>(osid.str());
	//cout<<" Sess2Repl unmarshalled to ... "<<*this;
}

void Sess2Repl_unmarshalling(Sess2Repl* t, const char* c) {
	t->unmarshalling(c);
}
long Sess2Repl_sizeof(const Sess2Repl& t) {
	return t.size();
}
void Sess2Repl_marshalling(char* c, const Sess2Repl* t) {
	t->marshalling(c);
}

void Sess2ReplStar_unmarshalling(Sess2ReplStar* t, const char* c) {
	(*t)->unmarshalling(c);
}
long Sess2ReplStar_sizeof(const Sess2ReplStar& t) {
	return (*t).size();
}
void Sess2ReplStar_marshalling(char* c, const Sess2ReplStar* t) {
	(*t)->marshalling(c);
}
long Sess2ReplQueue_sizeof(const QQueue& q) {
	return q.size();
}
void Sess2ReplQueue_marshalling(char* c, const QQueue* q) {
	//cout<<endl<<*q<<" Sess2ReplQueue_marshalling ... "<<endl;
	QQueue qq = *q;
	qq.marshalling(c);
	//cout<<*q<<" Sess2ReplQueue_marshalled "<<c;
}
void Sess2ReplQueue_unmarshalling(QQueue* t, const char* c) {
	//cout<<endl<<*t<<" Sess2ReplQueue_unmarshalling ... "<<endl;
	clearQueue(*t);

	if (*c != '[' || *(c + 1) == ']') {
		return;
	}
	Sess2Repl *conn;
	while (*c != ']') {
		c++; //for initial [ or ,
		conn = sess2ReplStar();
		conn->unmarshalling(c);
		add(conn, *t);
		c += conn->size();
	}
	//cout<<*t<<" Sess2ReplQueue_unmarshalled to "<<c;
}

/******************* SESS2REPL *** end *************************/

/*
 * Adds every msg in msgParams to the request or response message queue of connection
 */

void addMsgConn(ConnStar& conn, const string& address, StringQueue& msgParams) {
	list<string*>::const_iterator iter, end;
	if (const_compareStrings(address, conn->address1) == 0) {
		forlist(msgParams.Q)
		{
			string *s = *iter;
			//cout<< s;
			add(s, conn->reqMsg);
		}
		//cout << '\n' << "addMsgConn " << conn->reqMsg;
	} else {
		forlist(msgParams.Q)
		{
			string *s = *iter;
			//cout<< s;
			add(s, conn->respMsg);
		}
		//cout << '\n' << "addMsgConn " << conn->respMsg;
	}

}

bool hasFirstMsgConn(ConnStar& conn, const string& address, const string& msg) {
	if (const_compareStrings(address, conn->address2) == 0) {
		//string msg = *(conn.reqMsg.Q.begin());
		if (conn->reqMsg.Q.front()->compare(msg)) {
			return true;
		} else {
			return false;
		}
	} else {
		//string msg = *(conn.respMsg.Q.begin());
		if (conn->respMsg.Q.front()->compare(msg)) {
			return true;
		} else {
			return false;
		}
	}
}

/*
 * Remove the first message from the msgqueue of the conn (:id)
 * which has the same address (:string)
 */
string rmvMsgConn(ConnStar& conn, const string& address) {
	if (const_compareStrings(address, conn->address2) == 0) {
		//string msg = *(conn.reqMsg.Q.begin());
		return removeFirst(conn->reqMsg);
	} else {
		//string msg = *(conn.respMsg.Q.begin());
		return removeFirst(conn->respMsg);
	}
}

string getFromDataResultItem(QI* conn, const int pos) {
	return getQueue(((Conn*) conn)->reqMsg, pos);
}

void onePartyClosed(QQueue& q, const int& c, const string& create_time) {
	ConnStar conn = getFirstConnStatus(q, c, -1, create_time);
	if (++(conn->status) == 6) {
		removeConn(q, c, create_time);
	}
}

/**
 * Removes the id in q with c (:id)
 */
void removeConn(QQueue& q, const int& c, const string& create_time) {
	list<QI*>::iterator iter, end;
	forlist(q.Q)
	{
		if (const_isConn((Conn*) *iter, "", "", c, -1, create_time)) {
			q.Q.erase(iter);
			return;
		}
	}
}

std::ostream& operator<<(std::ostream& os, const Conn& q) {
	q.printItem(os);
	return os;
}

std::ostream& operator<<(std::ostream& os, const ConnStar& q) {
	return os << *q;
}

/*************** ConnQueue**********************/
/**
 * Returns the first id in the queue with a specified status
 * NOTE: must be called only if exists such id
 * @param QQueue& q: a queue with Conn objects
 * @param int& S: the status of the id in search
 * @return Conn: the found id or the null id
 *
 */
ConnStar getFirstStatus(const QQueue& q, const int& s) {
	list<QI*>::const_iterator iter = getFirstStatusIter(q, s);
	Conn* it = (Conn*) *iter;
	return it;
}

list<QI*>::const_iterator getFirstStatusIter(const QQueue& q, const int& s) {
	list<QI*>::const_iterator iter, end;
	Conn *it;
	forlist (q.Q)
	{
		it = (Conn*) *iter;
		if (it->status == s) {
			return iter;
		}
	}
	return iter;
}
/**
 * Returns the number of ids in connQueueInMB with same target address as id connInNetwork
 */
int const_ConnStringQueueSize(ConnStar& connInNetwork, QQueue& connQueueInMB) {
	//cout<<'\n'<<connInNetwork<<" : "<<connQueueInMB;
	list<QI*>::const_iterator iter, end;
	int count = 0;
	forlist(connQueueInMB.Q)
	{
		Conn *it = (Conn*) *iter;
		if (((it->address2).compare(connInNetwork->address2)) == 0) {
			count++;
		}
	}
	//cout<< " ConnStringQueueSize "<<count;
	//cout<<" size "<<const_size(connQueueInMB);
	return count;
}

bool connEquals(const ConnStar conn1, const ConnStar conn2) {
	if (conn1->id == conn2->id
			&& conn1->create_time.compare(conn2->create_time) == 0) {
		return true;
	}
	return false;
}

/*
 * Sets the status of the id (identical to conn1) in queue
 */
QQueue& setConnInQueueStatus(QQueue& q, const ConnStar& conn1, const int& s) {
	list<QI*>::iterator iter, end;
	forlist (q.Q)
	{
		Conn *it = (Conn*) *iter;

		if (connEquals(conn1, it)) {
			//Conn *p = new Conn();
			//add(p,q);
			//Conn *cptr = &(*it);
			(*it).status = s;
			//cout<<" found "<<*it<<" but ";
			//cout<<q;
			return q;
		}
	}
	return q;
}

/**
 * Returns if AQueue<string> contains the connections AddressTo (is a string)
 */
bool const_isConnTarget(const ConnStar& conn, const AQueue<string>& addrQueue) {

	list<string*>::const_iterator iter, end;
	//printConn(conn);
	//cout<<addrQueue.Q.size();
	forlist (addrQueue.Q)
	{
		//cout<<*iter;
		if ((**iter).compare(conn->address2) == 0) {
			//cout<< addrQueue<<" true";
			return true;
		}
	}
	//cout<< addrQueue<<" false";
	return false;
}

void printConn(const ConnStar& conn) {
	(*conn).printItem(std::cout);
}

/**
 * Returns if the queue contains connection with a status
 * @param QQueue& q: a queue with Conn objects
 * @param int& S: the status of the connection in search
 * @return bool: if connection with status is found
 */
bool const_hasStatus(const QQueue& q, const int& s) {
	list<QI*>::const_iterator iter = getFirstStatusIter(q, s);

	if (iter == q.Q.end()) { //if conn not found
		return false;
	} else {
		return true;
	}
}
/**
 * Returns if queue q has a connection with c (:id) and s (:status)
 */
bool const_hasConnStatus(const QQueue& q, const int& c, const int& s,
		const string& create_time) {
	list<QI*>::const_iterator iter = getFirstConnStatusIter(q, c, s,
			create_time);
	string crr = create_time;
//	cout << '\n' << " " << q << '\n';
//	cout << " c=" << cc;
//	cout << " s=" << ss;
//	cout << " ctime=" << crr;
	if (iter == q.Q.end()) { //if conn not found
//		cout << " nf" << '\n';
		return false;
	} else
		return true;
}

/**
 * Returns if queue q has a connection with c (:id) and s (:status) etc
 */
bool const_hasConn(const QQueue& q, const string a1, const string a2,
		const int& c, const int s, const string create_time) {
	list<QI*>::const_iterator iter = getFirstConnIter(q, a1, a2, c, s,
			create_time);
	if (iter == q.Q.end()) { //if conn not found
			//cout << " dd ";
		return false;
	} else {
		//cout << " ff ";
		return true;
	}
}

int getConnId(const Conn* c) {
	return c->id;
}

void copyConnParameters(Conn* conn, const string a, StringQueue& parameters,
		const int mode) {
	list<string*>::const_iterator iter, end;
	if (mode == 1) { //read from conn (iter), write to parameters (iter2)
		if (conn->address1.compare(a) == 0) {
			forlist(conn->reqMsg.Q)
			{
				string *s = *iter;

				//cout << " s " << *s;
				add(*s, parameters);
			}
		} else {
			forlist(conn->respMsg.Q)
			{
				string *s = *iter;
				add(*s, parameters);
			}
		}
	} else { //read from parameters, write to conn
		if (const_compareStrings(a, conn->address1) == 0) {
			forlist(parameters.Q)
			{
				string *s = *iter;
				//cout<< s;
				add(s, conn->reqMsg);
			}
			//cout << '\n' << "addMsgConn " << conn->reqMsg;
		} else {
			forlist(parameters.Q)
			{
				string *s = *iter;
				//cout<< s;
				add(s, conn->respMsg);
			}
			//cout << '\n' << "addMsgConn " << conn->respMsg;
		}
		//printConn(conn);
	}

}
/**
 * Returns if queue q has a connection with c (:id) and s (:status)
 */
Conn* getFirstConn(const QQueue& q, const string a1, const string a2,
		const int& c, const int s, const string create_time) {
	list<QI*>::const_iterator iter = getFirstConnIter(q, a1, a2, c, s,
			create_time);
	return (Conn*) *iter;
}

/*
 * Specific for service_tasks that need to match session info of an incoming request
 */
bool const_hasConnBPELInstanceMatch(const QQueue& q, const string& a1,
		const string& a2, const int& c, const int& s, const string& create_time,
		const StringQueue& propert) {
	//cout<<
	list<QI*>::const_iterator iter = getFirstConnIter(q, a1, a2, c, s,
			create_time);
	if (iter == q.Q.end()) //if such conn is not found, return false;
		return false;
	else if (const_BPELmatch(((Conn*) *iter)->reqMsg, propert)) {
		//cout<<" false ";
		return true;
	} else {

		return false;
	}

}

bool const_BPELmatch(const StringQueue& params, const StringQueue& propert) {
	list<string*>::const_iterator iter, end;
	list<string*>::const_iterator iter2 = params.Q.begin();
	forlist (propert.Q)
	{

		if (iter2 == params.Q.end() || ((*iter)->compare(**iter2)) != 0) {

			//cout << **iter << " " << **iter2;
			return false;
		}
		iter2++;
	}
	return true;
}
/**
 * Returns an iterator from the first connection in queue with status s and id c
 */
list<QI*>::const_iterator getFirstConnIter(const QQueue& q, const string a1,
		const string a2, const int& c, const int s, const string create_time) {
	list<QI*>::const_iterator iter, end;
	forlist(q.Q)
	{
		if (const_isConn((Conn*) *iter, a1, a2, c, s, create_time)) {
			return iter;
		}
	}
	return iter;
}

bool const_hasConnMsg(const QQueue& q, const int& c, const int& s,
		const string& create_time, const string& address, const string& msg) {
	list<QI*>::const_iterator iter = getFirstConnStatusIter(q, c, -1,
			create_time);
	if (iter == q.Q.end()) { //if conn not found
		return false;
	} else {
		Conn* it = (Conn*) *iter;
		return hasFirstMsgConn(it, address, msg);
	}
}
ConnQueue selectDataQuery(QQueue& q, string id) {

	ConnQueue res;
	list<QI*>::const_iterator iter, end;
	int idd;
	if (id.compare("") == 0) {
		idd = -1;
	} else {
		idd = atoi(id.c_str());
	}

	forlist(q.Q)
	{
		if (const_isConn((Conn*) *iter, "", "", idd )) {
			Conn* it = (Conn*) *iter;
			add(makeConnCopy(it), res);

		}
	}
	return res;
}

/* called to add a new <item, cartId>, writes to Data:id, reqMsg */
Conn* insertDataQuery(QQueue& q, string id, string value2) {

	list<QI*>::const_iterator iter;
	Conn* it = connStar();
	it->id = string2int(id);

	if (value2.compare("") != 0) {
		add(stringStar(value2), it->reqMsg); // add value2 to ReqMsg
	}
	add(it, q);

	Conn* res = connStar();
	add(stringStar(id), res->reqMsg);
	return res;
}
/* called to add a new cart with incremental id, writes cart id to Data:id */
Conn* insertDataIncQuery(QQueue& q, string value2) {

	list<QI*>::const_iterator iter;
	int id;
	if (q.Q.size() > 0) {
		id = (getId((QI*) getQueuePtr(q, q.Q.size() - 1)) + 1);
	}

	Conn* it = connStar();
	it->id = id;
	if (value2.compare("") != 0) {
		add(stringStar(value2), it->reqMsg); // add value2 to ReqMsg
	}
	add(it, q);

	Conn* res = connStar();
	add(stringStar(int2string(id)), res->reqMsg);
	return res;
}
Conn* removeDataQuery(QQueue& q, string id) {

	list<QI*>::const_iterator iter;
	int i = 0;
	while ((iter = getFirstConnIter(q, "", "", atoi(id.c_str()))) != q.Q.end()) {
		q.Q.remove(*iter);
		i++;
	}
	Conn* res = connStar();
	res->id = i;
	return res;
}

int getId(const QI* conn) {
	return conn->id;
}
string getCreateTime(const Conn* conn) {
	return conn->create_time;
}

/**
 * Returns the id of the first connection with address 'a' in the queue
 */
Conn* removeFirstConnToAddress(QQueue& q, const string& a) {
	list<QI*>::iterator iter, end;
	forlist (q.Q)
	{
		Conn *it = (Conn*) *iter;
		if (((*it).address2.compare(a)) == 0) {
			//int s = (*it).id;
			q.Q.remove(it);
			return it;
		}
	}
	return (Conn*) *iter;
}
/**
 * Removes the first connection with id and createtime
 */

Conn* removeFirstConn(QQueue& q, const int& a, const int& s,
		const string& create_time) {
	list<QI*>::iterator iter, end;
	forlist (q.Q)
	{
		Conn *it = (Conn*) *iter;
		if (const_isConn(it, "", "", a, -1, create_time)) {
			q.Q.remove(it);
			return it;
		}
	}
	return (Conn*) *iter;
}

/**
 * Returns if a Conn has c (:id) and s (: status)
 */
bool const_isConn(Conn* conn, const string& address1, const string& address2,
		const int& id, const int status, const string& create_time) {
	if (address1.compare("") != 0 && (conn->address1).compare(address1) != 0) {
		//cout << " a";
		return false;
	} else if (address2.compare("") != 0
			&& (conn->address2).compare(address2) != 0) {
		//cout << " b";
		return false;
	} else if (id != -1 && (conn->id) != id) {
		//cout << " c";
		return false;
	} else if (status != -1 && (conn->status) != status) {
		//cout << " d " << status << " " << conn->status;
		return false;
	} else if (create_time.compare("-1") != 0
			&& (conn->create_time).compare(create_time) != 0) {
		//cout << " e";
		return false;
	}
//cout << " f";
	return true;
}
/**
 * Returns an iterator from the first connection in queue with status s and id c
 */
list<QI*>::const_iterator getFirstConnStatusIter(const QQueue& q, const int& c,
		const int& s, const string& create_time) {
	list<QI*>::const_iterator iter, end;
	forlist(q.Q)
	{
		if (const_isConn((Conn*) *iter, "", "", c, s, create_time)) {
			return iter;
		}
	}
	return iter;
}

/**
 * Returns the first connection in queue with status s and id c
 */
ConnStar getFirstConnStatus(const QQueue& q, const int& c, const int& s,
		const string& create_time) {
	list<QI*>::const_iterator iter = getFirstConnStatusIter(q, c, s,
			create_time);
	Conn* it = (Conn*) *iter;
	return it;
}

ConnStar const_getFirstConnStatus(const QQueue& q, const int& c, const int& s,
		const string& create_time) {
	list<QI*>::const_iterator iter = getFirstConnStatusIter(q, c, s,
			create_time);
	Conn* it = (Conn*) *iter;
	return it;
}

/**
 *
 */
bool const_hasToRead(const QQueue& q, const string& a, const int&c,
		const string& create_time) {

	list<QI*>::const_iterator iter = getFirstConnStatusIter(q, c, -1,
			create_time);
	if (iter == q.Q.end()) { //if conn not found
			//cout<<'\n'<<"not found "<<a<<" "<<c<<" "<<s;
		return false;
	}
//cout<<'\n'<<"found "<<a<<" "<<c<<" "<<s;
	Conn *conn = (Conn*) *iter;
	if (const_compareStrings(conn->address1, a) == 0) {
		if (conn->respMsg.Q.size() == 0) {
			//cout<<'\n'<<"empty respMsg";
			return false;
		} else {
			return true;
		}
		return true;
	} else {
		if (conn->reqMsg.Q.size() == 0) {
			//cout<<'\n'<<"empty reqMsg";
			return false;
		} else { //there is a msg in reqMsg
			//cout<<'\n'<<"clabla";
			return true;
		}
	}
//cout << '\n' << "blabla";
	return true;
}
/**
 * Returns if q (:the queue of strings) contains a (:string)
 */
bool const_hasConnAddress(const QQueue& q, const string& a) {
	list<QI*>::const_iterator iter, end;
//cout<<" start search ";
	forlist (q.Q)
	{
		Conn *it = (Conn*) *iter;
		if (((*it).address2.compare(a)) == 0) {
			//cout<<" found ";
			return true;
		}
	}
//cout<<" not found ";
	return false;
}

void setConnStatus(ConnStar& conn1, const int& s) {
	conn1->status = s;
}

//******** Queue operations ******************/

///*********** Replic Discovery - Registry ********************/

/*string selectRandReplc(const AQueue<string>& addrQueue) {
 //int RandIndex = rand() % addrQueue.Q.size();
 int RandIndex = addrQueue.Q.size() - 1;
 list<string>::const_iterator i = addrQueue.Q.begin();
 std::advance(i, RandIndex);
 return (*i);
 }

 int hasCheckpoint(const int& id, const QQueue<int>& q) {
 if (find(q.Q.begin(), q.Q.end(), id) == q.Q.end())
 return 0;
 return 1;
 }

 void renew(const string& addr, QQueue<Replc>& q) {
 list<Replc>::iterator i;
 for (i = q.Q.begin(); i != q.Q.end(); i++) {
 if (const_compareStrings((*i).address, addr)) {
 (*i).time = 0;
 return;
 }
 }
 }*/
/*void addTime(QQueue<Replc>& q, const int& time) {
 if (q.Q.size() == 0) {
 cout << '\n' << "addTime to Empty Queue";
 return;
 }
 list<Replc>::iterator i;
 for (i = q.Q.begin(); i != q.Q.end();) {
 if (++((*i).time) >= time) {
 cout << '\n' << "ready To Erase " << (*i);
 q.Q.erase(i++);
 } else {
 i++;
 }
 }
 }*/

/////////// Replcas ////////////////////
Replc* replcStar(const string s1) {
	return new Replc(s1);
}

bool const_isReplc(const Replc* r, const int& id, const int& time,
		const string& addr) {
	if (id > -1 && r->id != id) {
		return false;
	} else if (time > -1 && r->time != time) {
		return false;
	} else if (addr.compare("") != 0 && r->address.compare(addr) != 0) {
		return false;
	}
	return true;
}

list<QI*>::const_iterator getFirstReplcIter(const QQueue& q, const int& c,
		const int& s, const string& addr) {

	list<QI*>::const_iterator iter, end;
	forlist(q.Q)
	{
		if (const_isReplc((Replc*) *iter, c, s, addr)) {
			return iter;
		}
	}
	return iter;

}
list<QI*>::iterator getFirstReplcIter(QQueue& q, const int& c, const int& s,
		const string& addr) {

	list<QI*>::iterator iter, end;
	forlist(q.Q)
	{
		if (const_isReplc((Replc*) *iter, c, s, addr)) {
			return iter;
		}
	}
	return iter;

}

bool const_isSess2Repl(const Sess2Repl* r, const int& id, const int& time,
		const string& addr) {
	if (id > -1 && r->id != id) {
		return false;
	} else if (time > -1 && r->time != time) {
		return false;
	} else if (addr.compare("") != 0 && r->replicaAddress.compare(addr) != 0) {
		return false;
	}
	return true;
}

list<QI*>::const_iterator getFirstSess2ReplIter(const QQueue& q, const int& c,
		const int& s, const string& addr) {

	list<QI*>::const_iterator iter, end;
	forlist(q.Q)
	{
		if (const_isSess2Repl((Sess2Repl*) *iter, c, s, addr)) {
			return iter;
		}
	}
	return iter;

}

//Sess2Repl* getFirstSess2Repl(const QQueue& q, const int& c,
//		const int& s, const string& addr) {
//
//	list<QI*>::const_iterator iter = getFirstSess2ReplIter(q, c, s,
//				addr);
//		return (Sess2Repl*) *iter;
//
//}

bool const_hasSess2Repl(const QQueue& q, const int id, const int time,
		const string replicaAddress) {
	if (getFirstSess2ReplIter(q, id, time, replicaAddress) == q.Q.end()) {
		return false;
	}
	return true;
}

bool const_hasReplcaAddress(const string& addr, const ReplcQueue& q) {
	list<QI*>::const_iterator iter = getFirstReplcIter(q, -1, -1, addr);
	if (iter == q.Q.end()) {
		return false;
	}
	return true;
}

StringQueue listReplcas(const ReplcQueue& q) {

	StringQueue addrQueue;
	list<QI*>::const_iterator iter, end;
	forlist (q.Q)
	{
		add(((Replc*) (*iter))->address, addrQueue);
	}
	return addrQueue;
}

std::ostream& operator<<(std::ostream& os, const Replc& q) {
	q.printItem(os);
	return os;
}

std::ostream& operator<<(std::ostream& os, const ReplcStar& q) {
	return os << *q;
}
void renewAlive(const StringQueue& address, ReplcQueue& q) {
	int c = -1;
	list<string*>::const_iterator iter, end;
	forlist(address.Q)
	{
		string* address = *iter;
		list<QI*>::iterator i = getFirstReplcIter(q, c, c, *address);
		if (i != q.Q.end()) {
			((Replc*) *i)->time = 0;
		}
	}
}

void readData(const DataQueue dataQueue, const int dataId, DataStar result,
		string error) {
	list<QI*>::const_iterator iter = getFirstConnIter(dataQueue, "", "",
			dataId);
	if (iter != dataQueue.Q.end()) {
		copyStringQueue(((Conn*) *iter)->reqMsg, ((Conn*) result)->reqMsg);
	} else {
		error = "not found";
	}
}

void updateData(DataQueue dataQueue, const int dataId, const int itemId) {
	list<QI*>::const_iterator iter = getFirstConnIter(dataQueue, "", "",
			dataId);
	if (iter != dataQueue.Q.end()) {
		stringstream ss;
		ss << itemId;
		StringStar s = stringStar(ss.str());
		add(s, ((Conn*) *iter)->reqMsg);
	}
}

void getParameter(string* parameter, const StringQueue& parameters,
		const int& pos) {
	list<string*>::const_iterator i = parameters.Q.begin();
	advance(i, pos);
	parameter = *i;
}

void updateGetDataId(const StringQueue& q, int& dataId) {
	dataId = atoi((*q.Q.front()).c_str());
}

Sess2Repl* sess2ReplStar(const int id, const string& replicaAddress) {
	return new Sess2Repl(id, replicaAddress);
}
void addData(DataQueue dataQueue, DataStar data, int dataId) {
	dataId = getNextId(dataQueue, 200);
	data = connStar("", "", dataId);
	add(data, dataQueue);
}

DataStar dataStar(const string& s1, const string& s2, const int& c,
		const int& s, const string& t) {
	return connStar(s1, s2, c, s, t);
}

///******** START custom *************************/

void Replc::printItem(std::ostream& os) const {
	os << "[" << this->id << "," << this->time << "," << this->address << "]";
}
void Replc::unmarshalling(const char* c) {
	cout << c << " Replc unmarshalling " << endl;
	int par = 0, j = 1; //passimg '['
	stringstream osid;
	stringstream ostime;
	this->address = "";
	while (true) {
		if (c[j] == ']') {
			break;
		} else if (c[j] == ',') {
			par++;
		} else if (par == 0) {
			osid << c[j];
		} else if (par == 1) {
			ostime << c[j];
		} else if (par == 2) {
			this->address.push_back(c[j]);
		}
		j++;
	}
	/*cout<<ostime.str()<<" "<<osid.str()<<endl;*/
	this->time = boost::lexical_cast<int>(ostime.str());
	this->id = boost::lexical_cast<int>(osid.str());
	cout << c << " Replc unmarshalled " << *this << endl;
}

long Replc_sizeof(const Replc& r) {
	return r.size();
}
void Replc_marshalling(char* c, const Replc* t) {
	//cout<<endl<<*t<<" Replc_marshalling ... "<<endl;
	t->marshalling(c);
	//cout<<*t<<" Replc unmarshalled "<<c<<endl;
}

void Replc_unmarshalling(Replc* t, const char* c) {
	//cout<<endl<<c<<" Replc unmarshalling ... "<<endl;
	t->unmarshalling(c);
	//cout<<c<<" Replc_unmarshalled "<<*t<<endl;
}
long ReplcStar_sizeof(const ReplcStar& r) {
	return (*r).size();
}
void ReplcStar_marshalling(char* c, const ReplcStar* t) {

	cout << endl << *t << " ReplcStar_marshalling ... ";
	(*t)->marshalling(c);
	cout << endl << *t << " ReplcStar_marshalled to " << c << endl;
}
void ReplcStar_unmarshalling(ReplcStar* t, const char* c) {

	//cout<<endl<<c << " ReplcStar_unmarshalling ... ";
	(*t)->unmarshalling(c);

	//cout<<endl<<c << " ReplcStar_unmarshalled to "<<*t<<endl;
}
long ReplcQueue_sizeof(const QQueue& r) {
	return r.size();
}
void ReplcQueue_marshalling(char* c, const QQueue* q) {
	//cout<<endl<<*q << " ReplcQueue_marshalling ... ";
	QQueue qq = *q;
	qq.marshalling(c);
	//cout<<endl<<*q<< " ReplcQueue_marshalled to "<<c<<endl;
}
void ReplcQueue_unmarshalling(QQueue* t, const char* c) {

	//cout<<endl<<c << " ReplcQueue_unmarshalling ... "<<endl;
	clearQueue(*t);
	if (*c != '[' || *(c + 1) == ']') {
		return;
	}
	Replc *conn;
	while (*c != ']') {
		c++; //for initial [ or ,
		conn = new Replc();
		conn->unmarshalling(c);
		add(conn, *t);
		c += conn->size();
	}
	//cout<<endl<<c << " ReplcQueue_unmarshalled to "<<*t<<endl;
}
void printReplcStar(std::ostream& os, Replc* r) {
	r->printItem(os);
}
void printReplc(Replc* r) {
	r->printItem(cout);
}
///*****************************************/

string getTime() {
	static int time = 0;
	stringstream os("");
	os << (++time);
	return os.str().c_str();
}

long ConnQueue_sizeof(const QQueue& q) {
	return q.size();
}
void ConnQueue_marshalling(char* c, const QQueue* q) {

	cout << endl << " ConnQueue_marshalling ... ";
	((QQueue *) q)->marshalling(c);

	cout << endl << " ConnQueue_marshalled ... ";
}
void ConnQueue_unmarshalling(QQueue* t, const char* c) {
	cout << endl << " ConnQueue_unmarshalling ... ";
	clearQueue(*t);

	if (*c != '[') {
		/*cout<<c<<" ConnQueue_unmarshalled to "<<*t<<endl;*/
		return;
	}
	Conn *conn;
	while (*(c) != ']' && *(c + 1) != ']') {
		c++; //for initial [ or ,
		conn = new Conn();
		conn->unmarshalling(c);
		add(conn, *t);
		c += conn->size();
	}
	cout << " ConnQueue_unmarshalled " << endl;
}

/*
 long ChkpointQueue_sizeof(const QQueue<int>& q) {
 return IntQueue_sizeof(q);
 }
 void ChkpointQueue_marshalling(char* c, const QQueue<int>* q) {
 return IntQueue_marshalling(c, q);
 }

 void ChkpointQueue_unmarshalling(QQueue<int>* t, const char* c) {
 IntQueue_unmarshalling(t, c);

 }*/

/*void IntQueue_unmarshalling(QQueue<int>* t, const char* c) {
 cout << '\n' << " Queue unmarshalling " << c;
 clearQueue(*t);
 const char* p = c;
 p++; //ignore "["
 char next = p[0];
 stringstream os;
 while ((++next) != ']') {
 if((next)==','){
 add(boost::lexical_cast<int>(os.str()),*t);
 os.clear();
 }else{
 os<<next;
 }
 }
 cout << '\n' << " Queue unmarshalled " << (*t);
 }
 */
void IntQueue_marshalling(char* c, const AQueue<int>* q) {
	cout << '\n' << " IntQueue marshalling " << (*q);
	list<int*>::const_iterator iter, end;
	stringstream os("[");
	forlist((*q).Q)
	{
		if (iter != (*q).Q.begin()) {
			os << ",";
		}
		int *qi = *iter;
		os << (*qi);
	}
	os << "]";
	strcpy(c, os.str().c_str());
	cout << '\n' << " IntQueue marshalled " << c << '\n';
}

long IntQueue_sizeof(const AQueue<int>& q) {
	list<int*>::const_iterator iter, end;
	stringstream os("[");
	forlist(q.Q)
	{
		if (iter != q.Q.begin()) {
			os << ",";
		}
		int *qi = *iter;
		os << (*qi);
	}
	os << "]";
	return strlen(os.str().c_str());
}

////////////// ******************* *************************/////////////////
long Conn_sizeof(const Conn& q) {
	return q.size();
}
void Conn_marshalling(char* c, const Conn* s) {
	s->marshalling(c);
}

void Conn_unmarshalling(Conn* t, const char* c) {
	t->unmarshalling(c);

}

Conn* makeConnCopy(Conn*& conn) {
	Conn* copy = connStar(conn->address1, conn->address2, conn->id,
			conn->status, conn->create_time);
	return copy;
}

DataStar makeDataCopy(const DataStar& data) {
	ConnStar c = connStar(data->address1, data->address2, data->id,
			data->status, data->create_time);
	copyStringQueue(data->reqMsg, c->reqMsg);
	copyStringQueue(data->respMsg, c->respMsg);
	return c;
}

DataQueue makeDataCopy(const DataQueue data) {
	DataQueue n;
	list<QI*>::const_iterator iter, end;
	forlist(data.Q)
	{
		add(makeDataCopy((DataStar) *iter), n);
	}
	return n;
}

void deleteConnStar(ConnStar& conn) {
//delete conn;
}
void deleteStringStar(StringStar& conn) {
//delete conn;
}
long ConnStar_sizeof(const ConnStar& cc) {
	return (*cc).size();
}
void ConnStar_marshalling(char* c, const ConnStar* ss) {
	cout << endl << " ConnStar_marshalling ... ";
	(*ss)->marshalling(c);
	cout << endl << " ConnStar_marshalled ";
}
void ConnStar_unmarshalling(ConnStar* tt, const char* c) {
	cout << endl << " ConnStar_unmarshalling ... ";
	(*tt)->unmarshalling(c);
	cout << endl << " ConnStar_unmarshalled ... ";
}
void deleteStar(QI* star) {
	delete star;
}
///*****************************************/
void testString_marshalling() {
	string s1 = "";
	StringQueue sq;

	int size1 = string_sizeof(s1);
	cout << endl << s1 << " has size= " << size1 << endl;
	int size2 = StringQueue_sizeof(sq);
	cout << endl << sq << " has size= " << size2 << endl;

	char c[size1 + size2];
	char *f = c;

	string_marshalling(c, &s1);
	//cout << s1 << " is marshalled to " << c << endl;
	f += (string_sizeof(s1));
	StringQueue_marshalling(f, &sq);
	//cout << s1 << " is marshalled to " << c << endl;

	f = c;
	string_unmarshalling(&s1, f);
	f += (string_sizeof(s1));
	//cout << c << " is unmarshalled to " << s1 << endl;
	StringQueue_unmarshalling(&sq, f);
	//	cout << c << " is unmarshalled to " << s1 << endl;

	if (string_sizeof(s1) + StringQueue_sizeof(sq) - (size1 + size2) != 0) {
		cout << endl << "testStringStar failed " << string_sizeof(s1) << " "
				<< StringQueue_sizeof(sq);
	} else {
		cout << endl << "testStringStar passed " << string_sizeof(s1) << " "
				<< StringQueue_sizeof(sq);
	}

//	if (string_sizeof(s1) - (size1) != 0) {
//			cout << "testStringStar failed "<<string_sizeof(s1)<<" "<<StringQueue_sizeof(sq);
//		} else {
//			cout << "testStringStar passed ";
//		}

}

void StringQueue_marshalling(char* c, const AQueue<string>* q) {
	cout << endl << " StringQueue_marshalling... ";
	std::ostringstream os;
	os << *q;
	strncpy(c, os.str().c_str(), os.str().size());
	cout << endl << " StringQueue_marshalled ";
}
long StringQueue_sizeof(const AQueue<string>& q) {

	std::ostringstream os;
	os << q;
	return os.str().size();
}
void StringQueue_unmarshalling(AQueue<string>* t, const char* c) {
	cout << endl << " StringQueue_unmarshalling... ";
	while (!t->Q.empty()) {
		delete t->Q.front();
		//t->Q.pop_front();
		cout << "a ";
	}
	if (*(c + 1) == ']') {
		cout << "c ";
		return;
	}
	unsigned int i = 1; //surpass first '['
	t->Q.push_back(new string(""));
	while (true) {

		if (c[i] == ',') {
			cout << "d ";
			t->Q.push_back(new string(""));
			cout << "e ";
		} else if (c[i] == ']') {
			cout << "f ";
			i++; //unnecessary
			break;
		} else {
			cout << "g ";
			t->Q.back()->push_back(c[i]);
			cout << "h ";
		}

		i++;
	}
	cout << "i :" << i << " " << *t;

	cout << endl << " StringQueue_unmarshalled ";
}
string appendToString(string& to, const string& from) {
	to.append(from);
	return to;
}
void addToAddress(string& address, const int& id) {
	stringstream s("");
	s << id;
	address.append(s.str());
}
/*

 std::ostream& operator<<(std::ostream& os, const QQueue<Replc>& q) {
 os << "[";
 typename list<Replc>::const_iterator i = q.Q.begin();
 while (i != q.Q.end()) {
 os << "[" << (*i).address << "," << (*i).time << "]";
 i++;
 }
 os << "]";

 return os;
 }

 void ReplcQueue_marshalling(char* c, const QQueue<Replc>* q) {
 Queue_marshalling(c, q);
 }
 void ReplcQueue_unmarshalling(QQueue<Replc>* t, const char* c) {

 Queue_unmarshalling(t, c);
 }

 long ReplcQueue_sizeof(const QQueue<Replc>& q) {

 return Queue_sizeof(q);
 }
 */

/*AQueue<string> addString(string s, AQueue<string>& q){
 string p = s;
 string* t=&p;
 add(t,q);
 return q;
 }*/

string strings(const string& s) {
	string p("");
	p = s;
	return p;
}
/***************  STRING ********************/

long string_sizeof(const string& s) {
	return (strlen(s.c_str()) + 2);
}

void string_marshalling(char* c, const string* s) {
//	cout<<endl<< *s <<" string_marshalling "<<c;
	//size_t size = strlen(s->c_str());
	int size = 0;
	strcpy(c + size, "'");
	size++;
	strcpy(c + size, s->c_str());
	size += strlen(s->c_str());
	strcpy(c + size, "'");
//	*c=*c+strlen(s->c_str());
//	for (unsigned int i=0; i<strlen(s->c_str())+3; i++){
//		if(c[i]=='\0'){
//			cout<<"t";
//		}else{
//			cout<<"p"<<c[i];
//		}
//	}
//	cout << endl<< *s<< " string_marshalled " << c ;

}
void string_unmarshalling(string* t, const char* c) {
	//cout<<endl<< c <<" string_unmarshalling ... ";
	(*t).clear();
	unsigned int i = 0;
	c++;	//first '
	while (c[i] != '\'') {
		(*t).push_back(c[i]);
		//cout << " pushed:" << c[i];
		i = i + 1;
	}
	//cout<<endl <<" string_unmarshalled ... "<<*t;
}

long StringStar_sizeof(const StringStar& s) {
	return string_sizeof(*s);
}
void StringStar_marshalling(char* c, const StringStar* s) {
	return string_marshalling(c, *s);
}
void StringStar_unmarshalling(StringStar* t, const char* c) {
	return string_unmarshalling(*t, c);
}

long StringSStar_sizeof(const StringStar& s) {
	return 0;
}
void StringSStar_marshalling(char* c, const StringStar* s) {
	delete (s);
}
void StringSStar_unmarshalling(StringStar* t, const char* c) {
	*t = stringStar();
}

/***********************************************/

void rmvTimedOut(QQueue& q, const int& timeout) {
	list<QI*>::iterator iter, end;
	forlist(q.Q)
	{
		if ((*iter)->time >= timeout) {
			q.Q.erase(iter);
		} else {
			++((*iter)->time);
		}
	}

}
Sess2Repl* getSess2ReplcPos(QQueue& q, const int& next) {
	list<QI*>::iterator i = q.Q.begin();
	std::advance(i, next);
	return ((Sess2Repl*) *i);
}
int getSesTime(QQueue& q, const int& next) {
	list<QI*>::iterator i = q.Q.begin();
	std::advance(i, next);
	return (*i)->time;
}

int addiTime(QQueue& q, const int& next) {
	list<QI*>::iterator i = q.Q.begin();
	std::advance(i, next);
	return ++((*i)->time);
}

string int2string(const int& e) {
	stringstream os1;
	os1 << e;
	return os1.str();
}
int string2int(const string& s) {
	return atoi(s.c_str());
}
QI* removePosition(QQueue& q, const int& next) {
	list<QI*>::iterator i = q.Q.begin();
	std::advance(i, next);
//delete * i;
	return *(q.Q.erase(i));
}

Replc* removeReplcPosition(QQueue& q, const int& next) {
	return (Replc*) removePosition(q, next);
}
string getReplcAddress(Replc * r) {
	return r->address;
}

/*
 * Adds a msg to the request or response message queue of connection
 */
void addMsgConn(ConnStar& conn, const string& address, string& msg) {
	string *s = &msg;
	if (const_compareStrings(address, conn->address1) == 0) {
		add(s, conn->reqMsg);
		//cout<<" lala "<<*conn;
	} else {
		add(s, conn->respMsg);
	}
}

/*************** Replicas ***********************/

int stringToInt(string s) {
	return atoi(s.c_str());
}

void isNewSession(const string& sessionId, Sess2ReplQueue& sess2repl,
		int newSes) {

	int sessId = atoi(sessionId.c_str()); // turn session id to integer
	/* search for the replica that serves this session */
	list<QI*>::const_iterator iter = getFirstSess2ReplIter(sess2repl, sessId,
			-1, "");

	if (iter != sess2repl.Q.end()) { //it is not supposed to happen, but...
		newSes = 0;
	}
}

string assignTarget(const string& sessionId, Sess2ReplQueue& sess2repl) {

	int sessId = atoi(sessionId.c_str()); // turn session id to integer
	/* search for the replica that serves this session */
	list<QI*>::const_iterator iter = getFirstSess2ReplIter(sess2repl, sessId,
			-1, "");
	string addressTo = "";
	if (iter != sess2repl.Q.end()) { //it is not supposed to happen, but...
		addressTo = ((Sess2Repl*) *iter)->replicaAddress;
	}
	return addressTo;
}

/** assigns to sessionId the first item of sessionData
 */
void getSessionIdFromData(string& sessionId, const StringQueue& sessionData) {
	sessionId = *(sessionData.Q.front());
}

void getSessionCreateTimeFromData(string& create_time,
		const StringQueue& sessionData) {
	list<string*>::const_iterator i = sessionData.Q.begin();
	advance(i, 1);
	string *s = *i;
	create_time = *s;
}

string chooseReplica(StringQueue replicas, Sess2ReplQueue& sess2repl) {

	return chooseReplcaLessSessions(replicas, sess2repl);

}

void id2address(const int& id, string& address) {
	if (id == 1) {
		address = "101"; /* client */
	} else if (id == 2) {
		address = "102"; /* proxy */
	} else if (id == 3) {
		address = "103"; /* server1 */
	} else if (id == 4) {
		address = "104"; /* server2 */
	}
}

void addListnAddresses(const int& id, StringStar& address,
		StringQueue& addrQueue) {
	if (id == 1) {
		address = stringStar("101"); /* client */
		add(address, addrQueue);
	} else if (id == 2) {
		address = stringStar("102"); /* proxy */
		add(address, addrQueue);
	} else if (id == 3) {
		address = stringStar("103"); /* server1 */
		add(address, addrQueue);
	} else if (id == 4) {
		address = stringStar("104"); /* server2 */
		add(address, addrQueue);
	}
}

void addMsgBusCtrlAddresses(const int& id, StringStar& address,
		StringQueue& addresses) {
	if (id == 1) {
		/* client: no need */
	} else if (id == 2) {
		address = stringStar("102"); /* proxy */
		add(address, addresses);
	} else if (id == 3) {
		address = stringStar("103");
		add(address, addresses); /* server1 */
	} else if (id == 4) {
		address = stringStar("104");
		add(address, addresses); /* server2 */
	}
}
void copyQQueue(const QQueue& from, QQueue& to) {
	clearQueue(to);
	list<QI*>::const_iterator iter, end;

	forlist(from.Q)
	{
		QI *s = *iter;
		add(s, to);
	}
}
void copyStringQueue(const StringQueue& from, StringQueue& to) {
	clearQueue(to);
	list<string*>::const_iterator iter, end;

	forlist(from.Q)
	{
		string *s = *iter;
		add(s, to);
	}

}

void copyParameters(const StringQueue& from, StringQueue& to) {
	copyStringQueue(from, to);

}
void addToReplcaSessions(Sess2Repl* sr, const string sessionId,
		const string replica, Sess2ReplQueue& sess2repl) {
	sr = sess2ReplStar(atoi(sessionId.c_str()), replica);
	add(sr, sess2repl);
}

void updateReplcaSession(const string sessionId, Sess2ReplQueue& sess2repl) {
	(*getFirstSess2ReplIter(sess2repl, atoi(sessionId.c_str()), -1, ""))->time =
			0;
}

//void pendingResponse(next)

string chooseReplcaLessSessions(StringQueue replicas,
		Sess2ReplQueue& sess2repl) {
	list<string*> repl = replicas.Q;
	list<string*>::iterator r;
	int c, cpast = 100;

	string replicaLess = "";
	// for each replica
	replicaLess = **(repl.begin());

	// if there are sessions in the list
	if (sess2repl.Q.size() > 0) {

		for (r = repl.begin(); r != repl.end(); ++r) {
			string thisReplca = **r;

			list<QI*>::iterator iter, end;
			// count its sessions
			c = 0;
			forlist(sess2repl.Q)
			{
				Sess2Repl* s2r = (Sess2Repl*) *iter;
				if (s2r->replicaAddress.compare(thisReplca)) {
					c++;
				}

			}
			if (c < cpast) {
				replicaLess = thisReplca;
				cpast = c;
				if (c == 0) {
					break;
				}
			}

		}
	}

	return replicaLess;
}

void DataStar_marshalling(char* c, const DataStar* ss) {
	ConnStar_marshalling(c, (ConnStar*) ss);
}
void DataStar_unmarshalling(DataStar* tt, const char* c) {
	ConnStar_unmarshalling((ConnStar*) tt, c);
}
long DataStar_sizeof(const DataStar& cc) {
	return ConnStar_sizeof((ConnStar) cc);
}

void DataQueue_marshalling(char* c, const DataQueue* ss) {
	ConnQueue_marshalling(c, (ConnQueue*) ss);
}
void DataQueue_unmarshalling(DataQueue* tt, const char* c) {
	ConnQueue_unmarshalling((ConnQueue*) tt, c);
}
long DataQueue_sizeof(const DataQueue& cc) {
	return ConnQueue_sizeof((ConnQueue) cc);
}

/***************  ADDRQUEUE ********************/

/*int main() {

 string replicaLess;
 StringQueue replicas = StringQueue();
 Sess2ReplQueue sess2repl = Sess2ReplQueue();
 Sess2ReplStar sess = sess2ReplStar(0, "");

 StringStar re = stringStar("1");
 add(re, replicas);

 re = stringStar("2");
 add(re, replicas);

 int sessId = getNextId(sess2repl, 2);
 //string str = stoi(sessId);
 addToReplcaSessions(sess, "0", "1", sess2repl);
 cout << sessId << " " << '\n';
 cout << sess2repl << " " << '\n';
 sessId = getNextId(sess2repl, 2);
 addToReplcaSessions(sess, "1", "1", sess2repl);
 cout << sessId << " " << '\n';

 cout << sess2repl << " " << replicas << " " << '\n';

 cout << " fin";Less


 }*/

//int main(){
//	StringQueue sq;
//	string s = "cartId itemId";
//	splitPath(s,sq);
//	cout<<sq;
//}
bool const_check(int& i, int& g) {
	cout << endl << i << ", " << g << endl;
	return (i == g);
}

void addRequestedOperationToParameters(StringStar& operReq,
		StringQueue& parametersIn) {
	add(operReq, parametersIn);
}
void allTests();

//int main(){
//
//
//  ConnStar conn = connStar();
//	 data ConnQueue connQueue
//	 data int maxId, connection
//
//	ConnQueue sq;
//	add(connStar(),sq);
//	printQQueue(sq);
//}

void allTests() {

//	testStringStar_marshalling();
	testString_marshalling();
//	testStringQueue_marshalling();
//		testConnQueue_marshalling();
//		testConnStar_marshalling();
//		testConn_marshalling();
//		testReplcQueue_marshalling();
//		testReplcStar_marshalling();
	//testSess2Repl_marshalling();
//testSess2ReplStar_marshalling();
//testSess2ReplQueue_marshalling();

}

void testSess2ReplQueue_marshalling() {
	Sess2Repl q;
	q.replicaAddress = "sth2";
	q.time = 15;
	q.id = 12;
	char c[100];

	Sess2ReplQueue qq;
	add(q, qq);
	add(q, qq);
	int size = Sess2ReplQueue_sizeof(qq);
	cout << qq << " has size= " << size << endl;
	Sess2ReplQueue_marshalling(c, &qq);
	cout << qq << " is marshalled to " << c << endl;
	Sess2ReplQueue_unmarshalling(&qq, c);
	cout << c << " is unmarshalled to " << qq << endl;
	if (Sess2ReplQueue_sizeof(qq) - size != 0) {
		cout << "testSess2ReplQueue failed";
	} else {
		cout << "testSess2ReplQueue passed";
	}
}

void testSess2ReplStar_marshalling() {
	Sess2Repl* q = sess2ReplStar();
	q->replicaAddress = "sth2";
	q->time = 15;
	q->id = 12;

	char c[20];
	Sess2Repl r = *q;
	int size = Sess2Repl_sizeof(r);
	cout << endl << r << " has size= " << size << endl;
	Sess2Repl_marshalling(c, &r);
	cout << r << " is marshalled to " << c << endl;
	Sess2Repl_unmarshalling(&r, c);
	cout << c << " is unmarshalled to " << r << endl;
	if (Sess2Repl_sizeof(r) - size != 0) {
		cout << "testSess2ReplStar failed";
	} else {
		cout << "testSess2ReplStar passed";
	}
}

void testSess2Repl_marshalling() {
	Sess2Repl q;
//	q.replicaAddress = "sth2";
//	q.time = 15;
//	q.id = 12;
//
//	char c[20];
	const char *c = "[-1,0,][][][][][][][103,104]";
////Sess2Repl r = *q;
	int size = Sess2Repl_sizeof(q);
//	cout <<endl<< q << " has size= " << size << endl;
//	Sess2Repl_marshalling(c, &q);
//	cout << q << " is marshalled to " << c << endl;

	Sess2Repl_unmarshalling(&q, c);
	cout << endl << c << " is unmarshalled to " << q << endl;
	if (Sess2Repl_sizeof(q) - size != 0) {
		cout << "testSess2Repl failed";
	} else {
		cout << "testSess2Repl passed " << Sess2Repl_sizeof(q) << endl;
	}
	c = c + Sess2Repl_sizeof(q);
	//cout<<c;
	Sess2Repl_unmarshalling(&q, c);
	cout << endl << c << " is unmarshalled to " << q << endl;
	if (Sess2Repl_sizeof(q) - size != 0) {
		cout << "testSess2Repl failed";
	} else {
		cout << "testSess2Repl passed" << Sess2Repl_sizeof(q) << endl;
	}
	c = c + Sess2Repl_sizeof(q);
}
void testReplcQueue_marshalling() {
	ReplcStar q = replcStar();
	q->address = "sth2";
	q->time = 15;
	q->id = 12;

	ReplcQueue qq;
	char c[100];
	add(q, qq);
	add(q, qq);
	int size = ReplcQueue_sizeof(qq);
	cout << endl << qq << " has size= " << size << endl;
	ReplcQueue_marshalling(c, &qq);
	cout << qq << " is marshalled to " << c << endl;
	ReplcQueue_unmarshalling(&qq, c);
	cout << c << " is unmarshalled to " << qq << endl;
	if (ReplcQueue_sizeof(qq) - size != 0) {
		cout << "testReplcQueue failed";
	} else {
		cout << "testReplcQueue passed";
	}
}
void testReplcStar_marshalling() {
	ReplcStar q = replcStar();
	q->address = "sth2";
	q->time = 15;
	q->id = 12;

	char c[20];
	Replc r = *q;
	int size = Replc_sizeof(r);
	cout << endl << q << " has size= " << size << endl;
	Replc_marshalling(c, &r);
	cout << q << " is marshalled to " << c << endl;
	Replc_unmarshalling(&r, c);
	cout << c << " is unmarshalled to " << q << endl;
	if (Replc_sizeof(r) - size != 0) {
		cout << "testReplcStar failed";
	} else {
		cout << "testReplcStar passed";
	}
}
void testConnQueue_marshalling() {
//ConnStar q;
	/*q->address1 = "sth1";
	 q->address2 = "sth2";

	 q->id = 12;
	 StringStar s = stringStar("ipi1");
	 add(s, q->reqMsg);
	 s = stringStar("ipi2");
	 add(s, q->reqMsg);
	 s = stringStar("ipi3");
	 add(s, q->respMsg);

	 q->status = 3;
	 q->create_time = "1000092";
	 q->time = 19;*/
	char c[100];
	ConnQueue qq;
	/*add(q, qq);
	 add(q, qq);
	 add(q, qq);
	 */int size = ConnQueue_sizeof(qq);
	cout << endl << qq << " has size= " << size << endl;
	ConnQueue_marshalling(c, &qq);
	cout << qq << " is marshalled to " << c << endl;
	ConnQueue_unmarshalling(&qq, c);
	cout << c << " is unmarshalled to " << qq << endl;
	if (ConnQueue_sizeof(qq) - size != 0) {
		cout << "testConnQueue failed";
	} else {
		cout << "testConnQueue passed";
	}
}

void testConn_marshalling() {
	Conn q;
	/*q.address1 = "sth1";
	 q.address2 = "sth2";
	 q.create_time = "1000092";
	 q.id = 12;
	 StringStar s = stringStar("ipi1");
	 add(s, q.reqMsg);
	 s = stringStar("ipi2");
	 add(s, q.reqMsg);
	 s = stringStar("ipi3");
	 add(s, q.respMsg);
	 q.status = 3;
	 q.time = 19;*/
	char c[100];
	int size = Conn_sizeof(q);
	cout << endl << q << " has size= " << size << endl;
	Conn_marshalling(c, &q);
	cout << q << " is marshalled to " << c << endl;
	Conn_unmarshalling(&q, c);
	cout << c << " is unmarshalled to " << q << endl;
	if (Conn_sizeof(q) - size != 0) {
		cout << "testConn failed";
	} else {
		cout << "testConn passed";
	}
}

void testConnStar_marshalling() {
	ConnStar q = connStar();
//	q->address1 = "sth1";
//	q->address2 = "sth2";
//	q->create_time = "1000092";
//	q->id = 12;
//	StringStar s = stringStar("ipi1");
//	add(s, q->reqMsg);
//	s = stringStar("ipi2");
//	add(s, q->reqMsg);
//	s = stringStar("ipi3");
//	add(s, q->respMsg);
//
//	q->status = 3;
//	q->time = 19;

	char c[100];

	int size = ConnStar_sizeof(q);
	cout << endl << q << " has size= " << size << endl;
	ConnStar_marshalling(c, &q);
	cout << q << " is marshalled to " << c << endl;
	ConnStar_unmarshalling(&q, c);
	cout << c << " is unmarshalled to " << q << endl;
	if (ConnStar_sizeof(q) - size != 0) {
		cout << "testConnStar failed";
	} else {
		cout << "testConnStar passed";
	}

}

void testStringStar_marshalling() {
	StringStar s1 = stringStar("");

	char c[50];
	int size = StringStar_sizeof(s1);
	cout << endl << *s1 << " has size= " << size << endl;
	StringStar_marshalling(c, &s1);
	cout << *s1 << " is marshalled to " << c << endl;
	StringStar_unmarshalling(&s1, c);
	cout << c << " is unmarshalled to " << *s1 << endl;
	if (StringStar_sizeof(s1) - size != 0) {
		cout << "testStringStar failed";
	} else {
		cout << "testStringStar passed";
	}

}

void testStringQueue_marshalling() {
	StringQueue q;
	StringStar s1 = stringStar("");
	//StringStar s2 = stringStar("");

	add(s1, q);
	//add(s2, q);
	char c[50];
	int size = StringQueue_sizeof(q);
	cout << endl << q << " has size= " << size << endl;
	StringQueue_marshalling(c, &q);
	cout << q << " is marshalled to " << c << endl;
	StringQueue_unmarshalling(&q, c);
	cout << c << " is unmarshalled to " << q << endl;
	if (StringQueue_sizeof(q) - size != 0) {
		cout << "testStringQueue failed " << StringQueue_sizeof(q);
	} else {
		cout << "testStringQueue passed " << StringQueue_sizeof(q);
	}

}

#endif

/***************  ADDRQUEUE ********************/

string ResourcesId2Url(const int& id) {
	if (id == 1) {
		return string("RES_11");
	} else if (id == 2) {
		return string("RES_22");
	}
	return "";
}

void addToResource(ConnQueue& resourceItems, string& parameters,
		const string& url, string& item) {

	list<QI*>::const_iterator iter = getFirstConnIter(resourceItems, url, "",
			-1, -1, "-1");
	if (iter == resourceItems.Q.end()) { //if such conn is not found, return false;
		ConnStar conn = connStar(url, "", -1, -1, getTime());
		addMsgConn(conn, url, item);
		add(conn, resourceItems);
	} else {
		ConnStar conn = (Conn*) *iter;
		addMsgConn(conn, url, item);
		conn->create_time = getTime();
	}
}

void deleteResource(ConnQueue& resourceItems, string& parameters,
		const string& url, string& item) {

	list<QI*>::const_iterator iter = getFirstConnIter(resourceItems, url, "",
			-1, -1, "-1");
	if (iter == resourceItems.Q.end()) { //if such conn is not found, return false;
		ConnStar conn = connStar(url, "", -1, -1, getTime());
		addMsgConn(conn, url, item);
		add(conn, resourceItems);
	} else {
		ConnStar conn = (Conn*) *iter;
		addMsgConn(conn, url, item);
		conn->create_time = getTime();
	}
}

void updateResource(ConnQueue& resourceItems, string& parameters,
		const string& url, string& item) {

	list<QI*>::const_iterator iter = getFirstConnIter(resourceItems, url, "",
			-1, -1, "-1");
	if (iter == resourceItems.Q.end()) { //if such conn is not found, return false;
		ConnStar conn = connStar(url, "", -1, -1, getTime());
		addMsgConn(conn, url, item);
		add(conn, resourceItems);
	} else {
		ConnStar conn = (Conn*) *iter;
		addMsgConn(conn, url, item);
		conn->create_time = getTime();
	}
}

void getFromResource(const ConnQueue& resourceItems, const string& url,
		string& item, int& exists) {

	list<QI*>::const_iterator iter = getFirstConnIter(resourceItems, url, "",
			-1);
	if (iter == resourceItems.Q.end()) { //if such conn is not found, return false;
		exists = 0;
		return;
	} else {
		Conn *conn = (Conn*) *iter;
		exists = 1;
		item = *(conn->reqMsg.Q.front());
	}
}

void updateCache(ConnQueue& cachedItems, const string& url, string& item, int& eTag) {

	list<QI*>::const_iterator iter = getFirstConnIter(cachedItems, url);
	ConnStar conn;
	if (iter == cachedItems.Q.end()) { //if such conn is not found, return false;
		 conn = connStar(url, "",-1,-1,"0");
		addMsgConn(conn, url, item);
		add(conn, cachedItems);
	} else {
		conn = (Conn*) *iter;
		addMsgConn(conn, url, item);
		conn->create_time = int2string(string2int(conn->create_time) + 1);
	}
	eTag=string2int(conn->create_time);
}

void getFromCache(const ConnQueue& cachedItems, const string& url, string& item,
		int& eTag, int& exists, int& eTagOK) {

	list<QI*>::const_iterator iter = getFirstConnIter(cachedItems, url);
	if (iter == cachedItems.Q.end()) { //if such conn is not found, return false;
		exists = 0;
		eTagOK = 0;
		eTag = -1;
		return;
	} else {
		Conn *conn = (Conn*) *iter;
		exists = 1;
		eTagOK = (conn->create_time.compare(int2string(eTag))==0) ? 1 : 0;
		if (eTagOK == 0) {
			eTag = string2int(conn->create_time);
			item = *(conn->reqMsg.Q.front());
		}
	}
}

ConnQueue executeDataQueryCartItems3(const string& query, QQueue& availItems,
		QQueue& cartItems) {
	//1: remove availItems itemId
	//2: insert cartsItems cartId itemId
	//3: select availItems

	ConnQueue q;
	string action, data;
	istringstream iss(query);
	iss >> action;
	iss >> data;

	if (action.compare("insert") == 0) {
		string cartId, itemId;
		iss >> cartId;
		iss >> itemId;
		add(insertDataQuery(cartItems, cartId, itemId), q);
	} else if (action.compare("select") == 0) {
		if (data.compare("availItems") == 0) {
			q = selectDataQuery(availItems, "");
		} else {
			q = selectDataQuery(cartItems, "");
		}
	} else if (action.compare("remove") == 0) {
		string itemId;
		iss >> itemId;
		add(removeDataQuery(availItems, itemId), q);
	}
	return q;
}

void joinStringsWithParameters(string action, string resourceId,
		string parameters, string& actionIDParam) {
	string temp, temp2;
	temp = action + ",";
	temp2 = temp + resourceId;
	temp = temp2 + ",";
	actionIDParam = temp + parameters;
}

void isGet(string url, int& foundGet) {
	size_t found = url.find("/get/");
	foundGet = 0;
	if (found != string::npos) {
		foundGet = 1;
	}
}

void breakTokens(string url, string& controller, string& action,
		string& resourceId) {
	char str[20];
	strcpy(str, url.c_str());

	char* pch;
	string* stringArray = new string[3];
	pch = strtok(str, "/ ");
	int i = 0;
	while (pch != NULL) {
		stringArray[i++] = pch;
		pch = strtok(NULL, "/ ");
	}
	controller = stringArray[0];
	action = stringArray[1];
	resourceId = stringArray[2];
}

void breakTokensWithParameters(string url, string& controller, string& action,
		string& resourceId, string& parameters) {
	char str[20];
	strcpy(str, url.c_str());

	char* pch;
	string* stringArray = new string[4];
	pch = strtok(str, "/ ");
	int i = 0;
	while (pch != NULL) {
		stringArray[i++] = pch;
		pch = strtok(NULL, "/ ");
	}
	controller = stringArray[0];
	action = stringArray[1];
	resourceId = stringArray[2];
	parameters = stringArray[3];
}

// -------------------------------------------------------//
void findPathInParams(const StringQueue& params, string& path) {
	string url = getQueue(params, 0);
	string delimiter = " ";
	size_t pos = url.find(delimiter);
	url.erase(0, pos + delimiter.length());
	pos = url.find(delimiter);
	path = url.substr(0, pos);
}

void splitUrl(string url, string& action, string& path, string& protocol,
		int& eTag) {
	string delimiter = " ";
	size_t pos = 0;
	pos = url.find(delimiter);
	action = url.substr(0, pos);
	url.erase(0, pos + delimiter.length());

	pos = url.find(delimiter);
	path = url.substr(0, pos);
	url.erase(0, pos + delimiter.length());

	pos = url.find(delimiter);
	protocol = url.substr(0, pos);
	url.erase(0, pos + delimiter.length());

	eTag = string2int(url);

}

void joinStrings(string action, string path, string& actionPath) {
	string temp;
	temp = action + ",";
	actionPath = temp + path;
}

void splittoken(string dataMsg, string& action, string& path) {
	string delimiter = ",";
	size_t pos = 0;
	pos = dataMsg.find(delimiter);
	action = dataMsg.substr(0, pos);
	dataMsg.erase(0, pos + delimiter.length());
	path = dataMsg;
}

void splitPath(const string& path, StringQueue& q) {
	string delimiter = " ";
	size_t pos, newpos;
	pos = newpos = 0;
	while (true) {
		newpos = path.find(delimiter, pos);
		if (newpos > path.size()) {
			string *s = stringStar(path.substr(pos));
			add(s, q);
			break;
		}
		string *s = stringStar(path.substr(pos, (newpos - pos)));
		add(s, q);
		pos = newpos + 1;
	}

}

void result2Items(ConnQueue result, StringQueue dataOut) {
	list<QI*>::const_iterator iter, end;
	forlist (result.Q) {
		add(stringStar(int2string(((Conn*) *iter)->id)), dataOut);
	}
}

/***************  ADDRQUEUE ********************/

