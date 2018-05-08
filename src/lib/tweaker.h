//
// Created by demitriy on 5/6/18.
//

#ifndef ACTIO_TWEAKER_H
#define ACTIO_TWEAKER_H

#include "watch.h"
#include <map>
#include <boost/regex.hpp>
#include <string>

#define S1(x) #x
#define S2(x) S1(x)
#define _TV_COUNTER() static int tweak_counter = 0;
#define _TV_WATCH() _TweakWatch(__FILE__)
#define _TV_UPDATE() _reloadTweaks()
#define _TV(Val) _TweakValue(__FILE__, __LINE__, Val)

/**
 *
 * Usage
 *
 * _TV_COUNTER() - in top file
 * _TV_WATCH()   - in any single constructor
 * _TV_UPDATE()  - call refresh values before actions or in update function
 * _TV(1.0f)     - as value
 *
 */

struct TweakedValue {
    float fvalue;
    double dvalue;
    int ivalue;
    std::string svalue;
};

static std::map<std::string, TweakedValue> _tweakValues;

static Watcher tweakWatcher;

static void _TweakWatch(const char *key) {
    tweakWatcher.addFileWatch(key);
}

static float _TweakValue(const char *key, int counter, float origVal) {
    auto it = _tweakValues.find(std::string(key) + std::to_string(counter));
    if (it == _tweakValues.end()) return origVal;

    return it->second.fvalue;
}

static int _TweakValue(const char *key, int counter, int origVal) {
    auto it = _tweakValues.find(std::string(key) + std::to_string(counter));
    if (it == _tweakValues.end()) return origVal;

    return it->second.ivalue;
}

static double _TweakValue(const char *key, int counter, double origVal) {
    auto it = _tweakValues.find(std::string(key) + std::to_string(counter));
    if (it == _tweakValues.end()) return origVal;

    return it->second.dvalue;
}

static void _reloadTweaks() {
    tweakWatcher.check();

    if (tweakWatcher.isChanged()) {
        boost::regex regex_float("^\\d+\\.\\d+f$");
        boost::regex regex_double("^\\d+\\.\\d+$");
        boost::regex regex_int("^\\d+$");
        boost::regex regex_string("^\\\".+?\\\"$");

        const std::vector<std::string> &files = tweakWatcher.getChangedFiles();

        for (const auto&file : files) {
            std::string line;
            std::stringstream ss;
            std::ifstream filestream(file);
            int counter = 0;

            if (filestream.is_open())
            {
                boost::regex regex("_TV\\((.+?)\\)");

                while (getline (filestream, line))
                {
                    boost::sregex_token_iterator iter(line.begin(), line.end(), regex, 0);
                    boost::sregex_token_iterator end;

                    counter++;

                    for( ; iter != end; ++iter ) {
                        std::string key(file);
                        key+= std::to_string(counter);
                        std::string value = *iter;
                        value = value.substr(0, value.size()-1);
                        value = value.substr(4, value.size());

                        auto it = _tweakValues.find(key);
                        if (it == _tweakValues.end()) {
                            _tweakValues.insert({ key, {} });
                            it = _tweakValues.find(key);
                        }

                        boost::smatch res;
                        if (boost::regex_match(value,  res, regex_float)) {
                            it->second.fvalue = std::stof(value);
                        } else if (boost::regex_match(value,  res, regex_double)) {
                            it->second.dvalue = std::stod(value);
                        } else if (boost::regex_match(value,  res, regex_int)) {
                            it->second.ivalue = std::stoi(value);
                        } else {
                            it->second.svalue = value;
                        }
                    }
                }

                filestream.close();
            }
        }
    }
}

#endif //ACTIO_TWEAKER_H
