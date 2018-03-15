#include <cstdio>
#include "tools.h"
#include "lightManager.h"

using std::map;
using std::string;

LightManager * LightManager::instance() {
	static LightManager mgr;
	return &mgr;
}

LightManager::LightManager() {}

LightManager::~LightManager() {
	for(map<string, Light *>::iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

Light *LightManager::create(const std::string &key,
							Light::type_t t) {
	map<string, Light *>::iterator it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate light %s\n", key.c_str());
		return it->second;
	}
	Light * newlght = new Light(t);
	it = m_hash.insert(make_pair(key, newlght)).first;
	return it->second;
}


Light *LightManager::find(const std::string & key) const {
	map<string, Light *>::const_iterator it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

void LightManager::switchOn() {

	for(map<string, Light *>::const_iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->switchLight(true);
}

void LightManager::switchOff() {

	for(map<string, Light *>::const_iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->switchLight(false);
}

void LightManager::print() const {
	for(map<string, Light *>::const_iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->print();
}

LightManager::iterator LightManager::begin() { return LightManager::iterator(m_hash.begin()); }
LightManager::iterator LightManager::end() { return LightManager::iterator(m_hash.end()); }
