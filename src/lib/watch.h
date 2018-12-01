//
// Created by demitriy on 5/6/18.
//

#ifndef ACTIO_LIB_WATCH_H
#define ACTIO_LIB_WATCH_H

#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <string>
#include <vector>
#include <map>
#include "console.h"

class Watcher {
public:
    Watcher() {
        fd = inotify_init1(IN_NONBLOCK);

        if (fd < 0) return;
        inited = true;
    }

    explicit Watcher(std::vector<std::string> files) : Watcher() {
        if (inited) {
            for (auto &file : files) {
                addFileWatch(file);
            }
        }
    }

    ~Watcher() {
        console::info("destroy watcher");
        destroy();
    }

    void check() {
        if (!inited) return;

        changedFiles.clear();

        const size_t EVENT_SIZE = (sizeof(struct inotify_event));
        const size_t EVENT_BUF_LEN = (1024 * (EVENT_SIZE + 16));

        char buffer[EVENT_BUF_LEN];

        ssize_t length = read(fd, buffer, EVENT_BUF_LEN);

        if (length == 0) {
            changed = false;
            return;
        }

        if (length == -1) {
            changed = false;
            return;
        }

        int i = 0;
        while (i < length) {
            auto *event = (struct inotify_event *) &buffer[i];

            auto it = fds.find(event->wd);
            if (it != fds.end()) {
                changedFiles.push_back(it->second);
            }

            i += EVENT_SIZE + event->len;
        }

        console::info("changed");
        changed = true;
    }

    void destroy() {
        for (auto const &it : fds) {
            if (it.first >= 0) inotify_rm_watch(fd, it.first);
        }

        fds.clear();
        close(fd);
        fd = -1;
    }

    void addFileWatch(const std::string &file) {
        int fileFd = inotify_add_watch(fd, file.c_str(), IN_MODIFY);
        fds.insert({ fileFd, file });
    }

    const std::vector<std::string> &getChangedFiles() {
        return changedFiles;
    }

    bool isChanged() {
        return changed;
    }

    bool isInited() {
        return inited;
    }

private:
    int fd = -1;
    bool changed = false;
    bool inited = false;
    std::map<int, std::string> fds;
    std::vector<std::string> changedFiles;
};

#endif //ACTIO_LIB_WATCH_H
