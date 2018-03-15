#include <cstdio>
#include <map>
#include "avatarManager.h"

using std::map;
using std::string;

AvatarManager * AvatarManager::instance() {
	static AvatarManager mgr;
	return &mgr;
}

AvatarManager::AvatarManager() {}

AvatarManager::~AvatarManager() {
	for(map<string, Avatar *>::iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

Avatar *AvatarManager::create(const std::string &key,
							  Camera *theCamera, int radius) {
	map<string, Avatar *>::iterator it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate avatar %s\n", key.c_str());
		return it->second;
	}
	Avatar * newavt = new Avatar(key, theCamera, radius);
	it = m_hash.insert(make_pair(key, newavt)).first;
	return it->second;
}


Avatar *AvatarManager::find(const std::string & key) const {
	map<string, Avatar *>::const_iterator it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

void AvatarManager::print() const {
	for(map<string, Avatar *>::const_iterator it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		it->second->print();
}
