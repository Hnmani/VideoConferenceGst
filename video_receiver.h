#include <gst/gst.h>
void *video_receiver(void *arg)
{
    GstElement *pipeline, *source, *rtph264depay, *avdec_h264, *convert, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    GstCaps *caps;

    source = gst_element_factory_make("udpsrc", "source");
    rtph264depay = gst_element_factory_make("rtph264depay", "rtph264depay");
    avdec_h264 = gst_element_factory_make("avdec_h264", "avdec_h264");
    convert = gst_element_factory_make("videoconvert", "convert");
    sink = gst_element_factory_make("autovideosink", "sink");
    caps = gst_caps_new_simple("application/x-rtp",
                               "payload", G_TYPE_INT, 127,
                               NULL);
    // Create empty pipeline
    pipeline = gst_pipeline_new("receiver-pipeline");

    if (!pipeline || !source || !rtph264depay || !avdec_h264 || !convert || !sink)
    {
        g_printerr("Not all elements could be created.\n");
        return NULL;
    }

    g_object_set(source, "port", 5000, NULL);
    g_object_set(source, "caps", caps, NULL);

    // Build the pipeline
    gst_bin_add_many(GST_BIN(pipeline), source, rtph264depay, avdec_h264, convert, sink, NULL);
    if (gst_element_link_many(source, rtph264depay, avdec_h264, convert, sink, NULL) != TRUE)
    {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return NULL;
    }

    // Set the pipeline to playing state
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE)
    {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return NULL;
    }

    // Wait until error or EOS
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    // Parse message
    if (msg != NULL)
    {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg))
        {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;
        default:
            // Should not be reached
            g_printerr("Unexpected message received.\n");
            break;
        }
        gst_message_unref(msg);
    }

    // Free resources
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    exit(0);
}