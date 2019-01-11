#ifndef NETWORKDEF_H
#define NETWORKDEF_H

#include <QUrl>
#include <QEvent>
#include <QMap>
#include <QByteArray>
#include <QVariant>

#pragma pack(push, _CRT_PACKING)

// ��ģ��֧�ֵ�Э�飺HTTP(S)/FTP
// ��ģ��֧�ֵ�HTTPЭ�����󷽷���GET/POST/PUT/DELETE/HEAD

enum RequestType
{
	// Download��֧��http��ftp��
	eTypeDownload = 0,
	// Upload��֧��http��ftp��
	eTypeUpload,
	// GET��ʽ����֧��http��ftp��
	eTypeGet,
	// POST��ʽ���󣨽�֧��http��
	eTypePost,
	// PUT��ʽ����֧��http��ftp��
	eTypePut,
	// DELETE��ʽ���󣨽�֧��http������Ҫ������֧�֣�
	eTypeDelete,
	// HEAD��ʽ���󣨽�֧��http������Ҫ������֧�֣�
	eTypeHead,

	eTypeUnknown = -1,
};

//����ṹ
struct RequestTask
{
	// ��������ͣ��ϴ�/����/��������
	RequestType eType;

	// url
	// ע��: ftp�ϴ���url��ָ���ļ���.��"ftp://10.0.192.47:21/upload/test.zip", �ļ���������Ϊtest.zip.
	QUrl url;

	// case eTypeDownload:	���ص��ļ���ŵı���Ŀ¼. (����·�� or ���·��)
	// case eTypeUpload��	���ϴ����ļ�·��. (����·�� or ���·��)
	// case eTypePost��		post�Ĳ���. �磺"a=b&c=d".
	// case eTypePut��		put��������.
	QString strRequestArg;

	// case eTypeDownload: ��ָ����strSaveFileName���򱣴���ļ�����strSaveFileName;���򣬸���url.
	QString strSaveFileName;

	// �û��Զ������ݣ������ڻش���
	QVariant varContent;

	// �����header��Ϣ
	//void QNetworkRequest::setRawHeader(const QByteArray &headerName, const QByteArray &value);
	QMap<QByteArray, QByteArray> mapRawHeader;

	// �Ƿ���ʾ���ȣ�Ĭ��Ϊfalse.
	bool bShowProgress;

	// ����ʧ�ܺ��Ƿ��ٳ�������һ�Σ�Ĭ��Ϊfalse.
	bool bTryAgainWhileFailed;

	// ����������ʧ��һ������ֹ�������Σ�Ĭ��Ϊtrue.
	bool bAbortBatchWhileOneFailed;

	// ���߳�����ģʽ(�������֧��)
	//	 ���߳�����ģʽ�£�һ���ļ��ɶ������ͨ��ͬʱ����.
	//	 ��Ҫ�Ȼ�ȡhttp head��Content-Length��������Ҫ��������֧��.
	bool bMultiDownloadMode;
	// n������ͨ��(Ĭ����5)(ȡֵ��Χ2-10)
	quint16 nDownloadThreadCount;

	//////////////////////���ؽ�����ֶ�/////////////////////////////////////////////
	// �����Ƿ�ɹ�
	bool bSuccess;

	// ���󷵻ص�����/���صĴ�����Ϣ��.
	QByteArray bytesContent;

	// ����ID
	quint64 uiId;
	// ����ID (����������)
	quint64 uiBatchId;

	RequestTask()
	{
		uiId = 0;
		uiBatchId = 0;
		eType = eTypeUnknown;
		bSuccess = false;
		bShowProgress = false;
		bTryAgainWhileFailed = false;
		bAbortBatchWhileOneFailed = true;
		bMultiDownloadMode = false;
		nDownloadThreadCount = 5;
	}
};
Q_DECLARE_METATYPE(RequestTask);
typedef QVector<RequestTask> BatchRequestTask;


////////////////// Event ////////////////////////////////////////////////////
namespace QEventRegister
{
	template <class Type>
	int regiesterEvent(const Type& eventName)
	{
		typedef std::map<Type, int> TUserEventMap;
		static TUserEventMap s_mapUserEvent;

		TUserEventMap::const_iterator iter = s_mapUserEvent.find(eventName);
		if (iter != s_mapUserEvent.cend())
		{
			return iter->second;
		}
		
		int nEventType = QEvent::registerEventType();
		s_mapUserEvent[eventName] = nEventType;
		return nEventType;
	}
}

namespace NetworkEvent
{
	const QEvent::Type WaitForIdleThread	= (QEvent::Type)QEventRegister::regiesterEvent(QLatin1String("WaitForIdleThread"));
	const QEvent::Type ReplyResult			= (QEvent::Type)QEventRegister::regiesterEvent(QLatin1String("ReplyResult"));
	const QEvent::Type NetworkProgress		= (QEvent::Type)QEventRegister::regiesterEvent(QLatin1String("NetworkProgress"));
}

//�ȴ������߳��¼�
class WaitForIdleThreadEvent : public QEvent
{
public:
	WaitForIdleThreadEvent() : QEvent(QEvent::Type(NetworkEvent::WaitForIdleThread)){}
};

//֪ͨ����¼�
class ReplyResultEvent : public QEvent
{
public:
	ReplyResultEvent() : QEvent(QEvent::Type(NetworkEvent::ReplyResult)), bDestroyed(true){}

	RequestTask request;
	bool bDestroyed;
};

//����/�ϴ������¼�
class NetworkProgressEvent : public QEvent
{
public:
	NetworkProgressEvent() : QEvent(QEvent::Type(NetworkEvent::NetworkProgress))
	, bDownload(true)
	, uiId(0)
	, uiBatchId(0)
	, iBtyes(0)
	, iTotalBtyes(0){}

	bool bDownload;
	quint64 uiId;
	quint64 uiBatchId;
	qint64 iBtyes;
	qint64 iTotalBtyes;
};

////////////////׷�����ڴ������ͷ�/////////////////////////////////////////////////////
#define NETWORK_TRACE_CLASS_MEMORY
#ifdef NETWORK_TRACE_CLASS_MEMORY
namespace TraceMemory
{
template<typename T, typename TBase> class ClassIsDerived
{
public:
	static int t(TBase* base)
	{
		return 1;
	}

	static char t(void* t2)
	{
		return 0;
	}

	enum
	{
		Result = (sizeof(int) == sizeof(t((T*)NULL))),
	};
};

typedef std::map<std::string, std::atomic<int>> TClassReferenceCount;
const TClassReferenceCount constructorMap()
{
	static TClassReferenceCount s_mapRcConstructor;
	static TClassReferenceCount s_mapRcDestructor;
	return s_mapRcConstructor;
}

template <typename T>
void addTracedClass()
{
	const char *name = typeid(T).name;
	std::string str(name);
	auto iter = s_mapRcConstructor.find(str);
	if (iter == s_mapRcConstructor.cend())
	{
		s_mapRcConstructor[str] = 0;
	}
	auto iter = s_mapRcDestructor.find(str);
	if (iter == s_mapRcDestructor.cend())
	{
		s_mapRcDestructor[str] = 0;
	}
}

template <typename T>
void addConstructorCount()
{
	const char *name = typeid(T).name;
	std::string str(name);
	auto iter = s_mapRcConstructor.find(str);
	if (iter == s_mapRcConstructor.cend())
	{
	}
}

template <typename T>
void addDestructorCount()
{

}
}
#endif

#pragma pack(pop)

#endif ///NETWORKDEF_H