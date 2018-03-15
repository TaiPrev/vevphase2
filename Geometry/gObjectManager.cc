#include "gObjectManager.h"
#include "tools.h"

using std::map;
using std::string;

GObjectManager * GObjectManager::instance() {
	static GObjectManager mgr;
	return &mgr;
}

GObjectManager::GObjectManager() {}

GObjectManager::~GObjectManager() {
	for(map<string, GObject *>::iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

GObject *GObjectManager::create(const std::string &key) {
	map<string, GObject *>::iterator it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate GObject %s\n", key.c_str());
		return it->second;
	}
	GObject * newlght = new GObject(key);
	it = m_hash.insert(make_pair(key, newlght)).first;
	return it->second;
}

GObject *GObjectManager::createFromWavefront(const std::string &dir,
											 const std::string &fName) {
	string key = getFilename(dir, fName);
	map<string, GObject *>::iterator it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate GObject %s\n", key.c_str());
		return it->second;
	}
	GObject *newlght = GObject::createFromWavefront(dir, fName);
	it = m_hash.insert(make_pair(key, newlght)).first;
	return it->second;
}


GObject *GObjectManager::find(const std::string & key) const {
	map<string, GObject *>::const_iterator it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

GObject *GObjectManager::find(const std::string & dir,
							  const std::string & fName) const {
	string key = getFilename(dir, fName);
	return find(key);
}

void GObjectManager::print() const {
	for(map<string, GObject *>::const_iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		printf ("Gobj: %s\n", it->first.c_str());
}

GObjectManager::iterator GObjectManager::begin() {
	return GObjectManager::iterator(m_hash.begin());
}

GObjectManager::iterator GObjectManager::end() {
	return GObjectManager::iterator(m_hash.end());
}
