#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <mutex>

class DataStore {
	private:
		std::unordered_map<std::string, std::string> *KV_Datastore;
		static DataStore *self;
		static std::mutex instanceMutex; 
		std::mutex dataMutex;
		DataStore();

	public:
		static DataStore *getInstance() {
			if (!self) {
				std::lock_guard<std::mutex> lock(instanceMutex);
				if (!self)
					self = new DataStore();
			}
			return self;
		}

		~DataStore();
		static void deleteInstance() {
			std::lock_guard<std::mutex> lock(instanceMutex);
			if (!self)
				return;
			delete self;
			self = nullptr;
		}
		DataStore(const DataStore&) = delete;
		DataStore& operator=(const DataStore&) = delete;

		std::string getValue(std::string key);
		void setValue(std::string key, std::string value);
		int countKeys();
		bool deleteKey(std::string key);
};
