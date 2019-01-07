#ifndef NETWORKDEF_H
#define NETWORKDEF_H

#include <QUrl>
#include <QEvent>
#include <QMap>
#include <QByteArray>
#include <QVariant>
#include <QIODevice>
#include <QNetworkRequest>

#pragma pack(push, _CRT_PACKING)

// ��ģ��֧�ֵ�Э�飺HTTP(S)/FTP
// ��ģ��֧�ֵ�HTTPЭ�����󷽷���GET/POST/PUT/DELETE/HEAD/OPTIONS

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
	// ע��: ftp�ϴ���url��ָ���ļ���.��"ftp://10.0.200.47:2121/incoming/test.zip", ��������Ϊtest.zip.
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
	//	 QMap<QNetworkRequest::KnownHeaders, QVariant> mapHeader;
	//	 mapHeader.insert(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;");
	QMap<QNetworkRequest::KnownHeaders, QVariant> mapHeader;

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

	~RequestTask()
	{
		uiId = 0;
		uiBatchId = 0;
	}
};
Q_DECLARE_METATYPE(RequestTask);
typedef QVector<RequestTask> RequestTasks;


////////////////// Event ////////////////////////////////////////////////////
namespace QEventRegister
{
	template <class Type>
	int regiesterEventType(const Type& eventName)
	{
		typedef std::map<Type, int> TEventMap;
		static TEventMap mapCustomEvent;

		TEventMap::const_iterator iter = mapCustomEvent.find(eventName);
		if (iter != mapCustomEvent.end())
		{
			return iter->second;
		}
		else
		{
			int iEventType = QEvent::registerEventType();
			mapCustomEvent[eventName] = iEventType;
			return iEventType;
		}
	}
}

namespace NetworkEvent
{
	const QEvent::Type WaitForIdleThread	= (QEvent::Type)QEventRegister::regiesterEventType(QLatin1String("WaitForIdleThread"));
	const QEvent::Type ReplyResult			= (QEvent::Type)QEventRegister::regiesterEventType(QLatin1String("ReplyResult"));
	const QEvent::Type NetworkProgress		= (QEvent::Type)QEventRegister::regiesterEventType(QLatin1String("NetworkProgress"));
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
public:
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
public:
	bool bDownload;
	quint64 uiId;
	quint64 uiBatchId;
	qint64 iBtyes;
	qint64 iTotalBtyes;
};

#pragma pack(pop)

#endif ///NETWORKDEF_H