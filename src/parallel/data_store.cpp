#include "data_store.hpp"

DataStore::DataStore() {
    KV_Datastore = new std::unordered_map<std::string, std::string>();
}

DataStore::~DataStore() {
    std::cout << "\nClearing DataStore..." << std::endl;
    if (!KV_Datastore)
        return;
    delete KV_Datastore;
    KV_Datastore = nullptr;
}


std::string DataStore::getValue(std::string key) {
    if (!KV_Datastore)
        return "NULL";
    std::unordered_map<std::string, std::string>::iterator iter = KV_Datastore->find(key);
    if (iter == KV_Datastore->end())
        return "NULL";
    return iter->second;
}
void DataStore::setValue(std::string key, std::string value) {
    if (KV_Datastore)
        (*KV_Datastore)[key] = value;
}
int DataStore::countKeys() {
    if (!KV_Datastore)
        return 0;
    return KV_Datastore->size();
}
bool DataStore::deleteKey(std::string key) {
    if (!KV_Datastore)
        return false;
    return KV_Datastore->erase(key);
}