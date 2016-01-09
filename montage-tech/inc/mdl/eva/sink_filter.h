/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __SINK_FILTER_H_
#define __SINK_FILTER_H_

/*
The CBaseRenderer class is a base class for implementing renderer filters.
It supports one input pin, implemented by the CRendererInputPin class. 
To use this class, declare a derived class that inherits CBaseRenderer. 
At a minimum, the derived class must implement the following methods, 
which are declared as pure virtual in the base class: 

CBaseRenderer::CheckMediaType: Accepts or rejects proposed media types.
The filter calls this method during the pin connection process. 
CBaseRenderer::DoRenderSample: Renders a sample. The filter calls this
method for every sample that it receives while running. 
The base class handles state changes and synchronization issues. 
It also schedules samples for rendering, although it does not implement
any quality-control measures. The base class also declares 
several "handler" methods. These are methods that the filter 
calls at specific points in the streaming process. 
They do nothing in the base class, but the derived class can override them. 
In the table that follows, they are listed under the 
heading Public Methods: Handlers.

The CBaseRenderer::OnReceiveFirstSample handler deserves special mention.
The filter calls this method if it receives a sample while the filter is paused.
That can occur if the graph switches from stopped to paused, or if the graph 
is seeked while paused. Video renderers typically use the sample to display
a still frame. When the filter switches from paused to running, it sends the 
same sample to the CBaseRenderer::DoRenderSample method, 
as the first sample in the stream.

The CBaseRenderer class exposes the IMediaSeeking and IMediaPosition 
interfaces through the CRendererPosPassThru object. 
It passes all seek requests to the next filter upstream.

*/

/*!
  private data length
  */
#define SINK_FILTER_PRIVAT_DATA (64)

/*!
  the base filter integrate feature of interface and class
  */
typedef struct tag_sink_filter
{
  /*!
    public base class, must be the first member
    */
  FATHER ifilter_t _filter;

  /*!
    private data buffer
    */
  u8 private_data[SINK_FILTER_PRIVAT_DATA];
}sink_filter_t;


/*!
  create sink filter paramter.
  */
typedef struct tag_sink_filter_para
{
  /*!
    input para.
    */
  u32 dummy;
}sink_filter_para_t;

/*!
  create a sink_filter instance

  \param[in] p_filter instance, if it's NULL, the instance will be create.
  \param[in] p_para init parameter.

  \return return the instance of sink_filter
  */
sink_filter_t * sink_filter_create(sink_filter_t *p_filter,
                                        sink_filter_para_t *p_para);

#endif // End for __SINK_FILTER_H_

