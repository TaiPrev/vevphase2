#include <cstdio>
#include <cstdlib>
#include "tools.h"
#include "materialManager.h"

using std::map;
using std::string;

MaterialManager * MaterialManager::instance() {
	static MaterialManager mgr;
	return &mgr;
}

MaterialManager::MaterialManager() {
	m_mdefault = create(string("_MG_Default_"), string("_MG_Default_"));
}

MaterialManager::~MaterialManager() {
	for(map<string, Material *>::iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

Material *MaterialManager::create(const std::string & lib, const std::string & name) {
	string key = getFilename(lib, name);
	map<string, Material *>::iterator it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate material %s/%s\n", lib.c_str(), name.c_str());
		return it->second;
	}
	Material * newmat = new Material(lib, name);
	it = m_hash.insert(make_pair(key, newmat)).first;
	return it->second;
}

Material *MaterialManager::find(const std::string & lib, const std::string & name) const {
	string key = getFilename(lib, name);
	map<string, Material *>::const_iterator it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

Material *MaterialManager::getDefault() const { return m_mdefault; }

// Debug

void MaterialManager::print() const {
	for(map<string, Material *>::const_iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->print();
}
