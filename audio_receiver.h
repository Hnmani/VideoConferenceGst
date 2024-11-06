
#include <gst/gst.h>
void *audio_receiver(void *arg)
{
    GstElement *pipeline, *udpsrc, *queue, *rtpopusdepay, *opusdec, *audioconvert, *autoaudiosink;
    GstBus *bus;
    GstMessage *msg;
    GstCaps *caps;

    /* Create elements */
    pipeline = gst_pipeline_new("audio-pipeline");
    udpsrc = gst_element_factory_make("udpsrc", "udpsrc");
    queue = gst_element_factory_make("queue", "queue");
    rtpopusdepay = gst_element_factory_make("rtpopusdepay", "rtpopusdepay");
    opusdec = gst_element_factory_make("opusdec", "opusdec");
    audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
    autoaudiosink = gst_element_factory_make("autoaudiosink", "autoaudiosink");

    if (!pipeline || !udpsrc || !queue || !rtpopusdepay || !opusdec || !audioconvert || !autoaudiosink)
    {
        g_printerr("Not all elements could be created.\n");
        return NULL;
    }

    /* Set up the pipeline */
    /* Add elements to the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), udpsrc, queue, rtpopusdepay, opusdec, audioconvert, autoaudiosink, NULL);
    /* Link elements */
    gst_element_link_many(udpsrc, queue, rtpopusdepay, opusdec, audioconvert, autoaudiosink, NULL);

    /* Set properties */
    g_object_set(udpsrc, "port", 5004, NULL);
    caps = gst_caps_from_string("application/x-rtp, media=(string)audio, clock-rate=(int)48000, encoding-name=(string)X-GST-OPUS-DRAFT-SPITTKA-00, payload=(int)96");
    g_object_set(udpsrc, "caps", caps, NULL);
    gst_caps_unref(caps);

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Free resources */
    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}
