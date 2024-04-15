#include "video_sender.h"
#include "video_receiver.h"
#include "audio_receiver.h"
#include "audio_sender.h"
#include <pthread.h>

int main(int argc, char *argv[])
{
    pthread_t video_sender_thread, video_receiver_thread, audio_sender_thread, audio_receiver_thread;
    gst_init(&argc, &argv);
    gchar *host_ip = (gchar *)argv[1];
    pthread_create(&video_sender_thread, NULL, video_sender, (void *)host_ip);
    pthread_create(&video_receiver_thread, NULL, video_receiver, NULL);
    pthread_create(&audio_sender_thread, NULL, audio_sender, (void *)host_ip);
    pthread_create(&audio_receiver_thread, NULL, audio_receiver, NULL);

    pthread_join(video_sender_thread, NULL);
    pthread_join(video_receiver_thread, NULL);
    pthread_join(audio_sender_thread, NULL);
    pthread_join(audio_receiver_thread, NULL);

    return 0;
}