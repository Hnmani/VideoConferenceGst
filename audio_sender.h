#include <gst/gst.h>
void *audio_sender(void *arg)
{
    GstElement *pipeline, *pulsesrc, *audioconvert, *audioresample, *opusenc, *rtpopuspay, *queue, *udpsink;
    GstBus *bus;
    GstMessage *msg;
    GstCaps *caps;

    gchar *host_ip = (gchar *)arg;

    /* Create elements */
    pipeline = gst_pipeline_new("audio-pipeline");
    pulsesrc = gst_element_factory_make("pulsesrc", "pulsesrc");
    audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
    audioresample = gst_element_factory_make("audioresample", "audioresample");
    opusenc = gst_element_factory_make("opusenc", "opusenc");
    rtpopuspay = gst_element_factory_make("rtpopuspay", "rtpopuspay");
    queue = gst_element_factory_make("queue", "queue");
    udpsink = gst_element_factory_make("udpsink", "udpsink");

    if (!pipeline || !pulsesrc || !audioconvert || !audioresample || !opusenc || !rtpopuspay || !queue || !udpsink)
    {
        g_printerr("Not all elements could be created.\n");
        return NULL;
    }

    gst_bin_add_many(GST_BIN(pipeline), pulsesrc, audioconvert, audioresample, opusenc, rtpopuspay, queue, udpsink, NULL);
    gst_element_link_many(pulsesrc, audioconvert, audioresample, opusenc, rtpopuspay, queue, udpsink, NULL);

    g_object_set(udpsink, "host", host_ip, "port", 5004, NULL);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}