#ifndef BUFFER_H
#define BUFFER_H




class CythonEntry {
public:
	static void* cEntry;
	CythonEntry(){}
	void putEntry(void* value) {
		CythonEntry::cEntry = value;
	}
	void* getEntry() {
		return CythonEntry::cEntry;
	}
};

class Cy_Dict {
public:
    Cy_Dict() {
    	iter = dMap.begin();
    }
    ~Cy_Dict() {};

    void setObject(std::string key, double value) {
	if(dMap.find(key) == dMap.end()) {    	
		dMap.insert(std::pair<std::string,double>(key, value));
	} else {
		dMap[key] = value;
	}
    }

    void removeObject(std::string key) {
    	std::map<std::string, double>::iterator it = dMap.find(key);
	if(it != dMap.end()) {
  		dMap.erase (it);
	}
    }

    double getObject(std::string key) {
    	if(dMap.find(key) != dMap.end()) {
		return dMap[key];
	}
	return -9999.999;
    }

    void resetIterator() {
        iter = dMap.begin();
    }

    void nextElement() {
    	if(iter != dMap.end()) { 	
		iter++;
	}
    }

    int hasElement(std::string key) {
	if(dMap.find(key) != dMap.end()) {
		return 1;
	}
	return 0;
    }

    std::string getCurrentKey() {
	if(iter != dMap.end()) { 	
		return iter->first;
	}
	return NULL;
    }


    double getCurrentValue() {
	if(iter != dMap.end()) { 	
		return iter->second;
	}
	return -9999.999;
    }

    int getLength() {
	return dMap.size();
    }

    void clear() {
    	dMap.clear();
    }

private:
    std::map<std::string, double> dMap;
    std::map<std::string, double>::iterator iter;
};
#endif
