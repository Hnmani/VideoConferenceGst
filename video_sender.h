#include <gst/gst.h>
void *video_sender(void *arg)
{
    GstElement *pipeline, *v4l2src, *capsfilter, *videoconvert1, *videoconvert2, *x264enc, *rtph264pay, *udpsink, *tee, *queue1, *queue2, *videosink;
    GstBus *bus;
    GstMessage *msg;
    GstCaps *caps;

    gchar *host_ip = (gchar *)arg;

    pipeline = gst_pipeline_new("pipeline");
    v4l2src = gst_element_factory_make("v4l2src", "v4l2src");
    capsfilter = gst_element_factory_make("capsfilter", "capsfilter");
    videoconvert1 = gst_element_factory_make("videoconvert", "videoconvert1");
    videoconvert2 = gst_element_factory_make("videoconvert", "videoconvert2");
    tee = gst_element_factory_make("tee", "tee");
    queue1 = gst_element_factory_make("queue", "queue1");
    queue2 = gst_element_factory_make("queue", "queue2");
    videosink = gst_element_factory_make("autovideosink", "videosink");
    x264enc = gst_element_factory_make("x264enc", "x264enc");
    rtph264pay = gst_element_factory_make("rtph264pay", "rtph264pay");
    udpsink = gst_element_factory_make("udpsink", "udpsink");

    if (!pipeline || !v4l2src || !videoconvert1 || !x264enc || !rtph264pay || !udpsink)
    {
        g_printerr("Not all elements could be created.\n");
        return NULL;
    }
    if (!tee || !queue1 || !queue2 || !videoconvert2 || !videosink || !capsfilter)
    {
        g_printerr("Not new elements could be created\n");
        return NULL;
    }

    /* Set device property for v4l2src */
    g_object_set(v4l2src, "device", "/dev/video0", NULL);

    /* Set encoder properties for x264enc */
    g_object_set(x264enc, "pass", 5, "quantizer", 20, "tune", 0x00000004, NULL);

    /* Set UDP sink properties */
    g_object_set(udpsink, "host", host_ip, "port", 5000, NULL);

    g_object_set(G_OBJECT(capsfilter), "caps",
                 gst_caps_new_simple("video/x-raw",
                                     "width", G_TYPE_INT, 1280,
                                     "height", G_TYPE_INT, 720,
                                     NULL),
                 NULL);

    /* Add elements to the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), v4l2src, capsfilter, tee, videoconvert2, videoconvert1, queue1, queue2, x264enc, rtph264pay, videosink, udpsink, NULL);

    /* Link the elements */
    if (!gst_element_link_many(v4l2src, capsfilter, tee, queue1, videoconvert1, x264enc, rtph264pay, udpsink, NULL))
    {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return NULL;
    }
    if (!gst_element_link_many(tee, queue2, videoconvert2, videosink, NULL))
    {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return NULL;
    }

    /* Set the pipeline to playing state */
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