#include <stdio.h>
#include <stdlib.h> // exit
#include <unistd.h>
#include <limits.h> // PATH_MAX
#include <sys/inotify.h>
#include <pthread.h>
#include <time.h>

#define INOTIFY_EVENT_BASE_SIZE (sizeof (struct inotify_event))
#define BUFFER_SIZE (INOTIFY_EVENT_BASE_SIZE+PATH_MAX)

struct listener {
    int       inotify_desc;
    pthread_t listener_thread;
    void*     (*on_event) (struct inotify_event*);
};

void* listener_listener(void* listener_void_ptr) {

    struct listener* listener_ptr = (struct listener*) listener_void_ptr;

    while (1) {

        char buffer[BUFFER_SIZE] = {0};
        int read_size = read(listener_ptr->inotify_desc, buffer, BUFFER_SIZE);
        if (read_size == -1) {
            pthread_exit((void*) -1);
        }

        listener_ptr->on_event((struct inotify_event*) buffer);
    }

    // actually it will listen forever and never go here
    pthread_exit((void*) 0);

}

int listener_init(struct listener* listener_ptr) {

    int inotify_desc = inotify_init();
    if (inotify_desc == -1) {
        return -1;
    }

    pthread_t listener_thread;
    if (pthread_create(&listener_thread, NULL, listener_listener, (void*) listener_ptr) == -1) {
        return -1;
    }

    listener_ptr->inotify_desc = inotify_desc;
    listener_ptr->listener_thread = listener_thread;

    return 0;

}

int listener_close(struct listener* listener_ptr) {
    // NOTE: It can't destory the watch descriptors.
    int return_val = 0;
    return_val = close(listener_ptr->inotify_desc);
    return_val = pthread_cancel(listener_ptr->listener_thread);
    return return_val;
}

int listener_add_watch(struct listener* listener_ptr, const char* pathname, uint32_t mask) {
    // If need, you must close watch descriptors it results by youself.
    return inotify_add_watch(listener_ptr->inotify_desc, pathname, mask);
}

int listener_rm_watch(struct listener* listener_ptr, int watch_desc) {
    return inotify_rm_watch(listener_ptr->inotify_desc, watch_desc);
}

void* my_on_event(struct inotify_event* event_ptr) {

    printf("event_ptr->wd     = %i\n"    , event_ptr->wd);
    printf("event_ptr->mask   = 0x%X\n"  , event_ptr->mask);
    printf("event_ptr->cookie = 0x%X\n"  , event_ptr->cookie);
    printf("event_ptr->len    = %i\n"    , event_ptr->len);
    printf("event_ptr->name   = \"%s\"\n", event_ptr->name);
    puts("");

    return NULL;

}

int main(int argc, char* argv[]) {

    struct listener listener = {0};
    listener.on_event = &my_on_event;

    if (listener_init(&listener) == -1) {
        perror("listener_init");
        exit(1);
    }

    if (listener_add_watch(&listener, "tmp", IN_CREATE | IN_DELETE | IN_MOVE) == -1) {
        perror("listener_add_watch");
        exit(1);
    }

    while (1) {
        puts("I am awake.");
        sleep(1);
    }

    if (listener_close(&listener) == -1) {
        perror("listener_close");
        exit(1);
    }

    return 0;

}
