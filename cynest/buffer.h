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

#endif
