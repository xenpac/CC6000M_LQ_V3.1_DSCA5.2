#ifndef EVENT_SINK_H_
#define EVENT_SINK_H_

#if 0 //not used, remove compile warn
// implement by application to receive data base signal
class IDatabaseEventSink {
public:
	virtual void onDatabaseChanged()=0;
};

class IDecodeEventSink {
public:
	virtual void onDecodeFinished(void* context)=0;
	virtual void onDecodeStarted(void*  context)=0;
	virtual void onDecodeAborted(void*  context)=0;
};
#endif//0

// implement by Database or application to receive download Daemon signal
class IDownloadEventSink {
public:
	virtual ~IDownloadEventSink() =0; //for pass warning
	virtual void onDownloadFinished(void* context)=0;
	virtual void onDownloadStarted(void* context)=0;
	virtual void onDownloadAborted(void* context)=0;
};


#endif

